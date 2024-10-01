
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattlePlayerWidget.generated.h"

class UProgressBar;
class UWidgetSwitcher;
class USizeBox;
class UTextBlock;
class UImage;
class AMovableFrame;
class ABattleGameModeBase;
class USnapMovementComponent;
class ARageTriggerBox;
class UBattlePlayerWidgetManager;
class ALevelSequenceActor;

UCLASS()
class IROAS_API UBattlePlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UProgressBar* pb_HealthBar;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UProgressBar* pb_EnergyBar;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UProgressBar* pb_HealthBar_Back;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UProgressBar* pb_EnergyBar_Back;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UTextBlock* txt_CurrentEnergyCount;
	UPROPERTY(EditDefaultsOnly, Category = MySettings, meta = (BindWidget))
	UTextBlock* txt_MaxEnergyCount;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UTextBlock* txt_CurrentHealthCount;
	UPROPERTY(EditDefaultsOnly, Category = MySettings, meta = (BindWidget))
	UTextBlock* txt_MaxHealthCount;
	UPROPERTY(EditDefaultsOnly, Category = MySettings, meta = (BindWidget))
	UTextBlock* txt_Timer;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UProgressBar* pb_Loading;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UWidgetSwitcher* ws_BattleSceneWidget;

	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UProgressBar* pb_EnemyCountBar;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UImage* img_Reward;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UImage* img_Boss;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	USizeBox* HealthAndEnergy;


	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UImage* img_SnapPoint_1;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UImage* img_SnapPoint_2;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UImage* img_SnapPoint_3;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UImage* img_SnapPoint_4;
	UPROPERTY(EditDefaultsOnly, Category=MySettings, meta=(BindWidget))
	UImage* img_SnapPoint_5;

	TArray<UImage*> snapPoints;

	AMovableFrame* movableFrame;

	ABattleGameModeBase* gm;

	int32 totalEnemyCount = 0;
	int32 currentEnemyCount = 0;

	USnapMovementComponent* snapMoveComp = nullptr;

	UBattlePlayerWidgetManager* widgetManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ARageTriggerBox* rageBox;

	UPROPERTY(EditAnywhere)
    ALevelSequenceActor* startCineActor = nullptr;

	UPROPERTY(EditAnywhere)
    ALevelSequenceActor* bossCineActor = nullptr;

	bool isFirstCine = true;
	float loadingTime = 0;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION()
	void BackToFirstCanvas();
	UFUNCTION()
	void SwitchCanvas(int32 idx);

	UFUNCTION()
	void RefreshPositionIndicator(const TArray<bool>& occupation);

	UFUNCTION()
	void EnemyCountChanged(int32 enemyCount);
	UFUNCTION()
	void SetIndicatorPosition();

	UFUNCTION()
	void SetTImer(float elapsedTime);

	UFUNCTION()
	void SetTutorialWidget1();
	UFUNCTION()
	void SetTutorialWidget2();
};