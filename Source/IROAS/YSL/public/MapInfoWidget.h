// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapInfoWidget.generated.h"


UCLASS()
class IROAS_API UMapInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_SetMapInfo;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category=MySettings)
	class UTextBlock* txt_MapName;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category=MySettings)
	class UTextBlock* txt_MapCreatorName;

	int32 currentMapIndex = 0;
	class ULobbyBoardWidget* ownerWidget;

	UFUNCTION()
	void SetMapIndex();
};
