// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyTutorialWidget.generated.h"

class UWidgetSwitcher;
class UTextBlock;

UCLASS()
class IROAS_API ULobbyTutorialWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable)
	void SwitchCanvas(int32 index);
	int32 GetCanvasIndex();

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UWidgetSwitcher* ws_LobbyTutorialSwitch;
	//UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	//UTextBlock* txt_Intro_1;
	//UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	//UTextBlock* txt_Intro_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_count_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_count_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_count_3;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_count_4;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_count_5;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* textFade;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* canvasFade;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* imageAnim;
};
