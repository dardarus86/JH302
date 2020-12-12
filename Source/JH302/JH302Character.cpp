// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "JH302Character.h"

#include <array>

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GeometryCollection/GeometryCollectionSimulationTypes.h"

//////////////////////////////////////////////////////////////////////////
// AJH302Character

AJH302Character::AJH302Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 2000.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	chargeCollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("collision charge"));
	chargeCollisionMesh->SetupAttachment(RootComponent);
	chargeCollisionMesh->SetCollisionProfileName("Charge");
	UStaticMesh* staticMeshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	//M_Seethrough.M_Seethrough
	//UMaterial* materialAsset = LoadObject<UMaterial>(nullptr,TEXT("StaticMesh'/Game/StarterContent/Materials/M_Seethrough.M_Seethrough'"));
	UMaterial* materialAsset = LoadObject<UMaterial>(nullptr,TEXT("StaticMesh'/Game/StarterContent/Materials/M_invisible_cube.M_invisible_cube'"));
	chargeCollisionMesh->SetStaticMesh(staticMeshAsset);
	chargeCollisionMesh->SetMaterial(0,materialAsset);
	chargeCollisionMesh->SetRelativeLocation(FVector(50,0,-100.0f));
	chargeCollisionMesh->SetWorldScale3D(FVector(2.0,1.5,1.75));
	chargeCollisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	chargeCollisionMesh->SetActive(false);
	chargeCollisionMesh->SetGenerateOverlapEvents(true);
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AJH302Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Ability1", IE_Released, this, &AJH302Character::F_SpawnCylinder);
	PlayerInputComponent->BindAction("Ability2", IE_Released, this, &AJH302Character::SpawnCylinderAtSetLocation);
	PlayerInputComponent->BindAction("Ability3", IE_Pressed, this, &AJH302Character::CanPlayerCharge);
	PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &AJH302Character::LeftClickSetCylinderBoolFalse);
	PlayerInputComponent->BindAction("PlusGravity", IE_Pressed, this, &AJH302Character::IncreasePlayerGravity);
	PlayerInputComponent->BindAction("MinusGravity", IE_Pressed, this, &AJH302Character::DecreasePlayerGravity);
	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &AJH302Character::SetleftMouseClickBoolTrue);
	PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &AJH302Character::SetleftMouseClickBoolFalse);
	PlayerInputComponent->BindAction("MovementIncrease", IE_Pressed, this, &AJH302Character::ChangeMovementPlus);
	PlayerInputComponent->BindAction("Movementdecrease", IE_Pressed, this, &AJH302Character::ChangeMovementMinus);
	PlayerInputComponent->BindAxis("MoveForward", this, &AJH302Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AJH302Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AJH302Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AJH302Character::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AJH302Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AJH302Character::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AJH302Character::OnResetVR);
}

void AJH302Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (b_Ability_1_Pressed)
	{
		FVector startPoint = FollowCamera->GetComponentLocation();
		FVector EndPoint = (FollowCamera->GetForwardVector() * m_targettable_Range) + startPoint;

		FCollisionQueryParams Parameters;
		GetWorld()->LineTraceSingleByChannel(outHit, startPoint, EndPoint, ECC_Visibility, Parameters);
		mycylinder->SetActorRelativeLocation(outHit.Location);

	}

	if(b_isCharging)
	{
		chargeCollisionMesh->SetActive(true);
		GetCharacterMovement()->MaxWalkSpeed = 2000.0f;
		PlayerCharge();
		
	}
	else
	{
		chargeCollisionMesh->SetActive(false);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}

	if(b_isLeftMouseDown)
	{
		if(CylinderScale >= 7.0f)
		{
			CylinderScale = 7.0f;
		}
		else
		{
			CylinderScale = CylinderScale + 0.04f;
		}
		mycylinder->SetActorScale3D(FVector(CylinderScale,CylinderScale,5.0f));
	}
	else
	{
		CylinderScale = 0.2f;
	}

	//if character is on the ground, you CANT ground pound
	if(GetCharacterMovement()->IsMovingOnGround())
	{
		GetCharacterMovement()->GravityScale = 1.0f;
		
	}
	else //if not on ground then you CAN ground pound
	{

			canGroundPound = true;
	}

	if(canGroundPound && GetCharacterMovement()->IsMovingOnGround())
	{
		GroundPound();
	}
}

#pragma region Player made functions
FVector AJH302Character::GetoutHitLineTrace()
{
	FVector startPoint = FollowCamera->GetComponentLocation();
	FVector EndPoint = (FollowCamera->GetForwardVector() * m_targettable_Range) + startPoint;

	FCollisionQueryParams Parameters;
	GetWorld()->LineTraceSingleByChannel(outHit, startPoint, EndPoint, ECC_Visibility, Parameters);

	return outHit.Location;
}

void AJH302Character::F_SpawnCylinder()
{
	b_Ability_1_Pressed = true;

	FRotator spawnRotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnParameters;
	mycylinder = static_cast<AMyCylinder*>(GetWorld()->SpawnActor<AMyCylinder>(GetoutHitLineTrace(), spawnRotation, SpawnParameters));
}

void AJH302Character::F_SpawnSphere()
{
	FVector DefaultLocation = FVector(-50.0, 850.0, 350.0);
	FVector offset = FVector(0, 0, 300);
	FRotator spawnRotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnParameters;

	groundPoundSphere = static_cast<AGroundPound*>(GetWorld()->SpawnActor<AGroundPound>(GetActorLocation()-offset, spawnRotation, SpawnParameters));

}

void AJH302Character::SpawnCylinderAtSetLocation()
{
	FVector DefaultLocation = FVector(-10.0, 850.0, 350.0);
	FRotator spawnRotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnParameters;

	GetWorld()->SpawnActor<AMyCylinder>(subClasscylinder, DefaultLocation, spawnRotation, SpawnParameters);
}


void AJH302Character::LeftClickSetCylinderBoolFalse()
{
	FVector Velocity(FMath::RandRange(-10.0f * CylinderScale,10.0f* CylinderScale),FMath::RandRange(-10.0f * CylinderScale,10.0f* CylinderScale),FMath::RandRange(100.0f * CylinderScale,1000.0f* CylinderScale));
	b_Ability_1_Pressed = false;
int testNumber = 0;
	mycylinder->GetOverlappingActors(enemiesInOverlapEvent);
	
	for(auto &x:enemiesInOverlapEvent)
	{
	
		UPrimitiveComponent* SM = Cast<UPrimitiveComponent>(x->GetRootComponent());
		//UStaticMeshComponent* SM = Cast<UStaticMeshComponent>(x->GetRootComponent());
		if(SM)
		{
				SM->SetSimulatePhysics(true);
				SM->CreatePhysicsState();
				SM->SetCollisionProfileName("PhysicsActor");
				SM->SetSimulatePhysics(true);
				SM->SetGenerateOverlapEvents(true);
				SM->AddForce(Velocity);
				SM->AddImpulseAtLocation(Velocity,SM->GetComponentLocation(),NAME_All);
				SM->AddImpulse(Velocity*SM->GetMass());
				SM->SetPhysicsLinearVelocity(Velocity,false,NAME_None);
		}
		AEnemies* enemy = Cast<AEnemies>(x);
		enemy->EnemyTakeDamage(15*CylinderScale);

	}
	mycylinder->Destroy();
}

void AJH302Character::IncreasePlayerGravity()
{
	GetCharacterMovement()->GravityScale = 1000.0f;
}

void AJH302Character::DecreasePlayerGravity()
{
	GetCharacterMovement()->GravityScale = 200.0f;
}

void AJH302Character::GroundPound()
{
	F_SpawnSphere();
	groundPoundSphere->GetOverlappingActors(enemiesInGroundPoundOverlapEvent);
	for(auto &x:enemiesInGroundPoundOverlapEvent)
	{
		UPrimitiveComponent* SM = Cast<UPrimitiveComponent>(x->GetRootComponent());
		
		if(SM)
		{
			SM->SetSimulatePhysics(true);
			SM->CreatePhysicsState();
			SM->SetCollisionProfileName("PhysicsActor");
			SM->SetSimulatePhysics(true);
			SM->SetGenerateOverlapEvents(true);
			SM->AddRadialForce(this->GetActorLocation(),80000,300000,ERadialImpulseFalloff::RIF_Linear,true);
			}
		AEnemies* enemy = Cast<AEnemies>(x);
		enemy->EnemyTakeDamage(70);
	}
	groundPoundSphere->Destroy();
	canGroundPound = false;
}

void AJH302Character::SetleftMouseClickBoolTrue()
{
	if(!b_isLeftMouseDown)
	{
		b_isLeftMouseDown = true;
	}
}

void AJH302Character::SetleftMouseClickBoolFalse()
{
	if(b_isLeftMouseDown)
	{
		b_isLeftMouseDown = false;
		mycylinder->SetActorScale3D(FVector(0.2,0.2,5.0f));
	}
}

void AJH302Character::ChangeMovementPlus()
{
	GetCharacterMovement()->MaxWalkSpeed = 40000.0f;
}

void AJH302Character::ChangeMovementMinus()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void AJH302Character::PlayerCharge()
{
		FVector Velocity(FMath::RandRange(GetCharacterMovement()->GetCurrentAcceleration().X/8,GetCharacterMovement()->GetCurrentAcceleration().X/2),FMath::RandRange( GetCharacterMovement()->GetCurrentAcceleration().Y/8,GetCharacterMovement()->GetCurrentAcceleration().Y/2),FMath::RandRange(100.0f,1000.0f));
		chargeCollisionMesh->GetOverlappingActors(enemiesInChargeOverlap);

	if(enemiesInChargeOverlap.Num() >= 2)
	{
		for(auto &x:enemiesInChargeOverlap)
		{
			UPrimitiveComponent* SM = Cast<UPrimitiveComponent>(x->GetRootComponent());
			if(SM)
			{
				SM->SetSimulatePhysics(true);
				SM->CreatePhysicsState();
				SM->SetCollisionProfileName("PhysicsActor");
				SM->SetSimulatePhysics(true);
				SM->SetGenerateOverlapEvents(true);
				SM->AddForce(Velocity);
				SM->AddImpulseAtLocation(Velocity,SM->GetComponentLocation(),NAME_All);
				SM->AddImpulse(Velocity*SM->GetMass());
				SM->SetPhysicsLinearVelocity(Velocity,false,NAME_None);
			}
			AEnemies* enemy = Cast<AEnemies>(x);
			//enemy->EnemyTakeDamage(15);
		}
	}
}

void AJH302Character::CanPlayerCharge()
{
	b_isCharging = !b_isCharging;
}

#pragma endregion 
#pragma region Base functions
void AJH302Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}


void AJH302Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AJH302Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AJH302Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AJH302Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AJH302Character::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AJH302Character::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
#pragma endregion 
