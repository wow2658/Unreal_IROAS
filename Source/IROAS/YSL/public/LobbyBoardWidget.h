// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IROASStructs.h"
#include "LobbyBoardWidget.generated.h"

class UWidgetSwitcher;
class UButton;
class UTextBlock;
class UVerticalBox;
class UWidgetAnimation;
class USoundBase;
class UNiagaraSystem;

UCLASS()
class IROAS_API ULobbyBoardWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable)
	void SwitchCanvas(int32 index);


	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UWidgetSwitcher* ws_LobbyGameMenuSwitch;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_StoryMode;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_CustomMode;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_EditorMode;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_MovetoEditorMap;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_MovetoLobby;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_MovetoBattleWaitingRoom;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_MovetoCustomMap;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_Back_0;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_Back_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_Back_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_Search_0;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_Search_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_Search_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UTextBlock* txt_MapName;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UTextBlock* txt_MapCreatorName;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UTextBlock* txt_UserName;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UTextBlock* txt_Intro_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UTextBlock* txt_Intro_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UVerticalBox* ver_Scroll_0;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UVerticalBox* ver_Scroll_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* textFade;


	UFUNCTION()
	void OnClickStoryMode();
	UFUNCTION()
	void OnClickCustomMode();
	UFUNCTION()
	void OnClickEditorMode();
	UFUNCTION()
	void OnClickBattleWaitingRoom();
	UFUNCTION()
	void OnClickMovetoLobby();
	UFUNCTION()
	void OnClickBack();
	/*UFUNCTION()
	void OnClickSearch();*/
	UFUNCTION()
	void OnMapSelected(int32 idx);

	int32 mapIndex = 0;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<class UMapInfoWidget> mapInfoWidget;

private:
	class UIROASGameInstance* gi;
	class ALobbyGameMode* gm;
	class UMapInfoWidget* mapInfo;
	UPROPERTY(EditAnywhere, Category = MySettings)
	TArray<FMapInfoTable> mapList;
};
