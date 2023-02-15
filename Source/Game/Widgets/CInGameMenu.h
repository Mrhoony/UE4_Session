#pragma once

#include "CoreMinimal.h"
#include "CMenuWidget.h"
#include "CInGameMenu.generated.h"

UCLASS()
class GAME_API UCInGameMenu : public UCMenuWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;
	
private:
	UFUNCTION()
		void CancelPressed();

	UFUNCTION()
		void QuitPressed();

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* CancelButton;
};
