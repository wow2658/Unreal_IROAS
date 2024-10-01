// Fill out your copyright notice in the Description page of Project Settings.


#include "EditInventoryWidget.h"
#include "EditInventoryWidgetActor.h"
#include "Edit3DActorButtonWidget.h"
#include <UMG/Public/Components/WidgetSwitcher.h>

void UEditInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UEditInventoryWidget::OnInitialized()
{
	for (int i = 0; i < widgetActor->numStaticMeshComp; i++) {
		FString WidgetName = FString::Printf(TEXT("WBP_InventoryButton_%d"), i);
		class UEdit3DActorButtonWidget* buttons = Cast<UEdit3DActorButtonWidget>(GetWidgetFromName(FName(*WidgetName)));
		if (buttons) {
			if (widgetActor->staticMeshComp[i] != nullptr) {
				buttons->buttonMesh = widgetActor->staticMeshComp[i];
			}
			if (widgetActor->spawnable_BP[i] != nullptr)
				buttons->spawnableMesh = widgetActor->spawnable_BP[i];
		}
	}
}

void UEditInventoryWidget::GetOwnerActor(class AEditInventoryWidgetActor* owner)
{
	widgetActor = owner;
}
 
void UEditInventoryWidget::SwitchCanvas(int32 index)
{
	ws_InventorySwitch->SetActiveWidgetIndex(index);
}