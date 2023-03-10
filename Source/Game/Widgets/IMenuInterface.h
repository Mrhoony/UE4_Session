#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IMenuInterface.generated.h"

UINTERFACE(MinimalAPI)
class UIMenuInterface : public UInterface
{
	GENERATED_BODY()
};

class GAME_API IIMenuInterface
{
	GENERATED_BODY()

public:
	virtual void Host(FString& InServerName) = 0;
	virtual void Join(const uint32& Index) = 0;
	virtual void LoadMainMenuLevel() = 0;
	virtual void RefreshServerList() = 0;
};
