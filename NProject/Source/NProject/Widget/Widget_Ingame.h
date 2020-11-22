// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Ingame.generated.h"

class UButton;
class UScrollBox;
class UEditableTextBox;

/**
 * 
 */
UCLASS()
class NPROJECT_API UWidget_Ingame : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ShowMessage;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* SendToMessage;

	UPROPERTY(meta = (BindWidget))
	UButton* btnChatEnter;

protected:
	UFUNCTION()
	void OnClickedEnter();

public:
	void AddChat(FText txtMsg);
};
