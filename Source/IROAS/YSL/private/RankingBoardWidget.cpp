// Fill out your copyright notice in the Description page of Project Settings.


#include "RankingBoardWidget.h"
#include "BattleGameStateBase.h"
#include <UMG/Public/Components/TextBlock.h>
#include <UMG/Public/Blueprint/WidgetTree.h>
#include <UMG/Public/Components/Border.h>
#include <Kismet/GameplayStatics.h>
#include <UMG/Public/Components/WidgetSwitcher.h>

void URankingBoardWidget::NativeConstruct()
{
	gs = GetWorld()->GetGameState<ABattleGameStateBase>();

	//gs->onGameFinishDelegate.AddDynamic(this, &URankingBoardWidget::SetLeaderBoardsInfo);
}

void URankingBoardWidget::SetLeaderBoardPlayerName(uint8 index, FString playerName)
{
	/*if(index == 0)
		txt_PlayerName_0->SetText(FText::FromString(playerName));
	else if (index == 1)
		txt_PlayerName_1->SetText(FText::FromString(playerName));
	else if (index == 2)
		txt_PlayerName_2->SetText(FText::FromString(playerName));
	else if (index == 3)
		txt_PlayerName_3->SetText(FText::FromString(playerName));
	else if (index == 4)
		txt_PlayerName_4->SetText(FText::FromString(playerName));
	else if (index == 5)
		txt_PlayerName_5->SetText(FText::FromString(playerName));

	*/
	FName textBlockName = FName(*FString::Printf(TEXT("txt_PlayerName_%d"), index));
	UTextBlock* txtPlayerName = WidgetTree->FindWidget<UTextBlock>(textBlockName);
	if (txtPlayerName != nullptr)
		txtPlayerName->SetText(FText::FromString(playerName));

}

void URankingBoardWidget::SetLeaderBoardPlayTime(uint8 index, int32 playTime)
{
	int32 totalSeconds = static_cast<int32>(playTime);
	int32 minutes = (totalSeconds / (1000*60)) % 60;
	int32 seconds = (totalSeconds / 1000) % 60;
	int32 milliseconds = totalSeconds % 1000;
	FString timeString = FString::Printf(TEXT("%02d:%02d.%02d"), minutes, seconds, milliseconds/10);
	/*if (index == 0)
		txt_PlayTime_0->SetText(FText::FromString(timeString));
	else if (index == 1)
		txt_PlayTime_1->SetText(FText::FromString(timeString));
	else if (index == 2)
		txt_PlayTime_2->SetText(FText::FromString(timeString));
	else if (index == 3)
		txt_PlayTime_3->SetText(FText::FromString(timeString));
	else if (index == 4)
		txt_PlayTime_4->SetText(FText::FromString(timeString));
	else if (index == 5)
		txt_PlayTime_5->SetText(FText::FromString(timeString));*/

	FName textBlockName = FName(*FString::Printf(TEXT("txt_PlayTime_%d"), index));
	UTextBlock* txtPlayTime = WidgetTree->FindWidget<UTextBlock>(textBlockName);
	if (txtPlayTime != nullptr)
		txtPlayTime->SetText(FText::FromString(timeString));
}

void URankingBoardWidget::SetPlayerTime()
{
	float playerTime = gs->gameTimeElapsed;
	int32 minutes = static_cast<int32>(playerTime/60);
	int32 seconds = static_cast<int32>(playerTime)%60;
	int32 milliseconds = static_cast<int32>((playerTime - static_cast<int32>(playerTime)) * 100);
	txt_PlayerTime->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d.%02d"), minutes, seconds, milliseconds)));
	//txt_PlayerTime->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), minutes, seconds)));
}

void URankingBoardWidget::CheckPlayerName(uint8 index)
{
	// 자기 기록이 순위에 올랐을 때 하이라이팅 처리
	/*FName borderName = FName(*FString::Printf(TEXT("bd_border_%d"), index));
	UBorder* border = WidgetTree->FindWidget<UBorder>(borderName);
	if (border) {
		border->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(BorderAnim);
	}*/
	FName textName = FName(*FString::Printf(TEXT("txt_PlayerName_%d"), index));
	UTextBlock* name = WidgetTree->FindWidget<UTextBlock>(textName);
	if (name) {
		name->SetColorAndOpacity(FLinearColor(0.8f, 0.7f, 0.08f, 1.f));
		FSlateFontInfo fontInfo = name->GetFont();
		//fontInfo.OutlineSettings.OutlineSize = 1;
		fontInfo.Size = 38;
		name->SetFont(fontInfo);
	}
	FName textTime = FName(*FString::Printf(TEXT("txt_PlayTime_%d"), index));
	UTextBlock* time = WidgetTree->FindWidget<UTextBlock>(textTime);
	if (time) {
		time->SetColorAndOpacity(FLinearColor(0.8f, 0.7f, 0.08f, 1.f));
		FSlateFontInfo fontInfo = time->GetFont();
		//fontInfo.OutlineSettings.OutlineSize = 1;
		fontInfo.Size = 33;
		time->SetFont(fontInfo);
	}
}

void URankingBoardWidget::SwitchCanvas(uint8 index)
{
	ws_Switcher->SetActiveWidgetIndex(index);
}