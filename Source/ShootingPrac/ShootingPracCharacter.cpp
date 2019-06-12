// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShootingPracCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Public/TimerManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AShootingPracCharacter

AShootingPracCharacter::AShootingPracCharacter()
{
	static ConstructorHelpers::FObjectFinderOptional<UObject> muzzleOb(TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_MuzzleFlash"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> firemtg(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Fire_A_Fast_V1_Montage"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> aimMtg(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Idle_Ability_RMB_Montage"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> closeAttackMtg(TEXT("AnimMontage'/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Cast_Montage.Cast_Montage'"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> fireSoundOb(TEXT("/Game/MyAudio/LaserFire"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> boostFX_ob(TEXT("/Game/ParagonWraith/FX/Particles/Abilities/ScopedShot/FX/P_Wraith_Sniper_Stabilizer"));
	static ConstructorHelpers::FClassFinder<AActor> interactiveUI_ob(TEXT("/Game/ThirdPersonCPP/Blueprints/Character/InteractiveUI_Actor"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> hitWorldObj(TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitWorld"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> trailObj(TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_Trail"));

	if (hitWorldObj.Succeeded())
	{
		hitParticle = (UParticleSystem*)hitWorldObj.Get();
	}
	if (trailObj.Succeeded())
		bulletTrail = (UParticleSystem*)trailObj.Get();
	if(muzzleOb.Succeeded())
		muzzleFX = (UParticleSystem*)muzzleOb.Get();
	if (firemtg.Succeeded())
		fireMontage = (UAnimMontage*)firemtg.Get();
	if (aimMtg.Succeeded())
		aimMontage = (UAnimMontage*)aimMtg.Get();
	if (fireSoundOb.Succeeded())
		fireSound = (USoundBase*)fireSoundOb.Get();
	if (boostFX_ob.Succeeded()) boosterFX = (UParticleSystem*)boostFX_ob.Get();
	if (interactiveUI_ob.Succeeded())
		interactiveUI = interactiveUI_ob.Class;
	if (closeAttackMtg.Succeeded())
		closeAttackMontage = (UAnimMontage*)closeAttackMtg.Get();

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	//bUseControllerRotationPitch = true;
	//bUseControllerRotationYaw = true;
	//GetCharacterMovement()->bOrientRotationToMovement = false;

	// bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;
	bUseControllerRotationPitch = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->AirControl = 0.2f;
	//GetCharacterMovement()->bUseControllerDesiredRotation = true;
	

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Weapon Energy
	weaponEnergyCharge = 0.5f;
	weaponEnergyDisCharge = 2.0f;
	WeaponEnergy = 100.0f;
	nowWeaponEnergy = WeaponEnergy;

	// Boost Energy
	boostEnergyCharge = 3.0f;
	boostEnergyDisCharge = 0.5f;
	boostEnergy = 100.0f;
	nowBoostEnergy = boostEnergy;

	// HP
	hp = 100.0f;
	nowHp = hp;

	// Rate
	fireRate = 3.0f;

	// Speed
	walkSpeed = 600.0f;
	boostSpeed = 2000.0f;
	AimingSpeed = 10.0f;

	// Etc

	fireDistance = 10000.0f;
	coverCheckDist = 100.0f;
	coverOffset = 5.0f;
	interactionDist = 1000.0f;
	coverDist = 200.0f;
	closeAtkDist = 250.0f;
	closeCoolDownDelay = 1.0f;
	damage = 30.0f;
	headDamage = 50.0f;
	closeAttackDamage = 50.0f;

	nonAimAccuracy = 7.0f;
	aimAccuracy = 0.5f;

	isDie = false;
	isBoosting = false;
	isAiming = false;
	isFiring = false;
	isWalking = false;
	isCloseCoolDown = false;
	interactiveUI_Instance = NULL;
	prev_InteractableActor = NULL;


	isOutLeft = false, isOutRight = false;
	bReverseCam = true;
	isLeanBody_Left = false;
	isLeanBody_Right = false;
	upperLeanAngle = 30;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AShootingPracCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &AShootingPracCharacter::Aim);
	PlayerInputComponent->BindAction("Aiming", IE_Released, this, &AShootingPracCharacter::Aim);

	PlayerInputComponent->BindAction("LeftLean", IE_Pressed, this, &AShootingPracCharacter::LeanUpperBody_Left);
	PlayerInputComponent->BindAction("LeftLean", IE_Released, this, &AShootingPracCharacter::LeanUpperBody_Left_Release);

	PlayerInputComponent->BindAction("RightLean", IE_Pressed, this, &AShootingPracCharacter::LeanUpperBody_Right);
	PlayerInputComponent->BindAction("RightLean", IE_Released, this, &AShootingPracCharacter::LeanUpperBody_Right_Release);

	PlayerInputComponent->BindAction("Cover", IE_Pressed, this, &AShootingPracCharacter::Cover);
	//PlayerInputComponent->BindAction("Cover", IE_Released, this, &AShootingPracCharacter::ReleaseCover);

	PlayerInputComponent->BindAction("CloseAttack", IE_Pressed, this, &AShootingPracCharacter::CloseAttack);

	PlayerInputComponent->BindAction("ReverseCam", IE_Pressed, this, &AShootingPracCharacter::ReverseCamPos);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShootingPracCharacter::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShootingPracCharacter::StopFire);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AShootingPracCharacter::TurnOnOffBooster);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AShootingPracCharacter::TurnOnOffBooster);

	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &AShootingPracCharacter::Interaction);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShootingPracCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShootingPracCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("TurnRate", this, &AShootingPracCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	//PlayerInputComponent->BindAxis("LookUpRate", this, &AShootingPracCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AShootingPracCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AShootingPracCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AShootingPracCharacter::OnResetVR);
}


void AShootingPracCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AShootingPracCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AShootingPracCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AShootingPracCharacter::MoveForward(float Value)
{
	if (isDie) return;

	if ((Controller != NULL) && (Value != 0.0f) && !isCovered)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

// Ÿ�̸Ӹ� �̿��Ͽ� ������ ���� �� ���� �¿�� �� �����̰� �ϱ�.
void AShootingPracCharacter::MoveRight(float Value)
{
	if (isDie) return;


	if ((Controller != NULL) && (Value != 0.0f) && !isCovered)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
	else if ((Controller != NULL) && (Value != 0.0f) && isCovered)
	{
		if ((isOutLeft && Value < 0) || (isOutRight && Value > 0) || (isMustSit && !isSit)) return;
		FHitResult outHit;
		FCollisionQueryParams collQuery;

		FVector lStart, lEnd;
		lStart = GetMesh()->GetBoneLocation("Pelvis") + FVector(0, 0, -10.0f);
		lStart += coverLeftDir * (box.X / 2);
		lEnd = lStart + (coverFrontDir * coverCheckDist);
		DrawDebugLine(GetWorld(), lStart, lEnd, FColor::Red, true, 3.0f);

		FVector rStart, rEnd;
		rStart = GetMesh()->GetBoneLocation("Pelvis") + FVector(0, 0, -10.0f);
		rStart += coverRightDir * (box.X / 2);
		rEnd = rStart + (coverFrontDir * coverCheckDist);
		DrawDebugLine(GetWorld(), rStart, rEnd, FColor::Blue, true, 3.0f);

		// ����
		if (GetWorld()->LineTraceSingleByChannel(outHit, lStart, lEnd, ECC_GameTraceChannel6, collQuery))
		{
			isOutLeft = false;
		}
		else
		{
			isOutLeft = true;
			if(Value < 0 && bReverseCam)
				ReverseCamPos();
		}
		// ������
		if (GetWorld()->LineTraceSingleByChannel(outHit, rStart, rEnd, ECC_GameTraceChannel6, collQuery))
		{
			isOutRight = false;
		}
		else
		{
			isOutRight = true;
			if(Value > 0 && !bReverseCam)
				ReverseCamPos();
		}

		if ((!isOutLeft && Value < 0) || (!isOutRight && Value > 0))
		{
			AddMovementInput(coverRightDir, Value);
		}

	}

}

void AShootingPracCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Cast<UCapsuleComponent>(RootComponent)->OnComponentBeginOverlap.AddDynamic(this, &AShootingPracCharacter::OnOverlapBegin);

	// ������, ���� ���¿� ī�޶� ��ġ�ؾ� �� ��ġ ���
	sitPoint = UMyHelperClass::GetChildComponentByName<USceneComponent>(RootComponent, "SitPoint");
	standPoint = UMyHelperClass::GetChildComponentByName<USceneComponent>(RootComponent, "StandPoint");

	GetActorBounds(true, org, box);
	muzzle = RootComponent->GetChildComponent(1)->GetChildComponent(2);

	Tags.Add("Hitable");
	Tags.Add("Human");
	Tags.Add("Player");

	if (AimingPoint == NULL)
		AimingPoint = CameraBoom->GetChildComponent(1);
	if (idlePoint == NULL)
	{
		idlePoint = CameraBoom->GetChildComponent(2);
	}
	nowPosition = idlePoint;
	FollowCamera->SetRelativeLocation(idlePoint->RelativeLocation);
	isAiming = false;

	bUseControllerRotationYaw = true;
}

void AShootingPracCharacter::Tick(float delta)
{
	Super::Tick(delta);

	if (isDie) return;

	if (nowWeaponEnergy < WeaponEnergy && !isFiring)
		nowWeaponEnergy += weaponEnergyCharge * delta;
	if (nowWeaponEnergy > WeaponEnergy)
		nowWeaponEnergy = WeaponEnergy;

	if (nowBoostEnergy < boostEnergy && !isBoosting)
		nowBoostEnergy += boostEnergyCharge * delta;
	if (nowBoostEnergy > boostEnergy)
		nowBoostEnergy = boostEnergy;

	// �Ȱ� �ִٸ�
	if (GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 && !isBoosting)
		isWalking = true;
	else
		isWalking = false;

	FindWithLineTrace();

	SetupMuzzleLocationRotation();
	Booster();
}

void AShootingPracCharacter::Aim()
{
	if (isCovered)
	{
		if (bReverseCam && isOutLeft) ReverseCamPos();
		else if (!bReverseCam && isOutRight) ReverseCamPos();
	}

	if (isMustSit)
	{
		if (isSit)
		{
			if (!isOutLeft && !isOutRight)
			{
				isSit = false;
				CameraBoom->SetRelativeLocation(standPoint->RelativeLocation);
			}
		}
		else
		{
			isSit = true;
			CameraBoom->SetRelativeLocation(sitPoint->RelativeLocation);
		}
	}

	ChangeCamera();
}

// ������, ���� ������ ī�޶� ��ȯ�� ���
void AShootingPracCharacter::ChangeCamera()
{
	static FVector pos;

	pos = nowPosition->RelativeLocation;
	if (nowPosition == idlePoint)
	{
		GetCharacterMovement()->MaxWalkSpeed = AimingSpeed;
		isAiming = true;
		bUseControllerRotationYaw = true;
		nowPosition = AimingPoint;
	}
	else if (nowPosition == AimingPoint)
	{
		GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
		isAiming = false;
		nowPosition = idlePoint;
		StopAnimMontage(aimMontage);
	}

	// ���� ������ ���� �ε巯�� ī�޶� ��ȯ ����
	// Ÿ�̸ӿ� ���ٸ� ���
	alpha = 0.0f;
	fDele.BindLambda([&] {
		UpdateCameraPosition(pos, nowPosition->RelativeLocation, alpha);
		alpha += 0.1f;
	});
	GetWorld()->GetTimerManager().SetTimer(hCamera, fDele, 0.01f, true, 0.0f);
}

void AShootingPracCharacter::UpdateCameraPosition(FVector from, FVector to, float ap)
{
	FVector mid = FMath::Lerp<FVector>(from, to, ap);
	FollowCamera->SetRelativeLocation(mid);

	if (ap >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(hCamera);
	}
}



// �÷��̾��� ȭ�� �߻縦 ���.
// 
// ## ��� ����� �ٲ� ��.
// 1. ũ�ν����� ���
// 2. ũ�ν����� �߻� �� �������� ���� ǥ���� ���� �ѱ����� �������� �߻�
// 3. �ش� �ѱ����� �߻� �� �������� �ε��� ǥ������ �������� ������.
void AShootingPracCharacter::Fire()
{
	if (isDie) return;

	isFiring = true;
	
	fFireDele.BindLambda([&] {
		if (nowWeaponEnergy - weaponEnergyDisCharge <= 0 || !isFiring)
		{
			isFiring = false;
			return;
		}
		nowWeaponEnergy -= weaponEnergyDisCharge;

		int angle = 90;
		if (!bReverseCam) angle = -angle;
		UGameplayStatics::SpawnEmitterAttached(muzzleFX, muzzle, NAME_None, FVector::ZeroVector, FRotator(0, angle, 0)/*, FVector(0.01f, 0.01f, 0.01f)*/, EAttachLocation::SnapToTarget, false);

		UGameplayStatics::PlaySoundAtLocation(this, fireSound, muzzle->GetComponentLocation(), FRotator(0, 0,0));
		PlayAnimMontage(fireMontage, 1.0f);

		//
		FHitResult outHit;
		FVector camPos = FollowCamera->GetComponentLocation();
		FVector start = camPos;
		FVector camForward = FollowCamera->GetForwardVector();
		FVector end = (camForward * fireDistance) + start;

		FCollisionQueryParams collParam("Trace", true, this);

		// ���� Ʈ���̽� ä���� �ٸ������� �ٲ㼭 ���� �����
		// �ڲ� �ѱ����� ������
		if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_GameTraceChannel2, collParam))
		{
			FVector originHitPos = outHit.Location;
			FVector hitPos = outHit.Location;
			FVector normal = (originHitPos - muzzle->GetComponentLocation()).GetUnsafeNormal();
			FRotator change = normal.Rotation();

			float limit;
			if (isAiming) limit = aimAccuracy;
			else limit = nonAimAccuracy;

			float yaw = change.Yaw;
			float pitch = change.Pitch;
			change.Yaw = FMath::RandRange(yaw - limit, yaw + limit);
			change.Pitch = FMath::RandRange(pitch - limit, pitch + limit);

			hitPos = change.Vector() * 10000.0f + muzzle->GetComponentLocation();

			FHitResult outHitFromMuzzle;
			if (GetWorld()->LineTraceSingleByChannel(outHitFromMuzzle, muzzle->GetComponentLocation(), hitPos, ECC_GameTraceChannel2, collParam))
			{
				UParticleSystemComponent *particle = UGameplayStatics::SpawnEmitterAtLocation(this, hitParticle, outHitFromMuzzle.Location, FRotator::ZeroRotator);
				particle->SetRelativeScale3D(FVector(1.2f, 1.2f, 1.2f));

				if (!outHit.GetActor()->ActorHasTag("Hitable")) return;

				AActor *actor = outHitFromMuzzle.GetActor();

				if (actor->IsA(AConstruction::StaticClass()))
				{
					Cast<AConstruction>(actor)->ReceiveDamage(damage);
				}
				else if (actor->IsA(AAI_Character::StaticClass()))
				{
					float finalDamage = damage;
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, outHitFromMuzzle.BoneName.ToString());
					
					if (outHitFromMuzzle.BoneName.ToString() == "Head")
						finalDamage = headDamage;
					Cast<AAI_Character>(actor)->ReceiveDamage(finalDamage);
				}
			}

		} 
	});

	GetWorld()->GetTimerManager().SetTimer(hFire, fFireDele, 0.1f, true, 0.0f);
}

// ũ�ν��� ����� ���� �� �Ѿ���
// �ش� ��ǥ�� ���߽�Ű�� ���� ĳ���� ����
void AShootingPracCharacter::RevisionCharAngle()
{
	FHitResult outHit;
	FVector camPos = FollowCamera->GetComponentLocation();
	FVector start = camPos;
	FVector camForward = FollowCamera->GetForwardVector();
	FVector end = (camForward * 10000.0f) + start;
	FVector camToChar = muzzle->GetComponentLocation() - camPos;

	FCollisionQueryParams collParam;

	// ũ�ν����� �����Ͽ� �������� ��ġ�� Ÿ���� ã�Ƴ�
	// "ũ�ν���� -> Ÿ��" ���Ϳ� "ī�޶� -> �ѱ�" ������ ���� ���� 
	// �߻� ������ ���
	// �� ������ ������ ĳ���͸� ȸ���ϰ�, �������� �߻縦 �Ѵ�.
	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collParam))
	{
		FVector muzzleToPoint = outHit.Location - muzzle->GetComponentLocation();
		muzzle_to_point = muzzleToPoint;

		float ms = muzzle->GetForwardVector().Size2D();
		FVector2D muzzleNV (muzzle->GetForwardVector().X / ms, muzzle->GetForwardVector().Y / ms);
		float cs = camToChar.Size2D();
		FVector2D camToCharNV(camToChar.X / cs, camToChar.Y / cs);

		// ������ ���� ���հ��� ���Ͽ� �� ����ŭ ĳ���͸� ȸ����Ų��.
		float angle = FVector2D::DotProduct(muzzleNV, camToCharNV);

		RootComponent->AddWorldRotation(FRotator(0, angle, 0));
	}
}

void AShootingPracCharacter::SetupMuzzleLocationRotation()
{
	muzzle->SetWorldLocation(GetMesh()->GetSocketLocation("gun_barrel"));
	muzzleRot = GetMesh()->GetSocketRotation("gun_barrel");
}

void AShootingPracCharacter::StopFire()
{
	isFiring = false;
	PlayAnimMontage(aimMontage, 1.0f);
}

void AShootingPracCharacter::TurnOnOffBooster()
{
	if (isBoosting)
	{
		GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
		isBoosting = false;
	}
	else
	{
		if (boostEnergy > 0)
		{
			isBoosting = true;
			GetCharacterMovement()->MaxWalkSpeed = boostSpeed;
		}
	}
}

void AShootingPracCharacter::Booster()
{
	if (nowBoostEnergy - boostEnergyDisCharge <= 0) 
	{
		isBoosting = false;
		GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
		return;
	}
	else if (isBoosting && !isAiming)
	{
		UGameplayStatics::SpawnEmitterAttached(boosterFX, this->GetMesh(), "FX_piston_l", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
		nowBoostEnergy -= boostEnergyDisCharge;
	}
}

// FŰ�� ������ �� ȣ��Ǵ� ��ȣ�ۿ� �Լ�.
void AShootingPracCharacter::Interaction()
{
	// GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collParam)
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Interaction");

	FHitResult outHit;
	FCollisionQueryParams collQuery;
	FVector start, end;
	FVector camForward = FollowCamera->GetForwardVector();

	start = FollowCamera->GetComponentLocation();
	end = start + (camForward * interactionDist);

	DrawDebugLine(GetWorld(), start, end, FColor::Red, true, 3.0f);
	// ĳ������ ��ȣ�ۿ��� ������ �Ÿ� ���� �ִ��� �˻�
	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collQuery))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Interaction Detect");
		// ��ü�� �Ÿ����� �����Ѵٸ� 
		// ��ü�� ������ ���� ���� �ٸ� ��ȣ�ۿ� �ൿ�� ȣ���Ѵ�.
		if (outHit.Actor->ActorHasTag("MapDevice"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, "DEVICE Detect");
			((AMapDevice*)(outHit.GetActor()))->ActiveDevice();
		}
	}

	return;
}

float AShootingPracCharacter::GetHealthPoint()
{
	return nowHp;
}

float AShootingPracCharacter::GetWeaponEnergy()
{
	return nowWeaponEnergy;
}

float AShootingPracCharacter::GetBoostEnergy()
{
	return nowBoostEnergy;
}


void AShootingPracCharacter::AddHealthPoint(float val)
{
	if (val + nowHp > hp)
		nowHp = hp;
	else
		nowHp += val;
}

void AShootingPracCharacter::AddWeaponEnergy(float val)
{
	if (val + nowWeaponEnergy > WeaponEnergy)
		nowWeaponEnergy = WeaponEnergy;
	else
		nowWeaponEnergy += val;
}
	
void AShootingPracCharacter::AddBoostEnergy(float val)
{
	if (val + nowBoostEnergy > boostEnergy)
		nowBoostEnergy = boostEnergy;
	else
		nowBoostEnergy += val;
}

// �� �����Ӹ��� ��ȣ�ۿ� ������ ��ġ, �������� �ִ��� �˻��Ѵ�.
void AShootingPracCharacter::FindWithLineTrace()
{
	FHitResult outHit;
	FCollisionQueryParams collQuery;
	FVector start, end;
	FVector camForward = FollowCamera->GetForwardVector();

	// �˻� ������ ���۰� ���� ���� ī�޶��� ��ġ, ī�޶��� ��ġ + �˻�Ÿ��� �����Ѵ�.
	start = FollowCamera->GetComponentLocation();
	end = start + (camForward * interactionDist);

	// �˻����� ��, ��ġ�� �������� �ִٸ�
	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collQuery))
	{
		// �ش� ��ü�� �̸���, UI�� ��� ���� ���ڿ��� �����´�.
		if (outHit.Actor->ActorHasTag("Interactable"))
		{
			interactiveStatus = ((AMapDevice*)outHit.GetActor())->GetInteractiveString();
		}
		else if (outHit.Actor->ActorHasTag("Construction"))
		{
			interactiveStatus = ((AConstruction*)outHit.GetActor())->GetInteractiveString();
		}
		// �ٸ� ��ü��� ���� ���ڿ��� ���� ó���Ѵ�.
		else
			interactiveStatus = "";
	}
	// ���� �ߴٸ� ���� ���ڿ��� ����ó���Ѵ�.
	else
	{
		interactiveStatus = "";
	}
}

void AShootingPracCharacter::Cover()
{
	// ����Ű (L Ctrl)�� ������
	// ũ�⿡ ���� ������, �������� ���Ѵ� (ũ��� �ش� ������ �ٿ��� �÷��̾� �ٿ���� ���ݳ��̿� ���Ѵ�.)
	// ���� �ɾҴٸ�, �÷��̾� ĸ�� �ݶ��̴��� ���-�ϴ� ���̸� ��������(96.0f -> 48.0f)�� ���̰�, ���� ��� �� ������ ������ �Ѵ�.
	// ������ �Ѵٸ� ���� ��ü�� ���ĳ����´�.
	// ����� �Ѵ�.
	// ������ ���� �ٽ� ���� ������ ��ü�� �������´�
	// CtrlŰ�� �ѹ� �� ������ ���� ���°� Ǯ����.
	// ���� Ʈ���̽��� ������ �������κ��� �ش� ������ ���� ���� * �Ÿ� ��ŭ ������, �÷��̾� ��ġ
	// �ش� ���� ������ �ִ� ����, ������ �޾ƿ� �����Ѵ�

	if (isCovered)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Release");
		isOutLeft = isOutRight = false;
		isCovered = false;
		isSit = false;
		isMustSit = false;
		coverConstruction = nullptr;
		CameraBoom->SetRelativeLocation(standPoint->RelativeLocation);
		return;
	}

	FHitResult outHit;
	FCollisionQueryParams collQuery;
	FVector start, end;
	FVector Forward = FollowCamera->GetForwardVector();

	start = GetActorLocation();
	end = start + (Forward * coverDist);

	DrawDebugLine(GetWorld(), start, end, FColor::Red, true, 3.0f);
	// ĳ������ ��ȣ�ۿ��� ������ �Ÿ� ���� �ִ��� �˻�

	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_GameTraceChannel6, collQuery))
	{
		if (!outHit.Actor->ActorHasTag("Coverable")) return;

		isCovered = true;
		DrawDebugLine(GetWorld(), outHit.Location, outHit.Location + outHit.Normal * 100, FColor::Purple, false, 5.0f);
		FVector newPos = outHit.Location;
		newPos += outHit.Normal * coverOffset;
		coverFrontDir = outHit.Normal * -1;
		SetActorLocation(newPos);

		coverLeftDir = outHit.Normal.RotateAngleAxis(90.0f, FVector::UpVector);
		coverRightDir = outHit.Normal.RotateAngleAxis(-90.0f, FVector::UpVector);
		DrawDebugLine(GetWorld(), outHit.Location, outHit.Location + coverLeftDir * 100, FColor::Green, false, 5.0f);
		DrawDebugLine(GetWorld(), outHit.Location, outHit.Location + coverRightDir * 100, FColor::Green, false, 5.0f);

		FVector origin, box;
		outHit.Actor.Get()->GetActorBounds(true, origin, box);
		FVector pOrigin, pBox;
		GetActorBounds(true, pOrigin, pBox);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Structure : %f Player : %f"), box.Z, pBox.Z));

		coverConstruction = Cast<AConstruction>(outHit.GetActor());
		if (box.Z < pBox.Z)
		{
			isSit = true;
			isMustSit = true;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Sit");
			CameraBoom->SetRelativeLocation(sitPoint->RelativeLocation);
		}
		else
		{
			isSit = false;
			isMustSit = false;
			CameraBoom->SetRelativeLocation(standPoint->RelativeLocation);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Stand");
		}

	}
}

void AShootingPracCharacter::ReleaseCover()
{
	if (!isCovered) return;

	isOutLeft = isOutRight = false;
	isCovered = false;
}

void AShootingPracCharacter::LeanUpperBody_Left()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Left Lean");

	if (!isLeanBody_Right && !isLeanBody_Left)
	{
		if (bReverseCam && upperLeanAngle > 0)
			upperLeanAngle = -upperLeanAngle;
		else if (!bReverseCam && upperLeanAngle < 0)
			upperLeanAngle = -upperLeanAngle;
		isLeanBody_Left = true;
	}
}

void AShootingPracCharacter::LeanUpperBody_Right()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Right Lean");

	if (!isLeanBody_Right && !isLeanBody_Left)
	{
		if (bReverseCam && upperLeanAngle < 0)
			upperLeanAngle = -upperLeanAngle;
		else if (!bReverseCam && upperLeanAngle > 0)
			upperLeanAngle = -upperLeanAngle;
		isLeanBody_Right = true;
	}
}

void AShootingPracCharacter::LeanUpperBody_Left_Release()
{
	isLeanBody_Left = false;
}

void AShootingPracCharacter::LeanUpperBody_Right_Release()
{
	isLeanBody_Right = false;
}

// VŰ�� ���� ���� ����
// ���� ������ ���� ���ݿ� ���� ü�� �Ҹ� �߰��� ��.
void AShootingPracCharacter::CloseAttack()
{
	if (isCloseCoolDown) return;
	isCloseCoolDown = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "CloseAttack");

	FHitResult outHit;
	FCollisionQueryParams collQuery;
	FVector start, end;
	FVector forward = GetActorForwardVector();
	
	start = GetActorLocation();
	end = start + (forward * closeAtkDist);
	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_GameTraceChannel2, collQuery))
	{
		AActor *actor = outHit.GetActor();
		if (actor->IsA(AConstruction::StaticClass()))
		{
			Cast<AConstruction>(actor)->ReceiveDamage(closeAttackDamage);
		}
		else if (actor->IsA(AAI_Character::StaticClass()))
		{
			Cast<AAI_Character>(actor)->ReceiveDamage(closeAttackDamage);
		}
	}

	DrawDebugLine(GetWorld(), start, end, FColor::Orange, true, 3.0f);

	float animLen = PlayAnimMontage(closeAttackMontage, 2.0f);

	GetWorld()->GetTimerManager().SetTimer(hCloseAttack,
		[&] {
		if(isAiming)
			PlayAnimMontage(aimMontage, 1.0f);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "ASD");

	}, 1.0f, false, animLen);


	fCloseAttackDele.BindLambda([&]{
		isCloseCoolDown = false;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "TT");

	});
	GetWorld()->GetTimerManager().SetTimer(hCloseAttack, fCloseAttackDele, 1.0f, false, closeCoolDownDelay);
}

void AShootingPracCharacter::ReverseCamPos()
{
	static FVector cPos;

	cPos = nowPosition->RelativeLocation;
	bReverseCam = !bReverseCam;
	FVector temp;
	temp = GetMesh()->GetComponentScale(); temp.X *= -1;
	GetMesh()->SetRelativeScale3D(temp);
	temp = idlePoint->RelativeLocation;  temp.Y *= -1;
	idlePoint->SetRelativeLocation(temp);
	temp = AimingPoint->RelativeLocation; temp.Y *= -1;
	AimingPoint->SetRelativeLocation(temp);
	temp = standPoint->RelativeLocation; temp.Y *= -1;
	standPoint->SetRelativeLocation(temp);
	temp = sitPoint->RelativeLocation; temp.Y *= -1;
	sitPoint->SetRelativeLocation(temp);

	alpha = 0.0f;
	fDele.BindLambda([&] {
		UpdateCameraPosition(cPos, nowPosition->RelativeLocation, alpha);
		alpha += 0.1f;
	});
	GetWorld()->GetTimerManager().SetTimer(hCamera, fDele, 0.01f, true, 0.0f); // 0.01;
}

void AShootingPracCharacter::CheckDead()
{
	if (hp <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, "DEAD");
	}
}

void AShootingPracCharacter::ReceiveDamage(float dmg)
{
	nowHp -= dmg;
	if (nowHp <= 0.0f)
	{
		// ReleaseTimer();
		isDie = true;
	}
}

void AShootingPracCharacter::DetectCoverDestroy()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, "CALL");
	isCovered = false;
	isSit = false;
	coverConstruction = nullptr;
}

bool AShootingPracCharacter::GetPlayerDieStatus()
{
	return isDie;
}

bool AShootingPracCharacter::GetPlayerArriveStatus()
{
	return isArrived;
}

void AShootingPracCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Exit"))
	{
		isArrived = true;
	}
}

void AShootingPracCharacter::ReleaseTimer()
{
	if (hFire.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(hFire);
	if (hCamera.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(hCamera);
	if (hCloseAttack.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(hCloseAttack);
}