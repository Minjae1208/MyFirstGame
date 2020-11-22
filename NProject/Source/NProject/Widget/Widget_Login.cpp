// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_Login.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/KismetSystemLibrary.h"

#include "PlayerController/NPlayerController_Login.h"

void UWidget_Login::NativeConstruct()
{
	Super::NativeConstruct();

	if (btnLogin != nullptr)
		btnLogin->OnClicked.AddDynamic(this, &UWidget_Login::OnClickedLogin);
	if (btnQuit != nullptr)
		btnQuit->OnClicked.AddDynamic(this, &UWidget_Login::OnClickedGameQuit);
}

void UWidget_Login::OnClickedLogin()
{
	if (txtID == nullptr ||
		txtPW == nullptr)
		return;

	if (ANPlayerController_Login* pPlayerController = Cast< ANPlayerController_Login>(GetOwningPlayer()))
	{
		pPlayerController->SendLogin(txtID->GetText(), txtPW->GetText());
	}
}

void UWidget_Login::OnClickedGameQuit()

{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
