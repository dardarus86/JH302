// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "GroundPound.generated.h"

UCLASS()
class JH302_API AGroundPound : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGroundPound();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector scale;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	FVector GetSphereScale();
	UFUNCTION(BlueprintCallable)
	void SetSphereScale(float scaleAmount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* staticMeshComponent;
};
