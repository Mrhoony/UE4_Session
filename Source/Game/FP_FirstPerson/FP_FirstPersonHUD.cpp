#include "FP_FirstPersonHUD.h"
#include "Global.h"

#include "FP_FirstPersonCharacter.h"

#include "Engine/Canvas.h"
#include "TextureResource.h"

#include "CPlayerState.h"

#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"

AFP_FirstPersonHUD::AFP_FirstPersonHUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}

void AFP_FirstPersonHUD::DrawHUD()
{
	Super::DrawHUD();

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	const FVector2D CrosshairDrawPosition( (Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)),
										   (Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)) );

	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );

	AFP_FirstPersonCharacter* player = Cast<AFP_FirstPersonCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	CheckNull(player);
	
	ACPlayerState* state = player->GetSelfPlayerState();
	CheckNull(state);

	float health = state->Health;
	float kill = state->Score;
	int32 death = state->Death;

	FString str;
	str = "HP : " + FString::FromInt(health);
	DrawText(str, FLinearColor(1, 0, 1), 50, 50);

	str = "Kill : " + FString::FromInt(kill);
	DrawText(str, FLinearColor(1, 0, 1), 50, 70);

	str = "Death : " + FString::FromInt(death);
	DrawText(str, FLinearColor(1, 0, 1), 50, 90);

	str = "You Died, Noob";
	if (health <= 0)
		DrawText(str, FLinearColor::MakeRandomColor(), Center.X, Center.Y);
}
