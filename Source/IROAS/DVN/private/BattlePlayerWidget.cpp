

#include "BattlePlayerWidget.h"
#include "UMG/Public/Components/WidgetSwitcher.h"
#include "UMG/Public/Components/Image.h"
#include <Kismet/GameplayStatics.h>
#include "MovableFrame.h"
#include <UMG/Public/Components/ProgressBar.h>
#include "BattleGameModeBase.h"
#include <UMG/Public/Components/Image.h>
#include <UMG/Public/Components/TextBlock.h>
#include <UMG/Public/Components/SizeBox.h>
#include "BattlePlayerWidgetManager.h"
#include "RageTriggerBox.h"
#include "LevelSequence/Public/LevelSequenceActor.h"

void UBattlePlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();


	/*movableFrame = Cast<AMovableFrame>(UGameplayStatics::GetActorOfClass(GetWorld(), AMovableFrame::StaticClass()));

	if (movableFrame != nullptr) {
		movableFrame->onPositionChanged.AddDynamic(this, &UBattlePlayerWidget::RefreshPositionIndicator);
	}*/
	TArray<AActor*> frames;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMovableFrame::StaticClass(), frames);
	for (AActor* frame : frames) {
		AMovableFrame* tempFrame = Cast<AMovableFrame>(frame);
		if (tempFrame != nullptr) {
			tempFrame->onPositionChanged.AddDynamic(this, &UBattlePlayerWidget::RefreshPositionIndicator);
		}
	}
	snapPoints.Add(img_SnapPoint_1);
	snapPoints.Add(img_SnapPoint_2);
	snapPoints.Add(img_SnapPoint_3);
	snapPoints.Add(img_SnapPoint_4);
	snapPoints.Add(img_SnapPoint_5);

	gm = GetWorld()->GetAuthGameMode<ABattleGameModeBase>();
	if (gm != nullptr) {
		totalEnemyCount = gm->GetTotalEnemyCount();
		//SetIndicatorPosition();
	}
}

void UBattlePlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (gm != nullptr) {
		currentEnemyCount = gm->CurrentEnemyCount;
		EnemyCountChanged(currentEnemyCount);
	}

	GEngine->AddOnScreenDebugMessage(2, 3.0f, FColor::Blue, FString::Printf(TEXT("widget : %d"), ws_BattleSceneWidget->GetActiveWidgetIndex()));

	if (ws_BattleSceneWidget->GetActiveWidgetIndex() == 1) {
		loadingTime += InDeltaTime;
		pb_Loading->SetPercent(FMath::Lerp(0.f, 1.f, loadingTime*0.4f));
		if (pb_Loading->GetPercent() >= 1) {
			widgetManager->loadingCubeRef->SetActorHiddenInGame(true);
			rageBox = Cast<ARageTriggerBox>(UGameplayStatics::GetActorOfClass(GetWorld(), ARageTriggerBox::StaticClass()));
			if (rageBox)
			{
				startCineActor = rageBox->startCineActor;
				bossCineActor = rageBox->bossCineActor;
				if (isFirstCine) {
					//rageBox->CineAutoPlay(startCineActor, true);
					isFirstCine = false;
					startCineActor->GetSequencePlayer()->Play();
					loadingTime = 0;
				}
				//else
				//	//rageBox->CineAutoPlay(bossCineActor, true);
				//	bossCineActor->GetSequencePlayer()->Play();
			}
		
			BackToFirstCanvas();
		}
	}
}

void UBattlePlayerWidget::BackToFirstCanvas()
{
	SwitchCanvas(0);
}

void UBattlePlayerWidget::SwitchCanvas(int32 idx)
{
	ws_BattleSceneWidget->SetActiveWidgetIndex(idx);
}


void UBattlePlayerWidget::RefreshPositionIndicator(const TArray<bool>& occupation)
{
	int8 len = occupation.Num();
	for (int8 i = 0; i < len; i++) {
		if(occupation[i]) snapPoints[i]->SetRenderOpacity(1.f);
		else snapPoints[i]->SetRenderOpacity(0.4f);
	}
}

void UBattlePlayerWidget::EnemyCountChanged(int32 enemyCount)
{
	pb_EnemyCountBar->SetPercent(static_cast<float> (totalEnemyCount - enemyCount)/ static_cast<float>(totalEnemyCount));
}

void UBattlePlayerWidget::SetIndicatorPosition()
{	
	if (totalEnemyCount == 0) return;
	// 보상 나오는 시점, 보스 나오는 시점
	float rewardPos = 300/(totalEnemyCount / 3);
	float bossPos = 300/(totalEnemyCount / 2) - 5.0f;
	img_Reward->SetRenderTranslation(FVector2D(rewardPos,0));
	img_Boss->SetRenderTranslation(FVector2D(bossPos, 0));

}

void UBattlePlayerWidget::SetTImer(float elapsedTime)
{
	int32 minutes = static_cast<int32>(elapsedTime / 60);
	int32 seconds = static_cast<int32>(elapsedTime) % 60;
	int32 milliseconds = static_cast<int32>((elapsedTime - static_cast<int32>(elapsedTime)) * 100);
	txt_Timer->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d.%02d"), minutes, seconds, milliseconds)));
}

void UBattlePlayerWidget::SetTutorialWidget1()
{
	txt_Timer->SetVisibility(ESlateVisibility::Hidden);
	HealthAndEnergy->SetVisibility(ESlateVisibility::Hidden);
	pb_HealthBar->SetVisibility(ESlateVisibility::Hidden);
	pb_HealthBar_Back->SetVisibility(ESlateVisibility::Hidden);
	pb_EnergyBar->SetVisibility(ESlateVisibility::Hidden);
	pb_EnergyBar_Back->SetVisibility(ESlateVisibility::Hidden);
}

void UBattlePlayerWidget::SetTutorialWidget2()
{
	txt_Timer->SetVisibility(ESlateVisibility::Hidden);
	HealthAndEnergy->SetVisibility(ESlateVisibility::Visible);
	pb_HealthBar->SetVisibility(ESlateVisibility::Visible);
	pb_HealthBar_Back->SetVisibility(ESlateVisibility::Visible);
	pb_EnergyBar->SetVisibility(ESlateVisibility::Visible);
	pb_EnergyBar_Back->SetVisibility(ESlateVisibility::Visible);
}