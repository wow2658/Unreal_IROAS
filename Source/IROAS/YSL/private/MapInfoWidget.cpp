// Fill out your copyright notice in the Description page of Project Settings.


#include "MapInfoWidget.h"
#include "LobbyBoardWidget.h"
#include <UMG/Public/Components/Button.h>

void UMapInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	btn_SetMapInfo->OnPressed.AddDynamic(this, &UMapInfoWidget::SetMapIndex);
}

void UMapInfoWidget::SetMapIndex()
{	
	//ownerWidget->mapIndex = currentMapIndex;
	ownerWidget->OnMapSelected(currentMapIndex);
	
}
