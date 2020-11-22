#pragma once

class ANCharacter;

class NCharacterInfo
{
public:
	NCharacterInfo(){
		Clear();
	}

	void Clear()
	{
		m_nSessionID = 0;
		m_strNickname.Empty();
	}

	int m_nSessionID;
	FString m_strNickname;
};

class CharacterManager
{
private:
	CharacterManager() {}

public:
	void AddCharacter(ANCharacter* pCharacter);
	ANCharacter* GetCharacter(int nSessionID);
	bool HasCharacter(int nSessionID);


public:
	NCharacterInfo& GetMyCharacterInfo() { return m_kMyCharacterInfo;  }
	int GetMySessionID() { return m_kMyCharacterInfo.m_nSessionID;  }

	bool IsMyCharacter(int nSessionID)	{
		return m_kMyCharacterInfo.m_nSessionID == nSessionID;
	}

	void SetMyNickname(FString strName) {
		m_kMyCharacterInfo.m_strNickname = strName;
	}
	void SetMySessionID(int nSessionID) {
		m_kMyCharacterInfo.m_nSessionID = nSessionID;
	}

protected:
	TMap<int, ANCharacter*> m_arCharacter;
	NCharacterInfo m_kMyCharacterInfo;

public:
	static CharacterManager& Get()
	{
		static CharacterManager This;
		return This;
	}
};