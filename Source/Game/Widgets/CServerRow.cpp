#include "CServerRow.h"
#include "CMainMenu.h"
#include "Components/Button.h"

void UCServerRow::Setup(UCMainMenu* InParent, uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;

	RowButton->OnClicked.AddDynamic(this, &UCServerRow::OnClicked);
}

void UCServerRow::OnClicked()
{
	Parent->SetSelectedIndex(Index);
}
