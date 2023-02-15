#pragma once

#include "CoreMinimal.h"
#include "CMenuWidget.h"
#include "CMainMenu.generated.h"

UCLASS()
class GAME_API UCMainMenu : public UCMenuWidget
{
	GENERATED_BODY()
	
private:
	UFUNCTION()		void HostServer();
	UFUNCTION()		void JoinServer();
	UFUNCTION()		void OpenJoinMenu();
	UFUNCTION()		void OpenMainMenu();
	UFUNCTION()		void QuitPressed();

private:
	UPROPERTY(meta = (BindWidget)) // 변수명과 동일한 실제 위젯을 찾아준다.
		class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* ConfirmJoinMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* CancelJoinMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
		class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
		class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuSwitcher;
	
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* IPAddressField;

public:
	virtual bool Initialize() override;
};
