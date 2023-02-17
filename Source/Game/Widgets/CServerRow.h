#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CServerRow.generated.h"

UCLASS()
class GAME_API UCServerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(class UCMainMenu* InParent, uint32 InIndex);
	
private:
	UFUNCTION()
		void OnClicked();

public:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
		class UButton* RowButton;

private:
	class UCMainMenu* Parent;
	uint32 Index;
};
