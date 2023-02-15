#include "CInGameMenu.h"
#include "Components/Button.h"

bool UCInGameMenu::Initialize()
{
	bool success = Super::Initialize();
	if (success == false) return false;

	if (CancelButton == nullptr) return false;
	CancelButton->OnClicked.AddDynamic(this, &UCInGameMenu::CancelPressed);

	if (QuitButton == nullptr) return false;
	QuitButton->OnClicked.AddDynamic(this, &UCInGameMenu::QuitPressed);

	return true;
}

void UCInGameMenu::CancelPressed()
{
	Teardown();
}

void UCInGameMenu::QuitPressed()
{
	if (MenuInterface != nullptr)
	{
		Teardown();
		MenuInterface->LoadMainMenuLevel();
	}
}
