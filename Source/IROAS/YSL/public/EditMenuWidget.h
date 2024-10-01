// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EditMenuWidget.generated.h"

UCLASS()
class IROAS_API UEditMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	void OnInitialized();
	void GetOwnerActor(class AEditMenuWidgetActor* owner);
	void PanelVisibilityOn(class UCanvasPanel* canvas, bool bisVisible);
	class AEditMenuWidgetActor* widgetActor;
	//class AEditInventoryWidgetActor* inventoryWidgetActor;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UCanvasPanel* canvas_Menu;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UCanvasPanel* canvas_EnemyProperty;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UCanvasPanel* canvas_PropCategory;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UCanvasPanel* canvas_SpawnProperty;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class USlider* slide_SpawnAmount;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class USlider* slide_SpawnInterval;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class USlider* slide_SpawnStartDelay;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class USlider* slide_EnemyHP;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class USlider* slide_EnemyPower;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UTextBlock* txt_SpawnAmount;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UTextBlock* txt_SpawnInterval;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UTextBlock* txt_SpawnStartDelay;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UTextBlock* txt_EnemyHP;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UTextBlock* txt_EnemyPower;

	UFUNCTION()
	void OnSlideSpawnAmountMoved(float value);
	UFUNCTION()
	void OnSlideSpawnIntervalMoved(float value);
	UFUNCTION()
	void OnSlideSpawnStartDelayMoved(float value);
	UFUNCTION()
	void OnSlideEnemyHPMoved(float value);
	UFUNCTION()
	void OnSlideEnemyPowerMoved(float value);
};
