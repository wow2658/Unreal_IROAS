// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GunWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UWidgetSwitcher;
class AGun;

UCLASS()
class IROAS_API UGunWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UTextBlock* txt_BulletGaugeNum;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UWidgetSwitcher* ws_BulletWidgetSwitcher;

	int32 maxBulletNum = 0;


public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION()
	void SwithCanvas(int32 idx);
	UFUNCTION()
	void SetBulletNum(int32 bulletNum);
};
