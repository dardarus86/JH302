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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LineTrace)
	float m_targettable_Range = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CylinderPlacement)
	float b_Can_Cylinder_move_Until_Left_Click = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AbilityBools)
	float b_Ability_1_Pressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AbilityBools)
	float canGroundPound = false;
#pragma endregion 
#pragma region project functions
	void SpawnCylinderAtSetLocation() ;
	void LeftClickSetCylinderBoolFalse();
	void IncreasePlayerGravity();
	void DecreasePlayerGravity();
	void GroundPound();
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
#pragma endregion 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Linetrace)
	FHitResult outHit;
	
	UFUNCTION(BlueprintCallable)
        FVector GetoutHitLineTrace();

	UFUNCTION(BlueprintCallable)
        void F_SpawnCylinder();

	UFUNCTION(BlueprintCallable)
        void F_SpawnSphere();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cylinder)
	TSubclassOf<AMyCylinder> subClasscylinder;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cylinder)
	AMyCylinder* mycylinder;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GroundPound)
	TSubclassOf<AGroundPound> subClassgroundPoundSphere;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GroundPound)
	AGroundPound* groundPoundSphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemies)
	TArray<AEnemies*> enemyArray;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemies)
	TArray<AActor*> enemiesInOverlapEvent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemies)
	TArray<AActor*> enemiesInGroundPoundOverlapEvent;
};

