#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Widgets/IMenuInterface.h"
#include "CGameInstance.generated.h"

UCLASS()
class GAME_API UCGameInstance : public UGameInstance, public IIMenuInterface
{
	GENERATED_BODY()

public:
	UCGameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Exec)
		void LoadMainMenu();

	UFUNCTION(BlueprintCallable, Exec)
		void LoadInGameMenu();

	UFUNCTION(Exec)
		void Host() override;

	UFUNCTION(Exec)
		void Join(const FString& InAddress) override;
	
	void LoadMainMenuLevel() override;

private:
	TSubclassOf<class UUserWidget> MainMenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;
	class UCMainMenu* MainMenu;
	class UCInGameMenu* InGameMenu;
};
