// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/StaticMeshComponent.h>
#include "Materials/Material.h"
#include "MyCylinder.generated.h"

UCLASS()
class JH302_API AMyCylinder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCylinder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector scale;

public:	
	// Called every frameMy
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* staticMeshComponent;
};
