// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCylinder.h"



// Sets default values
AMyCylinder::AMyCylinder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	staticMeshComponent->SetupAttachment(GetRootComponent());
	staticMeshComponent->SetCollisionProfileName("Cylinder");
	scale = FVector(0.2,0.2,5.0);
	staticMeshComponent->SetWorldScale3D(scale);
	

	UStaticMesh* staticMeshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder'"));
	UMaterial* materialAsset = LoadObject<UMaterial>(nullptr,TEXT("StaticMesh'/Game/materials/cylinder.cylinder'"));
	staticMeshComponent->SetStaticMesh(staticMeshAsset);
	staticMeshComponent->SetMaterial(0,materialAsset);

}

// Called when the game starts or when spawned
void AMyCylinder::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyCylinder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AMyCylinder::GetCylinderScale()
{
	return scale;
}
void AMyCylinder::SetCylinderScale(FVector scaleAmount)
{
	scale = FVector(scaleAmount.X,scaleAmount.Y,scaleAmount.Z);
}

