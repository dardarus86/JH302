// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies.h"


#include "Engine/EngineTypes.h"
#include "Serialization/StructuredArchiveFormatter.h"

// Sets default values
AEnemies::AEnemies()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	skeletalMesh->SetupAttachment(RootComponent);
	skeletalMesh->SetCollisionProfileName("PhysicsActor");
	skeletalMesh->SetSimulatePhysics(true);
	skeletalMesh->SetGenerateOverlapEvents(true);
	skeletalMesh->SetMobility(EComponentMobility::Movable);


	USkeletalMesh* skeletalMeshAsset = LoadObject<USkeletalMesh>(nullptr, TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	UMaterial* skeletalmaterialAsset = LoadObject<UMaterial>(nullptr,TEXT("SkeletalMaterial'/Game/Mannequin/Character/Materials/M_UE4Man_Body.M_UE4Man_Body'"));
	UMaterial* skeletalmaterialAsset2 = LoadObject<UMaterial>(nullptr,TEXT("SkeletalMaterial2'/Game/Mannequin/Character/Materials/M_UE4Man_ChestLogo.M_UE4Man_ChestLogo'"));

	skeletalMesh->SetSkeletalMesh(skeletalMeshAsset);
	skeletalMesh->SetMaterial(0,skeletalmaterialAsset);
	skeletalMesh->SetMaterial(1,skeletalmaterialAsset2);

}

// Called when the game starts or when spawned
void AEnemies::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemies::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

