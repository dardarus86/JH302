// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundPound.h"



// Sets default values
AGroundPound::AGroundPound()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	staticMeshComponent->SetupAttachment(GetRootComponent());
	staticMeshComponent->SetCollisionProfileName("Cylinder");
	scale = FVector(20.0f,20.0f,4.0f);
	staticMeshComponent->SetWorldScale3D(scale);
	

	UStaticMesh* staticMeshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	UMaterial* materialAsset = LoadObject<UMaterial>(nullptr,TEXT("StaticMesh'/Game/StarterContent/Materials/M_Seethrough_sphere.M_Seethrough_sphere'"));
	staticMeshComponent->SetStaticMesh(staticMeshAsset);
	staticMeshComponent->SetMaterial(0,materialAsset);
	
}

// Called when the game starts or when spawned
void AGroundPound::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGroundPound::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

