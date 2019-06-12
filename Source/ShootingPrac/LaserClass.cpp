// Fill out your copyright notice in the Description page of Project Settings.

#include "LaserClass.h"
#include "ConstructorHelpers.h"
#include "Engine.h"

// Sets default values
ALaserClass::ALaserClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinderOptional<UObject> hitWorldObj(TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitWorld"));
	static ConstructorHelpers::FObjectFinderOptional<UObject> trailObj(TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_Trail"));

	if (hitWorldObj.Succeeded())
	{
		hitParticle = (UParticleSystem*)hitWorldObj.Get();
	}

	if (trailObj.Succeeded())
		bulletTrail = (UParticleSystem*)trailObj.Get();

	projMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjMovementComponent"));
	projMovementComponent->InitialSpeed = 5000.0f;
	projMovementComponent->MaxSpeed = 5000.0f;
	projMovementComponent->bRotationFollowsVelocity = true;
	projMovementComponent->bShouldBounce = false;
	projMovementComponent->Bounciness = 0.0f;
	projMovementComponent->ProjectileGravityScale = 0.0f;

	Tags.Add("Projectile");

}

// Called when the game starts or when spawned
void ALaserClass::BeginPlay()
{
	Super::BeginPlay();
	//projMovementComponent->SetUpdatedComponent(RootComponent->GetChildComponent(0));
	projMovementComponent->SetUpdatedComponent(RootComponent);

	collision = (UCapsuleComponent*)RootComponent->GetChildComponent(0);
	collision->BodyInstance.SetCollisionProfileName(TEXT("Proj"));
	collision->OnComponentBeginOverlap.AddDynamic(this, &ALaserClass::OnOverlapBegin);
	UGameplayStatics::SpawnEmitterAttached(bulletTrail, RootComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, collision->GetName());
}

// Called every frame
void ALaserClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALaserClass::FireInDir(const FVector &shootDir, float dmg)
{
	projMovementComponent->Velocity = shootDir * projMovementComponent->InitialSpeed;
	this->damage = dmg;
}

void ALaserClass::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, OtherActor->GetName());

	//if (OtherActor->ActorHasTag("Construction"))
	//{
		UGameplayStatics::SpawnEmitterAtLocation(this, hitParticle, GetActorLocation(), FRotator::ZeroRotator, FVector(5.0f, 5.0f, 5.0f));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, OtherActor->GetName());
		// 총알 탄착점에 피격 파티클 실행 잘 시키기
		//if (SweepResult.Location.X == 0 && SweepResult.Location.Y == 0 && SweepResult.Location.Z == 0)
		//{
		//	UGameplayStatics::SpawnEmitterAtLocation(this, hitParticle, GetActorLocation(), FRotator::ZeroRotator, FVector(5.0f, 5.0f, 5.0f));
		//}
		//else
		//{

		//}
	//}

}

float ALaserClass::GetDamage()
{
	return damage;
}