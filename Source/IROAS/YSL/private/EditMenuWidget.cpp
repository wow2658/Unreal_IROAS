// Fill out your copyright notice in the Description page of Project Settings.


#include "EditMenuWidget.h"
#include <UMG/Public/Components/Button.h>
#include "EditMenuWidgetActor.h"
#include "EditButtonWidget.h"
#include "EditInventoryWidgetActor.h"
#include "EditableSpawner.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "EditStaticMeshComponent.h"
#include <UMG/Public/Components/CanvasPanel.h>
#include "EditorPlayer.h"
#include "EditWidgetActor.h"

void UEditMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	slide_SpawnAmount->OnValueChanged.AddDynamic(this, &UEditMenuWidget::OnSlideSpawnAmountMoved);
	slide_SpawnInterval->OnValueChanged.AddDynamic(this, &UEditMenuWidget::OnSlideSpawnIntervalMoved);
	slide_SpawnStartDelay->OnValueChanged.AddDynamic(this, &UEditMenuWidget::OnSlideSpawnStartDelayMoved);
	slide_EnemyHP->OnValueChanged.AddDynamic(this, &UEditMenuWidget::OnSlideEnemyHPMoved);
	slide_EnemyPower->OnValueChanged.AddDynamic(this, &UEditMenuWidget::OnSlideEnemyPowerMoved);

	slide_EnemyHP->SetValue(100);
	slide_EnemyPower->SetValue(100);
}

void UEditMenuWidget::OnInitialized()
{
	for (int i = 0; i < 10; i++) {
		FString WidgetName = FString::Printf(TEXT("WBP_Button_%d"), i);
		UEditButtonWidget* buttons = Cast<UEditButtonWidget>(GetWidgetFromName(FName(*WidgetName)));
		if (buttons) {
			//menu 위젯 스태틱메쉬 연결해주기
			if(widgetActor->staticMeshComp[i] != nullptr) {
				buttons->buttonMesh = widgetActor->staticMeshComp[i];     
				buttons->owningActor = widgetActor;
				buttons->menuWidget = this;
				// inventory 위젯 스태틱메쉬 연결해주기
				for (int j = 0; j < 24; j++) {
					widgetActor->inventoryWidgetActor->staticMeshComp[j]->owingButton = buttons;
					widgetActor->inventoryWidgetActor->staticMeshComp[j]->BindDelegate();
				}
				for (int j = 0; j < 6; j++) {
					widgetActor->editstaticMeshComp[j]->owingButton = buttons;
					widgetActor->editstaticMeshComp[j]->BindDelegate();
				}
			}
			else if (widgetActor->editstaticMeshComp[i - 5] != nullptr) {
				widgetActor->editstaticMeshComp[i-5]->owingButton = buttons;
				widgetActor->editstaticMeshComp[i-5]->BindDelegate();
				for (int j = 0; j < 24; j++) {
					widgetActor->inventoryWidgetActor->staticMeshComp[j]->owingButton = buttons;
					widgetActor->inventoryWidgetActor->staticMeshComp[j]->BindDelegate();
				}
			}
		}
	}
}

void UEditMenuWidget::GetOwnerActor(class AEditMenuWidgetActor* owner)
{
	widgetActor = owner;
}

void UEditMenuWidget::PanelVisibilityOn(class UCanvasPanel* canvas, bool bisVisible)
{
	if (bisVisible)
		canvas->SetVisibility(ESlateVisibility::Visible);
	else
		canvas->SetVisibility(ESlateVisibility::Hidden);
}

void UEditMenuWidget::OnSlideSpawnAmountMoved(float value)
{
	txt_SpawnAmount->SetText(FText::AsNumber((int32)value));
	if (widgetActor->editor->editableSpawner != nullptr)
		widgetActor->editor->editableSpawner->spawnerInfo.spawnAmount = value;
}

void UEditMenuWidget::OnSlideSpawnIntervalMoved(float value)
{
	txt_SpawnInterval->SetText(FText::AsNumber((int32)value));
	if (widgetActor->editor->editableSpawner != nullptr)
		widgetActor->editor->editableSpawner->spawnerInfo.spawnInterval = value;
}

void UEditMenuWidget::OnSlideSpawnStartDelayMoved(float value)
{
	txt_SpawnStartDelay->SetText(FText::AsNumber((int32)value));
	if (widgetActor->editor->editableSpawner != nullptr)
		widgetActor->editor->editableSpawner->spawnerInfo.spawnStartDelay = value;
}

void UEditMenuWidget::OnSlideEnemyHPMoved(float value)
{
	txt_EnemyHP->SetText(FText::AsNumber((int32)value));
	if (widgetActor->editor->editableSpawner != nullptr)
		widgetActor->editor->editableSpawner->spawnerInfo.enemyHP = value;
}

void UEditMenuWidget::OnSlideEnemyPowerMoved(float value)
{
	txt_EnemyPower->SetText(FText::AsNumber((int32)value));
	if (widgetActor->editor->editableSpawner != nullptr)
		widgetActor->editor->editableSpawner->spawnerInfo.enemyPower = value;
}
