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

// 타이머를 이용하여 완전히 앉을 때 까지 좌우로 못 움직이게 하기.
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

		// 왼쪽
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
		// 오른쪽
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

	// 비조준, 조준 상태에 카메라가 위치해야 할 위치 등록
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

	// 걷고 있다면
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

// 비조준, 조준 상태의 카메라 전환을 담당
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

	// 선형 보간을 통해 부드러운 카메라 전환 구현
	// 타이머에 람다를 사용
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



// 플레이어의 화기 발사를 담당.
// 
// ## 사격 방식을 바꿀 것.
// 1. 크로스헤어에서 쏜다
// 2. 크로스헤어에서 발사 됀 레이저가 맞은 표적을 향해 총구에서 레이저를 발사
// 3. 해당 총구에서 발사 됀 레이저가 부딪힌 표적에게 데미지를 입힌다.
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

		// 여기 트레이스 채널을 다른것으로 바꿔서 적을 맞출것
		// 자꾸 총구에서 끝난다
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

// 크로스헤어에 사격을 했을 때 총알이
// 해당 목표에 명중시키기 위한 캐릭터 보정
void AShootingPracCharacter::RevisionCharAngle()
{
	FHitResult outHit;
	FVector camPos = FollowCamera->GetComponentLocation();
	FVector start = camPos;
	FVector camForward = FollowCamera->GetForwardVector();
	FVector end = (camForward * 10000.0f) + start;
	FVector camToChar = muzzle->GetComponentLocation() - camPos;

	FCollisionQueryParams collParam;

	// 크로스헤어에서 시작하여 일직선상에 위치한 타겟을 찾아냄
	// "크로스헤어 -> 타겟" 벡터와 "카메라 -> 총구" 벡터의 차를 통해 
	// 발사 방향을 계산
	// 이 방향을 가지고 캐릭터를 회전하고, 방향으로 발사를 한다.
	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collParam))
	{
		FVector muzzleToPoint = outHit.Location - muzzle->GetComponentLocation();
		muzzle_to_point = muzzleToPoint;

		float ms = muzzle->GetForwardVector().Size2D();
		FVector2D muzzleNV (muzzle->GetForwardVector().X / ms, muzzle->GetForwardVector().Y / ms);
		float cs = camToChar.Size2D();
		FVector2D camToCharNV(camToChar.X / cs, camToChar.Y / cs);

		// 내적을 통해 사잇값을 구하여 그 값만큼 캐릭터를 회전시킨다.
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

// F키를 눌렀을 때 호출되는 상호작용 함수.
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
	// 캐릭터의 상호작용이 가능한 거리 내에 있는지 검사
	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collQuery))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Interaction Detect");
		// 개체가 거리내에 존재한다면 
		// 개체의 종류에 따라 각각 다른 상호작용 행동을 호출한다.
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

// 매 프레임마다 상호작용 가능한 장치, 아이템이 있는지 검사한다.
void AShootingPracCharacter::FindWithLineTrace()
{
	FHitResult outHit;
	FCollisionQueryParams collQuery;
	FVector start, end;
	FVector camForward = FollowCamera->GetForwardVector();

	// 검사 지점의 시작과 끝을 각각 카메라의 위치, 카메라의 위치 + 검사거리로 지정한다.
	start = FollowCamera->GetComponentLocation();
	end = start + (camForward * interactionDist);

	// 검사했을 때, 장치나 아이템이 있다면
	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collQuery))
	{
		// 해당 개체의 이름과, UI에 띄울 상태 문자열을 가져온다.
		if (outHit.Actor->ActorHasTag("Interactable"))
		{
			interactiveStatus = ((AMapDevice*)outHit.GetActor())->GetInteractiveString();
		}
		else if (outHit.Actor->ActorHasTag("Construction"))
		{
			interactiveStatus = ((AConstruction*)outHit.GetActor())->GetInteractiveString();
		}
		// 다른 개체라면 상태 문자열을 공백 처리한다.
		else
			interactiveStatus = "";
	}
	// 실패 했다면 상태 문자열을 공백처리한다.
	else
	{
		interactiveStatus = "";
	}
}

void AShootingPracCharacter::Cover()
{
	// 엄폐키 (L Ctrl)을 누른다
	// 크기에 따라 앉을지, 서있을지 정한다 (크기는 해당 엄폐물의 바운드와 플레이어 바운드의 절반높이와 비교한다.)
	// 만약 앉았다면, 플레이어 캡슐 콜라이더의 상단-하단 길이를 절반으로(96.0f -> 48.0f)로 줄이고, 역의 경우 이 과정을 역으로 한다.
	// 조준을 한다면 몸의 상체만 빼꼼내놓는다.
	// 사격을 한다.
	// 조준을 떼면 다시 빼곰 내놓은 상체를 돌려놓는다
	// Ctrl키를 한번 더 누르면 엄폐 상태가 풀린다.
	// 라인 트레이스가 성공한 지점으로부터 해당 액터의 법선 벡터 * 거리 만큼 벌린후, 플레이어 배치
	// 해당 면이 가지고 있는 시작, 끝점을 받아와 엄폐한다

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
	// 캐릭터의 상호작용이 가능한 거리 내에 있는지 검사

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

// V키를 눌러 근접 공격
// 추후 적에게 근접 공격에 대한 체력 소모를 추가할 것.
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