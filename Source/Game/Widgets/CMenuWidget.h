#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IMenuInterface.h"
#include "CMenuWidget.generated.h"

UCLASS()
class GAME_API UCMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup();
	void Teardown();

	FORCEINLINE void SetMenuInterface(IIMenuInterface* InMenuInterface) { MenuInterface = InMenuInterface; }

protected:
	IIMenuInterface* MenuInterface;
};
