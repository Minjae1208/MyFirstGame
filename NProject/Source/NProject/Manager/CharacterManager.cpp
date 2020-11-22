
#include "CharacterManager.h"
#include "Actor/NCharacter.h"

void CharacterManager::AddCharacter(ANCharacter* pCharacter)
{
	if (pCharacter == nullptr)
		return;

	if (m_arCharacter.Contains(pCharacter->GetSessionID()))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed : Character SessionID Constains"));
		return;
	}

	m_arCharacter.Add(pCharacter->GetSessionID(), pCharacter);
}

ANCharacter* CharacterManager::GetCharacter(int nSessionID)
{
	if (m_arCharacter.Contains(nSessionID))
	{
		return m_arCharacter.FindRef(nSessionID);
	}

	return nullptr;
}

bool CharacterManager::HasCharacter(int nSessionID)
{
	return m_arCharacter.Contains(nSessionID);
}
