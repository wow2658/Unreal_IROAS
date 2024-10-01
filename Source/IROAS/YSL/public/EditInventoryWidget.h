// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EditInventoryWidget.generated.h"


UCLASS()
class IROAS_API UEditInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	void OnInitialized();
	void GetOwnerActor(class AEditInventoryWidgetActor* owner);
	void SwitchCanvas(int32 index);
	class AEditInventoryWidgetActor* widgetActor;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UWidgetSwitcher* ws_InventorySwitch;
};
