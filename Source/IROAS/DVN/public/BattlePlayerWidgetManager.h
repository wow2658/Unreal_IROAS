
#pragma once

#include "CoreMinimal.h"
#include "WidgetManagerBase.h"
#include "BattlePlayerWidgetManager.generated.h"

class ABattlePlayer;
class UWidgetComponent;
class UBattlePlayerWidget;
class UStaticMesh;
class UCurveFloat;
class ABattlePlayerIndicator;
class ABattleGameStateBase;

UCLASS()
class IROAS_API UBattlePlayerWidgetManager : public UWidgetManagerBase
{
	GENERATED_BODY()

public:
	UBattlePlayerWidgetManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	ABattlePlayer* player;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* widgetCompRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBattlePlayerWidget* widgetRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABattlePlayerIndicator> bp_Indicator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABattlePlayerIndicator* indicatorRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> bp_LoadingCube;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* loadingCubeRef;

	ABattleGameStateBase* bgsb;

	float lerpHealthAlpha = 0;
	float curHealthPer = 1.f;
	float targetHealthPer = 1.f;
	float lerpEnergyAlpha = 0;
	float curEnergyPer = 0;
	float targetEnergyPer = 0;

	bool bIndicatorOff = false;

	void IndicateCall(EIndicatorType indicationType);
	void CallSetTimer(float time);
	void SetTutorialWidget();
	void DieIndicate();
	void SetDefaultIndicator();

	void SwitchCanvas(int32 idx);
	void SetHealthPercent();
	void SetEnergyPercent();

	void SetFirstLodingCube();
	void SetSecondeLodingCube();
};
