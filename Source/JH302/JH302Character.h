// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Engine/Engine.h>
#include <Components/StaticMeshComponent.h>
#include <components/PrimitiveComponent.h>
#include "GroundPound.h"
#include "MyCylinder.h"
#include "Enemies.h"
#include "JH302Character.generated.h"

UCLASS(config=Game)
class AJH302Character : public ACharacter
{
	GENERATED_BODY()
#pragma region Base functions
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
#pragma endregion
public:
	AJH302Character();
#pragma region Base functions
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
#pragma endregion

protected:

#pragma region project bools
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AbilityBools)
	bool b_Ability_1_Pressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AbilityBools)
	bool canGroundPound = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AbilityBools)
    bool b_isLeftMouseDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AbilityBools)
	bool b_isCharging = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AbilityBools)
	bool b_isGroundPounding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AbilityBools)
	bool b_isCastingLightning = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AbilityBools)
	bool b_isRightMouseDown = false;
#pragma endregion
#pragma region project variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LineTrace)
	float m_targettable_Range = 2000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cylindervariable)
	float CylinderScale = 0.2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cylindervariable)
	float timeScale = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GroundPound)
	float playerDistanceFromGround;
	
#pragma endregion
#pragma region project functions
	UFUNCTION(BlueprintCallable)
	void LightningBlast();
	UFUNCTION(BlueprintCallable)
	void GroundPoundPlayerGravityIncrease();
	UFUNCTION(BlueprintCallable)
	void GroundPound();
	UFUNCTION(BlueprintCallable)
	void SetleftMouseClickBoolTrue();
	UFUNCTION(BlueprintCallable)
	void SetleftMouseClickBoolFalse();
	UFUNCTION(BlueprintCallable)
	void PlayerCharge();
	UFUNCTION(BlueprintCallable)
	void CanPlayerCharge();
	UFUNCTION(BlueprintCallable)
	void CanChangeGlobalTimeDilation();
	UFUNCTION(BlueprintCallable)
	void CheckIfRightMouseDownAndTimeDilate();
	UFUNCTION(BlueprintCallable)
	void CheckIfChargingAndSetWalkSpeed();
	UFUNCTION(BlueprintCallable)
	void CheckLeftMouseDownAndReScaleCylinder();
	UFUNCTION(BlueprintCallable)
	void CheckIfCanGroundPoundAndDoIfTrue();
	UFUNCTION(BlueprintCallable)
	void UpdateCylinderPosition();
	UFUNCTION(BlueprintCallable)
        FVector GetoutHitLineTrace();
	UFUNCTION(BlueprintCallable)
        void F_SpawnCylinder();
	UFUNCTION(BlueprintCallable)
        void F_SpawnSphere();
#pragma endregion
#pragma region project properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Linetrace)
	FHitResult outHit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cylinder)
	AMyCylinder* mycylinder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GroundPound)
	AGroundPound* groundPoundSphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemies)
	TArray<AActor*> enemiesInOverlapEvent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemies)
	TArray<AActor*> enemiesInGroundPoundOverlapEvent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemies)
	TArray<AActor*> enemiesInChargeOverlap;
#pragma endregion 
#pragma region Base functions
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

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
#pragma endregion

public:

#pragma region Base functions
	virtual void Tick(float DeltaTime) override;
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* chargeCollisionMesh;

	UPROPERTY(EditAnywhere)
	UParticleSystem* particleGroundPound;
	UPROPERTY(EditAnywhere)
	UParticleSystem* particleCylinder;
	UPROPERTY(EditAnywhere)
	UParticleSystem* particleCharge;
	
	TSubclassOf<class UParticleSystem> Particle;
#pragma endregion 
};

