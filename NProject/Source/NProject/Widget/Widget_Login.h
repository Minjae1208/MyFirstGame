// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Login.generated.h"

class UButton;
class UEditableTextBox;

/**
 * 
 */
UCLASS()
class NPROJECT_API UWidget_Login : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	UButton* btnLogin;
	UPROPERTY(meta = (BindWidget))
	UButton* btnQuit;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* txtID;
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* txtPW;

protected:
	UFUNCTION()
	void OnClickedLogin();
	UFUNCTION()
	void OnClickedGameQuit();
	
};
