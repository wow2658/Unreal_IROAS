// Fill out your copyright notice in the Description page of Project Settings.


#include "GunWidget.h"
#include <UMG/Public/Components/TextBlock.h>
#include <UMG/Public/Components/WidgetSwitcher.h>
#include <UMG/Public/Components/ProgressBar.h>

void UGunWidget::NativeConstruct()
{

}

void UGunWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

}

void UGunWidget::SwithCanvas(int32 idx)
{
	ws_BulletWidgetSwitcher->SetActiveWidgetIndex(idx);
}

void UGunWidget::SetBulletNum(int32 bulletNum)
{
	txt_BulletGaugeNum->SetText(FText::AsNumber(bulletNum));
}
