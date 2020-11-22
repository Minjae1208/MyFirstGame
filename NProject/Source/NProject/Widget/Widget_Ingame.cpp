// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_Ingame.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Kismet/KismetSystemLibrary.h"

#include "PlayerController/NPlayerController_Ingame.h"

#include "Network/ClientNetwork.h"
#include "Manager/CharacterManager.h"


void UWidget_Ingame::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (btnChatEnter != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Create btnChatEnter!"), );
		btnChatEnter->OnClicked.AddDynamic(this, &UWidget_Ingame::OnClickedEnter);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UnCreate btnChatEnter!"), );
	}
		
}

void UWidget_Ingame::OnClickedEnter()
{
	ANPlayerController_Ingame* pPlayerController = GetOwningPlayer<ANPlayerController_Ingame>();
	if (ShowMessage == nullptr &&
		SendToMessage == nullptr)
		return;

	FText strMssage = SendToMessage->GetText();
	if (strMssage.IsEmpty() == true)
		return;

	SendToMessage->SetText(FText::FromString(TEXT("")));

#if 1
	ClientNetwork::Get().SendChat(CharacterManager::Get().GetMySessionID(), *strMssage.ToString());
#else
	AddChat(strMssage);
#endif
}

void UWidget_Ingame::AddChat(FText txtMsg)
{
	if (ShowMessage == nullptr &&
		SendToMessage == nullptr)
		return;

	if (txtMsg.IsEmpty() == true)
		return;

	UTextBlock* pMessage = NewObject<UTextBlock>(this);
	if (pMessage == nullptr)
		return;
	
	pMessage->SetText(txtMsg);

	ShowMessage->AddChild(pMessage);
}