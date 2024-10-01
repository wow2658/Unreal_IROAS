// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyTutorialWidget.h"
#include <UMG/Public/Components/WidgetSwitcher.h>

void ULobbyTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void ULobbyTutorialWidget::SwitchCanvas(int32 index)
{
	ws_LobbyTutorialSwitch->SetActiveWidgetIndex(index);
}

int32 ULobbyTutorialWidget::GetCanvasIndex()
{
	return ws_LobbyTutorialSwitch->GetActiveWidgetIndex();
}
