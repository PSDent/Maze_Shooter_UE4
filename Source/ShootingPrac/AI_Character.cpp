// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_Character.h"

// Sets default values
AAI_Character::AAI_Character()
{
	static ConstructorHelpers::FObjectFinderOptional<UObject> muzzleOb(TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_MuzzleFlash"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> firemtg(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Fire_A_Fast_V1_Montage"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> aimMtg(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Idle_Ability_RMB_Montage"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> fireSoundOb(TEXT("/Game/MyAudio/LaserFire"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> boostFX_ob(TEXT("/Game/ParagonWraith/FX/Particles/Abilities/ScopedShot/FX/P_Wraith_Sniper_Stabilizer"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> die_frontOb(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Death_Forward"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> die_backOb(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Death_Backward"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> hitWorldObj(TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitWorld"));
	static ConstructorHelpers::FObjectFinder<USoundCue> fireSoundObj(TEXT("/Game/ThirdPersonCPP/Blueprints/Sound/FireSound_soundCue"));

	if (muzzleOb.Succeeded())
		muzzleFX = (UParticleSystem*)muzzleOb.Get();
	if (firemtg.Succeeded())
		fireMontage = (UAnimMontage*)firemtg.Get();
	if (aimMtg.Succeeded())
		aimMontage = (UAnimMontage*)aimMtg.Get();
	if (fireSoundOb.Succeeded())
		fireSound = (USoundBase*)fireSoundOb.Get();
	if (boostFX_ob.Succeeded()) 
		boosterFX = (UParticleSystem*)boostFX_ob.Get();
	if (die_frontOb.Succeeded())
		die_front = (UAnimMontage*)die_frontOb.Get();
	if (die_backOb.Succeeded())
		die_back = (UAnimMontage*)die_backOb.Get();
	if (hitWorldObj.Succeeded())
		hitParticle = (UParticleSystem*)hitWorldObj.Get();

	if (fireSoundObj.Succeeded())
		fireAudioCue = fireSoundObj.Object;

	fireAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FireAudioComp"));
	fireAudioComponent->bAutoActivate = false;
	fireAudioComponent->AttachTo(RootComponent);

	PrimaryActorTick.bCanEverTick = true;

	muzzleScene = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleScene"));
	muzzleScene->SetupAttachment(RootComponent);

	//if (hpPlateWidget)
	//{
	//	hpWidget = CreateWidget<UUserWidget>(GetGameInstance(), hpPlateWidget);
	//	hpWidget->AddToViewport();
	//	hpWidget->SetVisibility(ESlateVisibility::Visible);

	//}


	patrolRadius = 700.0f;
	patrolLimitDist = 10000.0f;
	chaseDist = 5000.0f;
	fireDist = 4000.0f;
	fireRate = 0.1f;
	hp = 100.0f;

	maxFindCoverDist = 1000;
	maxCoverDist = 1100;
	exitCoverTime = 3.0f;

	remainAmmo = ammo;
	ammo = 30;
	fireRange = 30000.0f;
	boostSpeed = 1000.0f;
	walkSpeed = 600.0f;
	disappearDelay = 3.0f;
	hitRandomRad = 1.0f; //5.0f
	damage = 2.0f;
	fireTermDelay = 3.0f;
	maxFindCoverCnt = 50;
	CallTeamDist = 1000.0f;

	isFindingCover = false;
	isReached = false;
	isDie = false;
	isTerm = false;
}

// Called when the game starts or when spawned
void AAI_Character::BeginPlay()
{
	Super::BeginPlay();


	Tags.Add("Hitable");
	Tags.Add("Enemy");

	homePos = GetActorLocation();
	patrolPos = CalculateDest();
	nextPos = homePos;

	fireAudioComponent->SetRelativeLocation(GetMesh()->GetBoneLocation("gun_barrel", EBoneSpaces::ComponentSpace));
	if (fireAudioComponent->IsValidLowLevelFast())
		fireAudioComponent->SetSound(fireAudioCue);

	FActorSpawnParameters spawnInfo;
	homePoint = GetWorld()->SpawnActor<ASphereTrigger>(homePos, FRotator::ZeroRotator, spawnInfo);
	patrolPoint = GetWorld()->SpawnActor<ASphereTrigger>(patrolPos, FRotator::ZeroRotator, spawnInfo);

	blackBoard = UAIBlueprintHelperLibrary::GetBlackboard(this);
	Cast<UCapsuleComponent>(RootComponent)->OnComponentBeginOverlap.AddDynamic(this, &AAI_Character::OnOverlapBegin);
	Cast<UCapsuleComponent>(RootComponent)->OnComponentEndOverlap.AddDynamic(this, &AAI_Character::OnOverlapEnd);

	GetActorBounds(true, org, box);
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	blackBoard->SetValueAsBool(key_isArrived, true);
	blackBoard->SetValueAsBool(key_isDie, false);

	for (TActorIterator<AShootingPracCharacter> actorIter(GetWorld()); actorIter; ++actorIter)
		playerRef = *actorIter;

	for (TActorIterator<AAI_Character> actorIter(GetWorld()); actorIter; ++actorIter)
	{
		if(*actorIter != this)
			aiArray.Add(*actorIter);
	}

	hpWidget = UMyHelperClass::GetChildComponentByName<UWidgetComponent>(RootComponent, "HP_Widget");


}

// Called every frame
void AAI_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()->Velocity.Size2D() > 0)
	{
		isWalk = true;
		if (!blackBoard->GetValueAsBool(key_isMoving))
		{
			blackBoard->SetValueAsBool(key_isMoving, true);
		}
	}
	else
	{
		isWalk = false;
		if (blackBoard->GetValueAsBool(key_isMoving))
		{
			blackBoard->SetValueAsBool(key_isMoving, false);
		}
	}

	Revision_hpWidget();
}

void AAI_Character::Attack()
{
	Cast<AAIController>(Controller)->StopMovement();
	blackBoard->SetValueAsBool(key_isNowFiring, true);
	GetWorld()->GetTimerManager().SetTimer(hFire, this, &AAI_Character::Fire, fireRate, true, 0.0f);

	FHitResult outHit;
	FVector start = GetActorLocation(), end ;

	Stand();

	//GetWorld()->LineTraceSingleByChannel(outHit, start, )
}

// 일정 수 발사 후, 쿨타임을 두기
void AAI_Character::Fire()
{
	bool isFiring = blackBoard->GetValueAsBool(key_isFiring);
	if (!isFiring)
	{
		blackBoard->SetValueAsBool(key_isNowFiring, false);
		GetWorld()->GetTimerManager().ClearTimer(hFire);
		return;
	}
	else if (remainAmmo >= ammo) 
	{
		Sit();
		ReturnToCover();

		remainAmmo = 0;
		isTerm = true;
		GetWorld()->GetTimerManager().ClearTimer(hFire);

		GetWorld()->GetTimerManager().SetTimer(hFireTerm, [&] {
			isTerm = false;
			Stand();
			GoToPlayer();
			GetWorld()->GetTimerManager().SetTimer(hFire, this, &AAI_Character::Fire, fireRate, true, 0.0f);
		}
		, 0.1f, false, fireTermDelay);

		return;
	}

	++remainAmmo;
	PlayAnimMontage(fireMontage, 1.0f);

	muzzle = GetMesh()->GetBoneLocation("gun_barrel");
	FRotator dir = GetActorRotation();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), muzzleFX, muzzle, dir/*, FRotator(0, 90, 0)*/);

	fireAudioComponent->Play();

	FHitResult outHit;
	FVector start = muzzle;
	FVector end = playerPos - muzzle;
	FVector endNormal = end.GetUnsafeNormal();
	end = endNormal;
	FRotator change = end.Rotation();

	//change.Pitch += 5.0f;
	float yaw = change.Yaw;
	float pitch = change.Pitch;
	change.Yaw = FMath::RandRange(yaw - hitRandomRad, yaw + hitRandomRad);
	change.Pitch = FMath::RandRange(pitch - hitRandomRad, pitch + hitRandomRad);

	end = change.Vector() * fireRange + start;

	FCollisionQueryParams collParam;

	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collParam))
	{
		FVector hitPos = outHit.Location;
		UParticleSystemComponent *particle = UGameplayStatics::SpawnEmitterAtLocation(this, hitParticle, hitPos, FRotator::ZeroRotator, FVector(5.0f, 5.0f, 5.0f));
		particle->SetRelativeScale3D(FVector(0.7f, 0.7f, 0.7f));


		if (!outHit.GetActor()->ActorHasTag("Hitable")) return;

		AActor *actor = outHit.GetActor();

		if (actor->IsA(AConstruction::StaticClass()))
		{
			Cast<AConstruction>(actor)->ReceiveDamage(damage);
		}
		else if (actor->IsA(AShootingPracCharacter::StaticClass()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, "Hit Player");

			Cast<AShootingPracCharacter>(actor)->ReceiveDamage(damage);
		}
	}
}

// 반지름이 r인 원주에서 무작위로 위치를 선정 (sin, cos)
// 해당 위치로 가는게 너무 멀면 다음 위치를 찾는다.
// 위치가 선정되면 해당 위치로 이동한다. 
// 위치에 도착하면 N초간 멈춘 후 다시 다음 순찰지로 이동한다
// 만일 순찰도중 플레이어를 만나면 추적을 하는데,
// 추격 거리 이상으로 벗어나면 다시 원래자리로 돌아오게한다.

void AAI_Character::Patrol()
{
	if (nextPos == homePos)
	{
		nextPos = patrolPos;
	}
	else if (nextPos == patrolPos)
	{
		nextPos = homePos;
	}
	blackBoard->SetValueAsBool(key_isArrived, false);
	blackBoard->SetValueAsBool(key_isPatrol, true);

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "GOGO");
	Cast<AAIController>(Controller)->MoveToLocation(nextPos);
}

FVector AAI_Character::CalculateDest()
{
	UNavigationPath * path;
	UNavigationSystemV1 *navSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FVector patrolLocation;

	float pathLen;
	do
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, "Calculating");

		float val = FMath::FRandRange(0, 360);
		float dy = FMath::Sin(val) * patrolRadius;
		float dx = FMath::Cos(val) * patrolRadius;
		FVector nowPos = GetActorLocation();

		patrolLocation = FVector(nowPos.X + dx, nowPos.Y + dy, 1000);

		FVector start = patrolLocation, end = patrolLocation;
		end.Z = 0;
		FHitResult outHit;
		FCollisionQueryParams collQuery;
		if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collQuery))
		{
			patrolLocation = outHit.Location;
		}
	
		path = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), patrolLocation);
		if (!path->IsValid()) //continue;
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, "You Failed");
			pathLen = patrolLimitDist + 1;
			continue;
		}

		pathLen = path->GetPathLength();

	} while (pathLen > patrolLimitDist);



	return patrolLocation;
}


void AAI_Character::FindPlayer()
{
	if (isDie) return;

	if (playerRef)
	{
		float Distance = GetDistanceTo(playerRef);

		FVector start = GetActorLocation(), end = playerRef->GetActorLocation();
		FHitResult outHit;
		FCollisionQueryParams collParam;

		//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 6.0f);
		if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collParam))
		{
			if (outHit.GetActor()->IsA(AShootingPracCharacter::StaticClass()))
			{
				if (outHit.GetActor()->ActorHasTag("Player"))
				{
					blackBoard->SetValueAsBool(key_isSeePlayer, true);
				}
			}
			else
			{
				blackBoard->SetValueAsBool(key_isSeePlayer, false);
			}

		}

		bool res = blackBoard->GetValueAsBool(key_isSeePlayer) || blackBoard->GetValueAsBool(key_isFiring) || blackBoard->GetValueAsBool(key_isHit);

		if (!res) return;

		if (Distance <= chaseDist)
		{
			playerPos = playerRef->GetActorLocation();

			// AI가 플레이어를 향해 바라본다
			bool isChasing = blackBoard->GetValueAsBool(key_isChasing);
			bool isFiring = blackBoard->GetValueAsBool(key_isFiring);
			bool canLookPlayer = (isChasing || isFiring) && !isFindingCover;

			if (canLookPlayer)
			{
				FRotator lookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), playerRef->GetActorLocation());
				FRotator rotToPlayer = lookAtRot;

				angle = GetActorRotation().Pitch - rotToPlayer.Pitch;
				rotToPlayer.Pitch = 0.0f;
				SetActorRotation(rotToPlayer);
			}

			blackBoard->SetValueAsBool(key_isPatrol, false);

			FVector prevVector = blackBoard->GetValueAsVector(key_targetLocation);
			blackBoard->SetValueAsVector(key_prevTargetLocation, prevVector);
			blackBoard->SetValueAsVector(key_targetLocation, playerRef->GetActorLocation());
			blackBoard->SetValueAsObject(key_targetObject, playerRef);

			if (Distance <= fireDist)
			{
				blackBoard->SetValueAsBool(key_isChasing, false);
				blackBoard->SetValueAsBool(key_isFiring, true);
			}
			else
			{
				blackBoard->SetValueAsBool(key_isChasing, true);
				blackBoard->SetValueAsBool(key_isFiring, false);
			}

		}
		else
		{
			blackBoard->SetValueAsObject(key_targetObject, NULL);
			blackBoard->SetValueAsBool(key_isChasing, false);
			blackBoard->SetValueAsBool(key_isFiring, false);
		}
	}
	else
	{
		angle = 0;
	}
}

void AAI_Character::Chasing()
{
	AActor *target = (AActor*)blackBoard->GetValueAsObject(key_targetObject);
	blackBoard->SetValueAsBool(key_isPatrol, false);
	blackBoard->SetValueAsBool(key_isChasing, true);
	Cast<AAIController>(Controller)->MoveToActor(target);
}

void AAI_Character::Move()
{

}

void AAI_Character::Cover()
{

}

void AAI_Character::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, "Trigger");

	if (OtherActor->ActorHasTag("CoverTrigger"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Overlaped Cover Trigger");

		// 멈춰있고 엄폐물을 찾는상태로 되있다면 엄폐키를 true
		if (blackBoard->GetValueAsBool(key_isFindingCover))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "CoverTrigger Complete");

			isFindingCover = false;
			blackBoard->SetValueAsBool(key_isFindingCover, false);
			blackBoard->SetValueAsBool(key_isCover, true);

			Sit();
		}
	}
	else if (OtherActor->ActorHasTag("Trigger")  && (OtherActor == homePoint || OtherActor == patrolPoint))
	{
		blackBoard->SetValueAsBool(key_isArrived, true);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Overlaped");
	}
}

void AAI_Character::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Trigger") && ((OtherActor == homePoint) || (OtherActor == patrolPoint)))
	{
		blackBoard->SetValueAsBool(key_isArrived, false);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Out");
	}
}

void AAI_Character::Return()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Return");
	Cast<AAIController>(Controller)->MoveToLocation(homePos);
	nextPos = homePos;
}

void AAI_Character::ReceiveDamage(float dmg)
{
	hp -= dmg;
	blackBoard->SetValueAsBool(key_isHit, true);
	if (hp <= 0 && !isDie)
	{
		Die();
	}
}

void AAI_Character::Die()
{
	isDie = true;
	blackBoard->SetValueAsBool(key_isDie, true);
	blackBoard->SetValueAsBool(key_isFiring, false);
	blackBoard->SetValueAsBool(key_isPatrol, false);
	blackBoard->SetValueAsBool(key_isChasing, false);
	if (coverConstruction)
		coverConstruction->SetCoverer(nullptr);
	
	Cast<UCapsuleComponent>(RootComponent)->SetCollisionProfileName("NoCollision");
	Cast<UCapsuleComponent>(RootComponent)->UpdateCollisionProfile();

	Cast<AAIController>(Controller)->StopMovement();
	ReleaseTimer();


	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, die_front->GetName());

	
	deadDele.BindLambda([&] {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Time Out");

		Destroy();
	});
	GetWorld()->GetTimerManager().SetTimer(hDie, deadDele, 0.1f, false, disappearDelay);
}

// 반드시 주변에 엄폐물이 없을시 탐색을 중지하는 로직을 추가할 것.
// 그렇지 않으면 무한 루프로 인해서 게임이 뻑간다.
void AAI_Character::FindCoverPlace()
{
	UNavigationPath * path;
	UNavigationSystemV1 *navSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FVector coverLocation;
	bool isFindCoverPlace = false;

	int cnt = 0;

	blackBoard->SetValueAsBool(key_isHit, false);
	float pathLen = maxCoverDist + 1;
	do
	{
		++cnt;

		if (cnt >= 50)
		{
			blackBoard->SetValueAsBool(key_isFindingCover, false);
			blackBoard->SetValueAsBool(key_isCover, true);
			isFindingCover = false;
			isFindCoverPlace = false;
			return;
		}

		float val = FMath::FRandRange(0, 359);
		float dist = FMath::FRandRange(0, maxFindCoverDist);
		float dy = FMath::Sin(val) * dist;
		float dx = FMath::Cos(val) * dist;
		FVector nowPos = GetActorLocation();

		// 지상으로 찍을 공중의 지점을 구한다
		coverLocation = FVector(nowPos.X + dx, nowPos.Y + dy, 1000);

		FVector start = coverLocation, end = coverLocation;
		end.Z = 0;
		FHitResult outHit;
		FCollisionQueryParams collQuery;

		// 지상으로 레이저를 쏴서 지점을 구한다
		if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collQuery))
		{
			bool res = outHit.GetActor()->ActorHasTag("Coverable") || outHit.GetActor()->ActorHasTag("Wall");
			if (res) continue;

			FVector coverStart = outHit.Location;
			FVector coverEnd = playerRef->GetActorLocation();

			// 지상의 점으로부터 플레이어위치로 레이저를 쏜다
			DrawDebugLine(GetWorld(), coverStart, coverEnd, FColor::Red, false, 6.0f);
			if (GetWorld()->LineTraceSingleByChannel(outHit, coverStart, coverEnd, ECC_Visibility, collQuery))
			{
				// 엄폐 가능한 구조물이 중간에 감지되었다면
				if (outHit.Actor->ActorHasTag("Coverable"))
				{
					FVector lastStart = outHit.Location;

					// 감지된 위치에서 플레이어 위치로 레이저를 한 번 더 쏴서 막을 수 있는 장소인지 확인한다.
					if (GetWorld()->LineTraceSingleByChannel(outHit, lastStart, coverEnd, ECC_Visibility, collQuery))
					{
						// 막을 수 없다면 다시 탐색한다
						if (outHit.Actor->ActorHasTag("Player"))
						{
							continue;
						}
						// 막을 수 있다면 이제 해당 장소에 자리를 잡고 다음 행동으로 착수한다.
						else if (outHit.Actor->ActorHasTag("Coverable"))
						{
							coverLocation = outHit.Location;
							coverConstruction = Cast<AConstruction>(outHit.GetActor());
							isFindCoverPlace = true;
						}
					}
				}
			}
		}
		
		path = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), coverLocation, NULL);
		if (!path->GetPath() || !path->GetPath()->IsValid() || path->GetPath()->IsPartial()) //continue;
		{
			pathLen = maxCoverDist + 1;
			coverConstruction = nullptr;
			isFindCoverPlace = false;
			continue;
		}
		pathLen = path->GetPathLength();

	} while (pathLen > maxCoverDist && !isFindCoverPlace);

	isFindingCover = true;
	Cast<AAIController>(Controller)->MoveToLocation(coverLocation);
	blackBoard->SetValueAsBool(key_isFindingCover, true);

	FActorSpawnParameters spawnInfo;

	coverPos = coverLocation;
	coverSchedulePoint = GetWorld()->SpawnActor<ASphereTrigger>(coverLocation, FRotator::ZeroRotator, spawnInfo);
	coverSchedulePoint->Tags.Add("CoverTrigger");
	coverSchedulePoint->SetActorScale3D(FVector(3.0f, 3.0f, 3.0f));
	if (coverConstruction)
		coverConstruction->SetCoverer(this);
}

void AAI_Character::RevisionRotationLookAtPlayer()
{

}

void AAI_Character::Sit()
{
	if (!coverConstruction)
	{
		return;
	}

	FVector origin, bound;

	coverConstruction->GetActorBounds(true, origin, bound);
	if (bound.Z >= box.Z / 2)
		isSit = true;
}

void AAI_Character::Stand()
{
	isSit = false;
}

void AAI_Character::FindFirePosition()
{
	if (isSit) return;
}

void AAI_Character::DetectCoverDestroy()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, "Bye Bye");

	isSit = false;
	blackBoard->SetValueAsBool(key_isCover, false);
	coverSchedulePoint->Destroy();
	coverConstruction->Destroy();
	FindCoverPlace();
}

void AAI_Character::CallNearTeam()
{
	for (int i = 0; i < aiArray.Num(); ++i)
	{
		float dist = GetDistanceTo(aiArray[i]);
		if (dist <= CallTeamDist)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "CallNearTeam");
			aiArray[i]->GetBlackBoard()->SetValueAsBool(key_isFiring, true);
		}
	}
}

void AAI_Character::Revision_hpWidget()
{
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(hpWidget->GetComponentLocation(), playerRef->GetActorLocation());
	hpWidget->SetWorldRotation(rot);
}

void AAI_Character::ReleaseTimer()
{
	if(hFire.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(hFire);
	if (hFireTerm.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(hFireTerm);
	if (hExitCover.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(hExitCover);
	if (hDie.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(hDie);
	if(hGoToPlayer.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(hGoToPlayer);
}

UBlackboardComponent* AAI_Character::GetBlackBoard()
{
	return blackBoard;
}

void AAI_Character::ReturnToCover()
{
	if (!coverConstruction || isSit) return;

	Cast<AAIController>(Controller)->MoveToLocation(coverPos);
}

// 요부분이 뭔가 살짝 이상한 것 같다..
// 가끔 사격중이지 않을 때 돌아가지 않는 AI가 보임.
void AAI_Character::GoToPlayer()
{
	if (!coverConstruction || isSit) return;

	Cast<AAIController>(Controller)->MoveToLocation(playerPos);

	GetWorld()->GetTimerManager().SetTimer(hGoToPlayer, [&] {

		FVector start = GetActorLocation();
		FVector end = playerPos;
		FHitResult outHit;
		FCollisionQueryParams collParam;

		if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collParam))
		{
			bool rs = outHit.GetActor()->GetClass() == coverConstruction->GetClass();
			if (!rs)
			{
				if(Controller)
					Cast<AAIController>(Controller)->StopMovement();
				GetWorld()->GetTimerManager().ClearTimer(hGoToPlayer);
			}
		}
	}
	, 0.2f, true, 0.0f);
}

//void AAI_Character::BeginDestroy()
//{
//	//ReleaseTimer();
//}