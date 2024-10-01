

#include "BattlePlayerWidgetManager.h"
#include "BattlePlayer.h"
#include "BattlePlayerWidget.h"
#include "UMG/Public/Components/WidgetComponent.h"
#include <UMG/Public/Components/ProgressBar.h>
#include <UMG/Public/Components/TextBlock.h>
#include "BattlePlayerIndicator.h"
#include <Kismet/KismetMathLibrary.h>
#include "BattleGameStateBase.h"

UBattlePlayerWidgetManager::UBattlePlayerWidgetManager()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UBattlePlayerWidgetManager::BeginPlay()
{
	Super::BeginPlay();

	if(baseCharacter != nullptr) {
		player = Cast<ABattlePlayer>(baseCharacter);
		if (player->widgetComp != nullptr) {
			widgetCompRef = player->widgetComp;
			widgetCompRef->SetWidgetClass(defaultWidgetClass);
			widgetCompRef->SetRelativeLocationAndRotation(FVector(100.f, 0.f, -10.f), FRotator(0.f, 180.f, 0.f));
			widgetCompRef->SetRelativeScale3D(FVector(0.1f));
			widgetCompRef->SetDrawSize(FVector2D(1280.f, 720.f));

			widgetRef = Cast<UBattlePlayerWidget>(widgetCompRef->GetWidget());
			widgetRef->widgetManager = this;
		}
	}
}


void UBattlePlayerWidgetManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (player != nullptr && widgetRef != nullptr) {
		if (curHealthPer >= targetHealthPer) {
			lerpHealthAlpha += DeltaTime;
			lerpHealthAlpha = FMath::Clamp(lerpHealthAlpha, 0, 1);
			widgetRef->pb_HealthBar_Back->SetPercent(FMath::Lerp(curHealthPer, targetHealthPer, lerpHealthAlpha * 2));
		}
		else {
			lerpHealthAlpha += DeltaTime;
			lerpHealthAlpha = FMath::Clamp(lerpHealthAlpha, 0, 1);
			widgetRef->pb_HealthBar_Back->SetPercent(FMath::Lerp(targetHealthPer, curHealthPer, lerpHealthAlpha * 2));
		}

		if (curEnergyPer >= targetEnergyPer) {
			lerpEnergyAlpha += DeltaTime;
			lerpEnergyAlpha = FMath::Clamp(lerpEnergyAlpha, 0, 1);
			widgetRef->pb_EnergyBar_Back->SetPercent(FMath::Lerp(curEnergyPer, targetEnergyPer, lerpEnergyAlpha * 2));
		}
		else {
			lerpEnergyAlpha += DeltaTime;
			lerpEnergyAlpha = FMath::Clamp(lerpEnergyAlpha, 0, 1);
			widgetRef->pb_EnergyBar_Back->SetPercent(FMath::Lerp(targetEnergyPer, curEnergyPer, lerpEnergyAlpha * 2));
		}
	}

	if (player != nullptr && indicatorRef != nullptr && !player->IsDead() && bgsb != nullptr && !bgsb->bIsGameFinished && !bIndicatorOff) {
		if (curHealthPer <= 0.3)
			indicatorRef->HealthUnder30();
		else if (curHealthPer <= 0.1)
			indicatorRef->HealthUnder10();
	}

	GEngine->AddOnScreenDebugMessage(2, 3.0f, FColor::Blue, FString::Printf(TEXT("%.2f \r\n %.2f"), curHealthPer, targetHealthPer));
}

void UBattlePlayerWidgetManager::IndicateCall(EIndicatorType indicationType)
{
	indicatorRef->IndicateStart(indicationType);
}

void UBattlePlayerWidgetManager::CallSetTimer(float time)
{
	widgetRef->SetTImer(time);
}

void UBattlePlayerWidgetManager::SetTutorialWidget()
{
	widgetRef->SetTutorialWidget1();
}

void UBattlePlayerWidgetManager::DieIndicate()
{
	indicatorRef->DieStopAudio();
}

void UBattlePlayerWidgetManager::SetDefaultIndicator()
{
	indicatorRef->SetDefaultIndicator();
}

void UBattlePlayerWidgetManager::SwitchCanvas(int32 idx)
{
	widgetRef->SwitchCanvas(idx);
}

void UBattlePlayerWidgetManager::SetHealthPercent()
{
	lerpHealthAlpha = 0;
	curHealthPer = widgetRef->pb_HealthBar->GetPercent();
	targetHealthPer = player->GetHeath() / player->maxHealth;
	widgetRef->pb_HealthBar->SetPercent(targetHealthPer);
	widgetRef->txt_CurrentHealthCount->SetText(FText::AsNumber(static_cast<int32>(player->GetHeath())));
}

void UBattlePlayerWidgetManager::SetEnergyPercent()
{
	lerpEnergyAlpha = 0;
	curEnergyPer = widgetRef->pb_EnergyBar->GetPercent();
	targetEnergyPer = player->GetEnergy() / player->maxEnergy;
	widgetRef->pb_EnergyBar->SetPercent(targetEnergyPer);
	widgetRef->txt_CurrentEnergyCount->SetText(FText::AsNumber(static_cast<int32>(player->GetEnergy())));
}

void UBattlePlayerWidgetManager::SetFirstLodingCube()
{
	if (loadingCubeRef != nullptr) {
		TArray<UActorComponent*> components;
		loadingCubeRef->GetComponents(components);
		for (UActorComponent* comp : components) {
			if (comp->ComponentHasTag("Image_1")) {
				UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(comp);
				staticMesh->SetVisibility(false);
			}
		}
	}
}

void UBattlePlayerWidgetManager::SetSecondeLodingCube()
{
	if (loadingCubeRef != nullptr) {
		TArray<UActorComponent*> components;
		loadingCubeRef->GetComponents(components);
		for (UActorComponent* comp : components) {
			if (comp->ComponentHasTag("Image_0")) {
				UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(comp);
				staticMesh->SetVisibility(false);
			}
			if (comp->ComponentHasTag("Image_1")) {
				UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(comp);
				staticMesh->SetVisibility(true);
			}
		}
		SwitchCanvas(1);
	}
}
