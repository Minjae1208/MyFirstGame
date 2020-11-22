// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NPlayerController_Ingame.generated.h"

class UUserWidget;

UCLASS()
class NPROJECT_API ANPlayerController_Ingame : public APlayerController
{
	GENERATED_BODY()

public:
	ANPlayerController_Ingame();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupInputComponent() override;

public:
	void CreateUI();
	void InputType_UI();
	void InputType_Game();
	void InputType_GameAndUI();

public:
	void OnInput_Jump();
	void OnInput_StopJumping();
	void OnInput_MoveForward(float Value);
	void OnInput_MoveRight(float Value);
	void OnInput_Turn(float Rate);
	void OnInput_TurnAtRate(float Rate);
	void OnInput_LookUp(float Rate);
	void OnInput_LookUpAtRate(float Rate);

	void OnInput_Enter();

public:
	void AddChat(FString strMsg);

	void CreateCharacter(int nSessionID);

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY()
	UUserWidget* BP_MainUI;
};
