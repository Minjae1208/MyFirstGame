// Fill out your copyright notice in the Description page of Project Settings.


#include "NPlayerController_Ingame.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Components/Button.h"

#include "Widget/Widget_Ingame.h"
#include "Actor/NCharacter.h"

#include "Network/ClientNetwork.h"
#include "Manager/CharacterManager.h"

// Sets default values
ANPlayerController_Ingame::ANPlayerController_Ingame()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
}

void ANPlayerController_Ingame::BeginPlay()
{
	Super::BeginPlay();
	
	CreateUI();

	//InputType_UI();

	InputType_Game();

	//InputType_GameAndUI();

	ANCharacter* pCharacter = Cast<ANCharacter>(GetCharacter());
	if (IsValid(pCharacter) == false)
		return;

	pCharacter->SetSessionID(CharacterManager::Get().GetMySessionID());

	ClientNetwork::Get().SendEnterZone(pCharacter->GetSessionID(), pCharacter->GetActorLocation(), pCharacter->GetActorRotation());
}

void ANPlayerController_Ingame::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ANCharacter* pCharacter = Cast<ANCharacter>(GetCharacter());
	if (IsValid(pCharacter) == false)
		return;

	//ClientNetwork::Get().OffAllServer();
}

// Called every frame
void ANPlayerController_Ingame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANPlayerController_Ingame::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr)
		return;

	InputComponent->BindAction("Jump", IE_Pressed, this, &ANPlayerController_Ingame::OnInput_Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ANPlayerController_Ingame::OnInput_StopJumping);

	InputComponent->BindAxis("MoveForward", this, &ANPlayerController_Ingame::OnInput_MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ANPlayerController_Ingame::OnInput_MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &ANPlayerController_Ingame::OnInput_Turn);
	InputComponent->BindAxis("TurnRate", this, &ANPlayerController_Ingame::OnInput_TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &ANPlayerController_Ingame::OnInput_LookUp);
	InputComponent->BindAxis("LookUpRate", this, &ANPlayerController_Ingame::OnInput_LookUpAtRate);

	InputComponent->BindAction("Chat", IE_Pressed, this, &ANPlayerController_Ingame::OnInput_Enter);

}

void ANPlayerController_Ingame::CreateUI()
{
	UWorld* pWorld = GetWorld();
	if (pWorld == nullptr)
		return;

	FSoftClassPath refClass(TEXT("/Game/UI/BP_Ingame.BP_Ingame_C"));
	UClass* pClass = refClass.ResolveClass();
	if (pClass == nullptr)
		pClass = refClass.TryLoadClass<UUserWidget>();

	BP_MainUI = Cast<UUserWidget>(CreateWidget<UUserWidget>(pWorld, pClass));
	if (BP_MainUI == nullptr)
		return;

	// 화면에 붙이기
	BP_MainUI->AddToViewport(0);
}

void ANPlayerController_Ingame::InputType_UI()
{
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, BP_MainUI, EMouseLockMode::DoNotLock);
	// 마우스 커서 보이기
	bShowMouseCursor = true;
}

void ANPlayerController_Ingame::InputType_Game()
{
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
	// 마우스 커서 안보이기
	bShowMouseCursor = true;
}

void ANPlayerController_Ingame::InputType_GameAndUI()
{
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, BP_MainUI, EMouseLockMode::DoNotLock);
	// 마우스 커서 보이기
	bShowMouseCursor = true;
}

void ANPlayerController_Ingame::OnInput_Jump()
{
	ACharacter* pCharacter = GetCharacter();
	if (IsValid(pCharacter) == false)
		return;

	pCharacter->Jump();
}

void ANPlayerController_Ingame::OnInput_StopJumping()
{
	ACharacter* pCharacter = GetCharacter();
	if (IsValid(pCharacter) == false)
		return;

	pCharacter->StopJumping();
}

void ANPlayerController_Ingame::OnInput_MoveForward(float Value)
{
	ACharacter* pCharacter = GetCharacter();
	if (IsValid(pCharacter) == false)
		return;

	if (Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		pCharacter->AddMovementInput(Direction, Value);
	}
}

void ANPlayerController_Ingame::OnInput_MoveRight(float Value)
{
	ACharacter* pCharacter = GetCharacter();
	if (IsValid(pCharacter) == false)
		return;

	if (Value != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		pCharacter->AddMovementInput(Direction, Value);
	}
}

void ANPlayerController_Ingame::OnInput_Turn(float Rate)
{
	ACharacter* pCharacter = GetCharacter();
	if (IsValid(pCharacter) == false)
		return;

	pCharacter->AddControllerYawInput(Rate);
}

void ANPlayerController_Ingame::OnInput_TurnAtRate(float Rate)
{
	ACharacter* pCharacter = GetCharacter();
	if (IsValid(pCharacter) == false)
		return;

	// calculate delta for this frame from the rate information
	pCharacter->AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANPlayerController_Ingame::OnInput_LookUp(float Rate)
{
	ACharacter* pCharacter = GetCharacter();
	if (IsValid(pCharacter) == false)
		return;

	pCharacter->AddControllerPitchInput(Rate);
}

void ANPlayerController_Ingame::OnInput_LookUpAtRate(float Rate)
{
	ACharacter* pCharacter = GetCharacter();
	if (IsValid(pCharacter) == false)
		return;

	// calculate delta for this frame from the rate information
	pCharacter->AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ANPlayerController_Ingame::OnInput_Enter()
{
	if (BP_MainUI == nullptr)
		return;

	UWidget_Ingame* pWidgetIngame = Cast<UWidget_Ingame>(BP_MainUI);
	if (pWidgetIngame == nullptr)
		return;

}

void ANPlayerController_Ingame::AddChat(FString strMsg)
{
	UWidget_Ingame* pWidgetIngame = Cast<UWidget_Ingame>(BP_MainUI);
	if (pWidgetIngame == nullptr)
		return;

	pWidgetIngame->AddChat(FText::FromString(strMsg));
}

void ANPlayerController_Ingame::CreateCharacter(int nSessionID)
{
	UWorld* pWorld = GetWorld();
	if (pWorld == nullptr)
		return;

	FVector vLocation(-700.0f, 370.0f, 230.0f);
	
	FSoftClassPath refClass(TEXT("/Game/Chracter/BP_NCharacter.BP_NCharacter_C"));
	UClass* pClass = refClass.ResolveClass();
	if (pClass == nullptr)
		pClass = refClass.TryLoadClass<ANCharacter>();

	if (pClass == nullptr)
		return;

	ANCharacter* pCharacter = 
		pWorld->SpawnActor<ANCharacter>(
			pClass, vLocation, FRotator::ZeroRotator);
	if (pCharacter == nullptr)
		return;

	pCharacter->SetSessionID(nSessionID);

	CharacterManager::Get().AddCharacter(pCharacter);
}