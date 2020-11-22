// Copyright Epic Games, Inc. All Rights Reserved.

#include "NCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Manager/CharacterManager.h"
#include "Network/ClientNetwork.h"

//////////////////////////////////////////////////////////////////////////
// ANCharacter

ANCharacter::ANCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	m_eSpeedType = ESpeedType::Stop;
}

void ANCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CharacterManager::Get().IsMyCharacter(GetSessionID()) == false)
		CharacterManager::Get().AddCharacter(this);
}

void ANCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ANCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	bool bChange = false;
	FVector vCurLocation = GetActorLocation();
	FRotator rCurRotation = GetActorRotation();
	if (vCurLocation != m_vPreLocation)
	{
		m_vPreLocation = vCurLocation;
		m_eSpeedType = ESpeedType::Walk;
		bChange = true;
	}
	else
	{
		m_eSpeedType = ESpeedType::Stop;
	}

	if (rCurRotation != m_rPreRotation)
	{
		m_rPreRotation = rCurRotation;
		bChange = true;
	}

	if (CharacterManager::Get().IsMyCharacter(m_nSessionID) && 
		bChange == true)
	{
		ClientNetwork::Get().SendMoveInfo(m_nSessionID, vCurLocation, rCurRotation);
	}
}

float ANCharacter::GetSpeed() {
	switch (m_eSpeedType)
	{
	case ESpeedType::Stop: return 0.0f;
	case ESpeedType::Walk: return 370.0f;
	case ESpeedType::Run: return 370.0f;
	case ESpeedType::Sprint: return 370.0f;
	}
	return 0.0f;
}