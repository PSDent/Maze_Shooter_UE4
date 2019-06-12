// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "ShootingPracCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationSystem.h"
#include "Engine/TargetPoint.h"
#include "NavigationPath.h"
#include "SphereTrigger.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "AIModule/Classes/AIController.h"
#include "Construction.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/HUD.h"
#include "BluePrint/UserWidget.h"
#include "AI_Character.generated.h"

class AShootingPracCharacter;
class AConstruction;

UCLASS()
class SHOOTINGPRAC_API AAI_Character : public ACharacter
{
	GENERATED_BODY()

public:  
	// Sets default values for this character's properties
	AAI_Character();
	void ReceiveDamage(float dmg);
	void DetectCoverDestroy();
	void ReleaseTimer();
	UBlackboardComponent* GetBlackBoard();

public:

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable)
		void Attack();
	UFUNCTION(BlueprintCallable)
		void Patrol();
	UFUNCTION(BlueprintCallable)
		void Cover();
	UFUNCTION(BlueprintCallable)
		void FindPlayer();
	UFUNCTION(BlueprintCallable)
		void Chasing();
	UFUNCTION(BlueprintCallable)
		void Move();
	UFUNCTION(BlueprintCallable)
		void Return();
	UFUNCTION(BlueprintCallable)
		void FindCoverPlace();
	UFUNCTION(BlueprintCallable)
		void FindFirePosition();
	UFUNCTION(BlueprintCallable)
		void CallNearTeam();

	void Revision_hpWidget();

	void Fire();
	void Die();
	void RevisionRotationLookAtPlayer();
	void Stand();
	void Sit();

	void ReturnToCover();
	void GoToPlayer();

	FVector CalculateDest();
	
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = UI)
	//	class UUserWidget *hpWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		TSubclassOf<AActor> hpPlateWidget;

	UWidgetComponent *hpWidget;

	UBlackboardComponent *blackBoard;

	UPROPERTY(BlueprintReadOnly)
		bool isWalk;

	UPROPERTY(BlueprintReadOnly)
		bool isBoost;

	UPROPERTY(BlueprintReadOnly)
		bool isSit;

	UPROPERTY(BlueprintReadOnly)
		float angle;

protected:
	const FName key_targetObject = "TargetObject";
	const FName key_targetLocation = "TargetLocation";
	const FName key_prevTargetLocation = "PrevTargetLocation";
	const FName key_isMoving = "isMoving";
	const FName key_isArrived = "isArrived";
	const FName key_isChasing = "isChasing";
	const FName key_isFiring = "isFiring";
	const FName key_isPatrol = "isPatrol";
	const FName key_isNowFiring = "isNowFiring";
	const FName key_isDie = "isDie";
	const FName key_isCover = "isCovered";
	const FName key_isFindingCover = "isFindingCover";
	const FName key_isHit = "isHit";
	const FName key_isSeePlayer = "isSeePlayer";

protected:
	// reference
	TArray<AAI_Character*> aiArray;
	AShootingPracCharacter *playerRef;
	AConstruction *coverConstruction;

	// Timer 
	FTimerHandle hFire;
	FTimerHandle hFireTerm;
	FTimerHandle hExitCover;
	FTimerHandle hDie;
	FTimerHandle hGoToPlayer;

	FTimerDelegate goToPlayerDele;
	FTimerDelegate fireDele;
	FTimerDelegate deadDele;

	// Effects & AnimMontage
	USkeletalMeshSocket *boostpipe;
	USoundBase *fireSound;
	UAnimMontage *fireMontage;
	UAnimMontage *aimMontage;
	UAnimMontage *closeAttackMontage;
	UAnimMontage *die_front, *die_back;
	UParticleSystem *muzzleFX;
	UParticleSystem *boosterFX;
	UParticleSystem *hitParticle;

	USceneComponent *muzzleScene;
	UAudioComponent *fireAudioComponent;
	USoundCue *fireAudioCue;

	//
	FVector playerPos;
	FVector org, box;
	FVector coverPos;
	FVector coverAttackPos;

	UPROPERTY(BlueprintReadOnly)
		bool isDie;

	// Patrol
	bool isReached;
	float patrolRadius;
	float patrolLimitDist;
	FVector patrolPos;
	FVector homePos;
	FVector nextPos;
	ASphereTrigger *homePoint, *patrolPoint, *coverSchedulePoint;

	// Chase
	float chaseDist;

	// AI Stat
	UPROPERTY(BlueprintReadOnly)
		float hp;
	int remainAmmo;
	int ammo;
	float boostSpeed;
	float walkSpeed;
	float fireRate;
	float disappearDelay;
	float hitRandomRad;
	float fireRange;
	float fireTermDelay;
	float damage;
	bool isTerm;

	// Cover

	int maxFindCoverCnt;
	float coverCheckDist;
	float coverOffset;
	float coverDist;
	float interactionDist;
	float closeAtkDist;
	float closeCoolDownDelay;
	bool isCloseCoolDown;

	bool isFindingCover;

	float CallTeamDist;
	float exitCoverTime;
	float maxFindCoverDist;
	float maxCoverDist;

	float fireDist;
	float combatDist;

	FVector muzzle;
	FVector coverLeftDir, coverRightDir, coverFrontDir;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
