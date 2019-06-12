// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// My Classes
#include "LaserClass.h"
#include "MapDevice.h"
#include "MyHelperClass.h"

#include "Construction.h"
#include "AI_Character.h"

//

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "ShootingPracCharacter.generated.h"


class AAI_Character;
class AConstruction;

UCLASS(config=Game)
class AShootingPracCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	USceneComponent *idlePoint;
	USceneComponent *AimingPoint;
	USceneComponent *standPoint;
	USceneComponent *sitPoint;
	USceneComponent *nowPosition;
	FTimerHandle hFire;
	FTimerHandle hCloseAttack;
	FTimerHandle hCamera;
	FTimerDelegate fCloseAttackDele;
	FTimerDelegate fDele;
	FTimerDelegate fFireDele;

	float alpha;
	float beta;

public:
	AShootingPracCharacter();
	void ReceiveDamage(float dmg);
	void DetectCoverDestroy();
	void ReleaseTimer();
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		FRotator muzzleRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent *muzzle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<class ALaserClass> projClass;



	FVector muzzle_to_point;

		
protected: 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float boostEnergy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float WeaponEnergy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float hp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float nowWeaponEnergy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float nowBoostEnergy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float nowHp;

	UPROPERTY(BlueprintReadWrite)
		float fireRate;

	UPROPERTY(BlueprintReadWrite)
		FString interactiveStatus;

	UPROPERTY(BlueprintReadWrite)
		bool isSit;

	UPROPERTY(BlueprintReadWrite)
		bool isLeanBody_Left;

	UPROPERTY(BlueprintReadWrite)
		bool isLeanBody_Right;

	UPROPERTY(BlueprintReadWrite)
		float upperLeanAngle;

	UPROPERTY(BlueprintReadOnly)
		bool isCovered;

	UPROPERTY(BlueprintReadOnly)
		bool isDie;

	bool isArrived;

	float walkSpeed;
	float AimingSpeed;
	float boostSpeed;

	bool isOutLeft, isOutRight;
	bool isMustSit;
	bool bReverseCam;
	FVector org, box;
	AConstruction *coverConstruction;

	float nonAimAccuracy;
	float aimAccuracy;

	float coverCheckDist;
	float coverOffset;
	float coverDist;
	float interactionDist;
	float closeAtkDist;
	float closeCoolDownDelay;
	bool isCloseCoolDown;

	float boostEnergyCharge;
	float boostEnergyDisCharge;
	float weaponEnergyCharge;
	float weaponEnergyDisCharge;

	float fireDistance;
	float closeAttackDamage;
	float headDamage;
	float damage;
	float boostRestore;
	float weaponRestore;

	FVector coverLeftDir, coverRightDir, coverFrontDir;

	UPROPERTY()
		TSubclassOf<AActor> interactiveUI;
	AActor* interactiveUI_Instance;
	AActor* prev_InteractableActor;
	FVector prevPos;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool isFiring;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool isAiming;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool isBoosting;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool isWalking;

	USkeletalMeshSocket *boostpipe;
	USoundBase *fireSound;
	UAnimMontage *fireMontage;
	UAnimMontage *aimMontage;
	UAnimMontage *closeAttackMontage;
	UParticleSystem *muzzleFX;
	UParticleSystem *boosterFX;
	UParticleSystem *hitParticle;
	UParticleSystem *bulletTrail;
public:
	
	//TSubclassOf<class UParticleSystem*> muzzleFX;

protected:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void Cover();
	void ReleaseCover();
	void TurnOnOffBooster();
	void Booster();
	void CloseAttack();
	void ReverseCamPos();
	void CheckDead();

	void FindWithLineTrace();

	void ChangeCamera();
	void Aim();

	void LeanUpperBody_Left();
	void LeanUpperBody_Left_Release();
	void LeanUpperBody_Right();
	void LeanUpperBody_Right_Release();

	void UpdateCameraPosition(FVector, FVector, float);

	void BeginPlay() override;

	void Tick(float delta) override;

	void Fire();

	void StopFire();

	//void ReceiveDamage(float dmg);

	void RevisionCharAngle();

	void Interaction();

	void SetupMuzzleLocationRotation();

	
	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	float GetHealthPoint();
	float GetWeaponEnergy();
	float GetBoostEnergy();

	void SetHealthPoint(float val);
	void SetWeaponEnergy(float val);
	void SetBoostEnergy(float val);

	void AddHealthPoint(float val);
	void AddWeaponEnergy(float val);
	void AddBoostEnergy(float val);

	bool GetPlayerDieStatus();
	bool GetPlayerArriveStatus();
};