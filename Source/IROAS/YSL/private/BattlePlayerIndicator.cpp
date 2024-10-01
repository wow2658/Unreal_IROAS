// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerIndicator.h"
#include <Kismet/GameplayStatics.h>
#include <Components/AudioComponent.h>
#include "IROASEnums.h"

ABattlePlayerIndicator::ABattlePlayerIndicator()
{
	PrimaryActorTick.bCanEverTick = true;
	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(rootComp);

	indicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Indicator"));
	indicator->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempIndicatorMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cube"));
	if (tempIndicatorMesh.Succeeded()) {
		indicator->SetStaticMesh(tempIndicatorMesh.Object);
		indicator->SetRelativeLocation(FVector(0, 0, -35));
		indicator->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.65f));

	}
	indicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	audioComp->SetupAttachment(RootComponent);
}

void ABattlePlayerIndicator::BeginPlay()
{
	Super::BeginPlay();

	SetMaterialArray();
	/*indicatorMat = UMaterialInstanceDynamic::Create(indicator->GetMaterial(0), this);
	indicator->SetMaterial(0, indicatorMat);*/

	FOnTimelineFloat onProgressIndicator;
	onProgressIndicator.BindUFunction(this, FName("OnStartIndication"));
	FOnTimelineEvent onFinishIndicator;
	onFinishIndicator.BindUFunction(this, FName("OnFinishIndication"));

	indicatorTimeline.AddInterpFloat(indicationCurve, onProgressIndicator);
	indicatorTimeline.SetTimelineFinishedFunc(onFinishIndicator);
	indicatorTimeline.SetTimelineLength(indicationTime);

	audioComp->Stop();
}

void ABattlePlayerIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	indicatorTimeline.TickTimeline(DeltaTime);
}


void ABattlePlayerIndicator::IndicateStart(EIndicatorType indicationType)
{
	if (indicationType == EIndicatorType::HealthHeal) {
		currentType = EIndicatorType::HealthHeal;
		indicator->SetMaterial(0, matArray[0]);
		indicationColor = FLinearColor(0, 0.8f, 0, 1.0f);
		indicatorMat = matArray[0];
		indicatorMat->SetVectorParameterValue(FName("IndicationColor"), indicationColor);
		//indicatorMat->SetScalarParameterValue(FName("Gradient"), 1);
	}
	else if (indicationType == EIndicatorType::EnergyHeal) {
		currentType = EIndicatorType::EnergyHeal;
		indicator->SetMaterial(0, matArray[0]);
		//indicationColor = FLinearColor(0, 0.6f, 1.0f, 1.0f);
		indicatorMat = matArray[0];
		//indicatorMat->SetVectorParameterValue(FName("IndicationColor"), indicationColor);
		//indicatorMat->SetScalarParameterValue(FName("Gradient"), 1);
	}
	else if (indicationType == EIndicatorType::Damage) {
		currentType = EIndicatorType::Damage;
		indicator->SetMaterial(0, matArray[1]);
		indicatorMat = matArray[1];
		//indicationColor = FLinearColor(0.6f, 0, 0, 1.0f);
		//indicatorMat->SetScalarParameterValue(FName("Gradient"), 1);
	}
	else if (indicationType == EIndicatorType::LeftMove) {
		currentType = EIndicatorType::LeftMove;
		indicator->SetMaterial(0, matArray[2]);
		indicatorMat = matArray[2];
		/*indicationColor = FLinearColor(0, 0, 0, 1.0f);
		indicatorMat->SetScalarParameterValue(FName("MoveDirection"), 0.75f);
		indicatorMat->SetScalarParameterValue(FName("Gradient"), 0);*/
	}
	else if (indicationType == EIndicatorType::RightMove) {
		currentType = EIndicatorType::RightMove;
		indicator->SetMaterial(0, matArray[3]);
		indicatorMat = matArray[3];
		/*indicationColor = FLinearColor(0, 0, 0, 1.0f);
		indicatorMat->SetScalarParameterValue(FName("MoveDirection"), 0.25f);
		indicatorMat->SetScalarParameterValue(FName("Gradient"), 0);*/
	}
	else if (indicationType == EIndicatorType::Default) {
		currentType = EIndicatorType::Default;
		indicator->SetMaterial(0, matArray[4]);
		indicatorMat = matArray[4];
	}
	indicatorTimeline.Play();
}

void ABattlePlayerIndicator::FinishIndication()
{
	indicatorTimeline.Reverse();
}

void ABattlePlayerIndicator::OnStartIndication(float output)
{
	if (currentType == EIndicatorType::Damage) {
		float newVal = FMath::Lerp(0.0f, 0.5f, output);
		indicatorMat->SetScalarParameterValue(FName("Opacity"), newVal);
	}
	else if (currentType == EIndicatorType::EnergyHeal) {
		float newVal = FMath::Lerp(0.0f, 1.f, output);
		indicatorMat->SetScalarParameterValue(FName("Opacity"), newVal);
	}
	else if (currentType == EIndicatorType::Default) {
		indicatorMat->SetScalarParameterValue(FName("Opacity"), 0);
	}
	else {
		float newVal = FMath::Lerp(0.0f, 0.3f, output);
		indicatorMat->SetScalarParameterValue(FName("Opacity"), newVal);
	}
}

void ABattlePlayerIndicator::OnFinishIndication()
{
	FinishIndication();
}

void ABattlePlayerIndicator::HealthUnder30()
{
	if (!audioComp->IsPlaying())
		audioComp->FadeIn(1.2f);

	/*indicatorMat->SetScalarParameterValue(FName("Opacity"), 0.5f);
	IndicateStart(EIndicatorType::Damage);*/
	indicator->SetMaterial(0, matArray[1]);
	indicatorMat = matArray[1];
	indicatorMat->SetScalarParameterValue(FName("Opacity"), 0.7f);
}

void ABattlePlayerIndicator::HealthUnder10()
{
	if (audioComp->IsPlaying())
		audioComp->SetVolumeMultiplier(1.5f);

	indicator->SetMaterial(0, matArray[1]);
	indicatorMat = matArray[1];
	indicatorMat->SetScalarParameterValue(FName("Opacity"), 0.9f);
}

void ABattlePlayerIndicator::SetDefaultIndicator()
{
	IndicateStart(EIndicatorType::Default);
}

void ABattlePlayerIndicator::DieStopAudio()
{
	if (audioComp->IsPlaying()) {
		audioComp->Stop();
		//GEngine->AddOnScreenDebugMessage(2, 3.0f, FColor::Blue, FString::Printf(TEXT("audiocomp stop")));
	}
}

void ABattlePlayerIndicator::SetMaterialArray()
{
	UMaterialInterface* mat1 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/YSL/Mat/M_Indi_Basic"));
	UMaterialInstanceDynamic* matInstance1 = UMaterialInstanceDynamic::Create(mat1, nullptr);
	UMaterialInterface* mat2 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/YSL/Mat/M_Indi_Damage"));
	UMaterialInstanceDynamic* matInstance2 = UMaterialInstanceDynamic::Create(mat2, nullptr);
	UMaterialInterface* mat3 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/YSL/Mat/M_Indi_LeftMove"));
	UMaterialInstanceDynamic* matInstance3 = UMaterialInstanceDynamic::Create(mat3, nullptr);
	UMaterialInterface* mat4 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/YSL/Mat/M_Indi_RightMove"));
	UMaterialInstanceDynamic* matInstance4 = UMaterialInstanceDynamic::Create(mat4, nullptr);
	UMaterialInterface* mat5 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/YSL/Mat/M_Indi_Default"));
	UMaterialInstanceDynamic* matInstance5 = UMaterialInstanceDynamic::Create(mat5, nullptr);
	matArray.Add(matInstance1);
	matArray.Add(matInstance2);
	matArray.Add(matInstance3);
	matArray.Add(matInstance4);
	matArray.Add(matInstance5);
}
