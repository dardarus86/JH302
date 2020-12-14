// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "JH302Character.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

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
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 1.0f;

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
	PlayerInputComponent->BindAction("Ability2", IE_Released, this, &AJH302Character::GroundPoundPlayerGravityIncrease);
	PlayerInputComponent->BindAction("Ability3", IE_Pressed, this, &AJH302Character::CanPlayerCharge);
	PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &AJH302Character::FireSpikes);
	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &AJH302Character::SetleftMouseClickBoolTrue);
	PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &AJH302Character::SetleftMouseClickBoolFalse);
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &AJH302Character::CanChangeGlobalTimeDilation);
	PlayerInputComponent->BindAction("RightClick", IE_Released, this, &AJH302Character::CanChangeGlobalTimeDilation);
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

#pragma region Tick functions
void AJH302Character::CheckIfRightMouseDownAndTimeDilate()
{
	if(b_isRightMouseDown)
	{
		timeScale = timeScale -0.20 * FApp::GetDeltaTime();
		if(timeScale<= 0.2)
		{
			timeScale = 0.2;
		}
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(),timeScale);
	}

	if(!b_isRightMouseDown)
	{
		timeScale = timeScale + 0.60* FApp::GetDeltaTime();
		if(timeScale >= 1.0)
		{
			timeScale = 1.0;
		}
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(),timeScale);
	}
}

void AJH302Character::CheckIfChargingAndSetWalkSpeed()
{
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
}

void AJH302Character::CheckLeftMouseDownAndReScaleCylinder()
{
	if(b_isLeftMouseDown)
	{
		b_isCastingFireSpike = true;
		if(cylinderScale >= 7.0f)
		{
			cylinderScale = 7.0f;
		}
		else
		{
			cylinderScale = cylinderScale + 1.2f* FApp::GetDeltaTime();
		}
		myCylinder->SetActorScale3D(FVector(cylinderScale,cylinderScale,5.0f));
	}
	else
	{
		cylinderScale = 0.2f;
	}
}

void AJH302Character::CheckIfCanGroundPoundAndDoIfTrue()
{
	if(GetCharacterMovement()->IsMovingOnGround())
	{
		GetCharacterMovement()->GravityScale = 1.0f;
		
	}
	else //if not on ground then you CAN ground pound
	{

		b_CanGroundPound = true;
	}

	if(b_CanGroundPound && b_isGroundPounding && GetCharacterMovement()->IsMovingOnGround())
	{
		GroundPound();
	}
}

void AJH302Character::UpdateCylinderPosition()
{
	if (b_Ability_1_Pressed)
	{
		FVector startPoint = FollowCamera->GetComponentLocation();
		FVector EndPoint = (FollowCamera->GetForwardVector() * targettable_Range) + startPoint;

		FCollisionQueryParams Parameters;
		GetWorld()->LineTraceSingleByChannel(outHit, startPoint, EndPoint, ECC_Visibility, Parameters);
		myCylinder->SetActorRelativeLocation(outHit.Location);

	}
}

void AJH302Character::UpdateCoolDowns()
{
	UpdateAbility1CoolDown();
	UpdateAbility2CoolDown();
}

#pragma endregion 

void AJH302Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCoolDowns();

	UpdateCylinderPosition();

	CheckIfRightMouseDownAndTimeDilate();

	CheckIfChargingAndSetWalkSpeed();

	CheckLeftMouseDownAndReScaleCylinder();

	CheckIfCanGroundPoundAndDoIfTrue();
}

#pragma region Player made functions
FVector AJH302Character::GetOutHitLineTrace()
{
	FVector startPoint = FollowCamera->GetComponentLocation();
	FVector EndPoint = (FollowCamera->GetForwardVector() * targettable_Range) + startPoint;

	FCollisionQueryParams Parameters;
	GetWorld()->LineTraceSingleByChannel(outHit, startPoint, EndPoint, ECC_Visibility, Parameters);

	return outHit.Location;
}

//Ability 1 initial function
void AJH302Character::F_SpawnCylinder()
{
	if(b_CanCastAbility1)
	{
		b_Ability_1_Pressed = true;

		FRotator spawnRotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnParameters;
		myCylinder = static_cast<AMyCylinder*>(GetWorld()->SpawnActor<AMyCylinder>(GetOutHitLineTrace(), spawnRotation, SpawnParameters));
	}

}
//gravity set for ability 2
void AJH302Character::GroundPoundPlayerGravityIncrease()
{
	if(b_CanCastAbility2)
	{
	b_isGroundPounding = true;

	FVector startPoint = GetActorLocation();
	FVector EndPoint = (GetActorLocation().DownVector * targettable_Range) + startPoint;

	FCollisionQueryParams Parameters;
	GetWorld()->LineTraceSingleByChannel(outHit, startPoint, EndPoint, ECC_Visibility, Parameters);

	playerDistanceFromGround = GetActorLocation().Z - outHit.Location.Z;
	
	GetCharacterMovement()->GravityScale = 1000.0f;
	}
}
//used in GroundPound()
void AJH302Character::F_SpawnSphere()
{
	FVector offset = FVector(0, 0, 300);
	FRotator spawnRotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnParameters;

	groundPoundSphere = static_cast<AGroundPound*>(GetWorld()->SpawnActor<AGroundPound>(GetActorLocation()-offset, spawnRotation, SpawnParameters));

}
//Ability 1 main functionality
void AJH302Character::FireSpikes()
{
	if(b_CanCastAbility1)
	{
			
		
		FVector Velocity(FMath::RandRange(-30.0f * cylinderScale,30.0f* cylinderScale),FMath::RandRange(-30.0f * cylinderScale,30.0f* cylinderScale),FMath::RandRange(80.0f * cylinderScale,500.0f* cylinderScale));
		b_Ability_1_Pressed = false;
	
		myCylinder->GetOverlappingActors(enemiesInOverlapEvent);
		if(b_isCastingFireSpike && !b_Ability_1_Pressed)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleCylinder, GetOutHitLineTrace() -FVector(0,0,+20), FRotator::ZeroRotator, true);
		}
		
		for(auto &x:enemiesInOverlapEvent)
		{
		
			UPrimitiveComponent* PC = Cast<UPrimitiveComponent>(x->GetRootComponent());
	
			if(PC)
			{
					PC->SetSimulatePhysics(true);
					PC->CreatePhysicsState();
					PC->SetCollisionProfileName("PhysicsActor");
					PC->SetSimulatePhysics(true);
					PC->SetGenerateOverlapEvents(true);
					PC->AddForce(Velocity);
					PC->AddImpulseAtLocation(Velocity,PC->GetComponentLocation(),NAME_All);
					PC->AddImpulse(Velocity*PC->GetMass());
					PC->SetPhysicsLinearVelocity(Velocity,false,NAME_None);
			}
			AEnemies* enemy = Cast<AEnemies>(x);
			enemy->EnemyTakeDamage(15*cylinderScale);
			
	
		}
		myCylinder->Destroy();
		b_isCastingFireSpike = false;
		ability1CoolDown = ability1MaxCoolDown;
	}
}
//Ability 2 main functionality
void AJH302Character::GroundPound()
{
	if(b_CanCastAbility2)
	{
		F_SpawnSphere();
		groundPoundSphere->GetOverlappingActors(enemiesInGroundPoundOverlapEvent);
		if(b_isGroundPounding)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleGroundPound, GetActorLocation() - FVector(0,0,+80), FRotator::ZeroRotator, true);
		}
	
	
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
				SM->AddRadialForce(this->GetActorLocation(),80000,100000,ERadialImpulseFalloff::RIF_Linear,true);
			}
			AEnemies* enemy = Cast<AEnemies>(x);
			enemy->EnemyTakeDamage(FMath::Clamp(playerDistanceFromGround/10.0f,1.0f,100.0f));
			
		}
		groundPoundSphere->Destroy();
		b_CanGroundPound = false;
		b_isGroundPounding = false;
		ability2CoolDown = ability2MaxCoolDown;
	}
}
//Ability 3 main functionality
void AJH302Character::PlayerCharge()
{
	FVector Velocity(FMath::RandRange(GetCharacterMovement()->GetCurrentAcceleration().X/14,GetCharacterMovement()->GetCurrentAcceleration().X/10),FMath::RandRange( GetCharacterMovement()->GetCurrentAcceleration().Y/14,GetCharacterMovement()->GetCurrentAcceleration().Y/10),FMath::RandRange(300.0f,500.0f));
	chargeCollisionMesh->GetOverlappingActors(enemiesInChargeOverlapEvent);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleCharge, GetActorLocation() - FVector(100,0,100), FRotator::ZeroRotator, true);

	if(enemiesInChargeOverlapEvent.Num() >= 2)
	{
		for(auto &x:enemiesInChargeOverlapEvent)
		{
			UPrimitiveComponent* SM = Cast<UPrimitiveComponent>(x->GetRootComponent());
			if(SM)
			{
				SM->SetSimulatePhysics(true);
				SM->CreatePhysicsState();
				SM->SetCollisionProfileName("Enemies");
				SM->SetSimulatePhysics(true);
				SM->SetGenerateOverlapEvents(true);
				SM->AddForce(Velocity);
				SM->AddImpulseAtLocation(Velocity,SM->GetComponentLocation(),NAME_All);
				SM->AddImpulse(Velocity*SM->GetMass());
				SM->SetPhysicsLinearVelocity(Velocity,false,NAME_None);
			}
			AEnemies* enemy = Cast<AEnemies>(x);
			enemy->EnemyTakeDamage(10);
		}
	}
}
//Bool for ability 1 + left mouse click
void AJH302Character::SetleftMouseClickBoolTrue()
{
	if(!b_isLeftMouseDown)
	{
		b_isLeftMouseDown = true;
	}
}
//Bool for ability 1 + left mouse click
void AJH302Character::SetleftMouseClickBoolFalse()
{
	if(b_isLeftMouseDown)
	{
		b_isLeftMouseDown = false;
		myCylinder->SetActorScale3D(FVector(0.2,0.2,5.0f));
	}
}
//changing the bool from true/false for ability 3
void AJH302Character::CanPlayerCharge()
{
	b_isCharging = !b_isCharging;
}
//changing the bool from true/false for time dilation
void AJH302Character::CanChangeGlobalTimeDilation()
{
	b_isRightMouseDown = !b_isRightMouseDown;
}

void AJH302Character::UpdateAbility1CoolDown()
{
	if(ability1CoolDown >= 0)
	{
		b_CanCastAbility1 = false;
		ability1CoolDown -= 2.0* FApp::GetDeltaTime();
	}
	else
	{
		b_CanCastAbility1 = true;
	}
}

void AJH302Character::UpdateAbility2CoolDown()
{
	if(ability2CoolDown >= 0)
	{
		b_CanCastAbility2 = false;
		ability2CoolDown -= 2.0* FApp::GetDeltaTime();
	}
	else
	{
		b_CanCastAbility2 = true;
	}
}



#pragma endregion 
#pragma region Unreal Base functions
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
