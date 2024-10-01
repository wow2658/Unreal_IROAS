// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RankingBoardWidget.generated.h"

class UWidgetSwitcher;
class UTextBlock;
class UBorder;
class ABattleGameStateBase;

UCLASS()
class IROAS_API URankingBoardWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UWidgetSwitcher* ws_Switcher;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayerName_0;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayerName_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayerName_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayerName_3;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayerName_4;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayerName_5;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayTime_0;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayTime_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayTime_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayTime_3;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayTime_4;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayTime_5;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UBorder* bd_border_0;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UBorder* bd_border_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UBorder* bd_border_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UBorder* bd_border_3;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UBorder* bd_border_4;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UBorder* bd_border_5;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	UTextBlock* txt_PlayerTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* BorderAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* RankingIn;

	ABattleGameStateBase* gs;

	void SetLeaderBoardPlayerName(uint8 index, FString playerName);
	void SetLeaderBoardPlayTime(uint8 index, int32 playTime);
	void SetPlayerTime();
	UFUNCTION(BlueprintCallable)
	void CheckPlayerName(uint8 index);

	UFUNCTION()
	void SwitchCanvas(uint8 index);


};
