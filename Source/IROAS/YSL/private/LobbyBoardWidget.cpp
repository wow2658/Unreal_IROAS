// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyBoardWidget.h"
#include <UMG/Public/Components/WidgetSwitcher.h>
#include <UMG/Public/Components/Button.h>
#include <UMG/Public/Components/TextBlock.h>
#include <UMG/Public/Components/ScrollBox.h>
#include <UMG/Public/Components/VerticalBox.h>
#include <Kismet/GameplayStatics.h>
#include "IROASGameInstance.h"
#include "MapInfoWidget.h"
#include "LobbyGameMode.h"
#include "BattlePlayer.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>


void ULobbyBoardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	gi = GetGameInstance<UIROASGameInstance>();
	gm = GetWorld()->GetAuthGameMode<ALobbyGameMode>();

	btn_StoryMode->OnPressed.AddDynamic(this, &ULobbyBoardWidget::OnClickStoryMode);
	btn_CustomMode->OnPressed.AddDynamic(this, &ULobbyBoardWidget::OnClickCustomMode);
	btn_EditorMode->OnPressed.AddDynamic(this, &ULobbyBoardWidget::OnClickEditorMode);
	//btn_MovetoEditorMap->OnPressed.AddDynamic(this, &OnClickMovetoEditorMap);
	btn_MovetoLobby->OnPressed.AddDynamic(this, &ULobbyBoardWidget::OnClickMovetoLobby);
	btn_MovetoBattleWaitingRoom->OnPressed.AddDynamic(this, &ULobbyBoardWidget::OnClickBattleWaitingRoom);
	//btn_MovetoCustomMap->OnPressed.AddDynamic(this, &OnClickCustomMap);
	btn_Back_0->OnPressed.AddDynamic(this, &ULobbyBoardWidget::OnClickBack);
	btn_Back_1->OnPressed.AddDynamic(this, &ULobbyBoardWidget::OnClickBack);
	btn_Back_2->OnPressed.AddDynamic(this, &ULobbyBoardWidget::OnClickBack);
}

void ULobbyBoardWidget::SwitchCanvas(int32 index)
{
	ws_LobbyGameMenuSwitch->SetActiveWidgetIndex(index);
}


void ULobbyBoardWidget::OnClickStoryMode()
{
	// 배틀 레벨로 넘어가기 (건탁)
	UGameplayStatics::OpenLevel(GetWorld(), FName("Proto_StoryLevel"));

}

void ULobbyBoardWidget::OnClickCustomMode()
{
	SwitchCanvas(1);
	// gi에서 mapList 조회 --> gameMode에서 함
	if (gm != nullptr && gm->reqActor != nullptr) {
		//gi->mapList.Add(gi->customMapInfo);
		if (gm->GetMapListFromDB()) {
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
				{
					mapList = gm->mapList;

					for (int i = 0; i < mapList.Num(); i++) {
						mapInfo = CreateWidget<UMapInfoWidget>(GetWorld(), mapInfoWidget);
						if (mapInfo != nullptr) {
							mapInfo->txt_MapName->SetText(FText::FromString(mapList[i].mapName));
							mapInfo->txt_MapCreatorName->SetText(FText::FromString(mapList[i].mapCreator));
							mapInfo->currentMapIndex = i;
							mapInfo->ownerWidget = this;
							if (i % 2 == 0)
								ver_Scroll_0->AddChild(mapInfo);
							else
								ver_Scroll_1->AddChild(mapInfo);
						}
					}

				}), 0.2f, false);
		}
	}
}

void ULobbyBoardWidget::OnClickEditorMode()
{
	// 에디터 레벨로 넘어가기
	UGameplayStatics::OpenLevel(GetWorld(), FName("Proto_EditorLevel"));
}

void ULobbyBoardWidget::OnClickBattleWaitingRoom()
{
	// mapList[mapIndex]의 정보로 맵 구성.
	ETimeZone timeZone = gi->customMapInfo.timeZone;
	gi->customMapInfo = mapList[mapIndex];
	//		--> 함수로 구현(OnMapSelected)
	switch (timeZone)
	{
	case ETimeZone::Morning:
		UGameplayStatics::OpenLevel(GetWorld(), FName("Proto_Morning"));
		break;
	case ETimeZone::Noon:
		UGameplayStatics::OpenLevel(GetWorld(), FName("Proto_Morning"));
		//UGameplayStatics::OpenLevel(GetWorld(), FName("Proto_Noon"));
		break;
	case ETimeZone::Evening:
		//UGameplayStatics::OpenLevel(GetWorld(), FName("Proto_Evening"));
		UGameplayStatics::OpenLevel(GetWorld(), FName("Proto_Morning"));
		break;
	default:
		break;
	}	
}

void ULobbyBoardWidget::OnClickMovetoLobby()
{
	SwitchCanvas(0);
}

void ULobbyBoardWidget::OnClickBack()
{
	SwitchCanvas(0);
}

void ULobbyBoardWidget::OnMapSelected(int32 idx)
{
	mapIndex = idx;
	gi->customMapInfo = mapList[idx];
}
