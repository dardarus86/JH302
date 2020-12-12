// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SkeletalMeshComponent.h>
#include <Components/WidgetComponent.h>
#include "Components/WidgetComponent.h"
#include "Materials/Material.h"
#include "Enemies.generated.h"

UCLASS()
class JH302_API AEnemies : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemies();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* skeletalMesh;

	UFUNCTION(BlueprintCallable)
	void EnemyTakeDamage(float damage);
	UFUNCTION(BlueprintCallable)
	void DestroyEnemyActor();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Health")
	float FullHealth;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Health")
	float currentHealth;
	

};
