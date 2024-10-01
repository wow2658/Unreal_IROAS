// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "RageTriggerBox.generated.h"

class ALevelSequenceActor;

UCLASS()
class IROAS_API ARageTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:

	ARageTriggerBox();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(class AActor* OverlappedActor, class AActor* OtherActor);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA|Rock")
	float rockDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA|Rock")
	float rockSpeed = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA|Slash")
	float slashDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA|Slash")
	float slashSpeed = 2500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA|Boss")
	float bossHp = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA|StartCine")
	bool startCineAutoPlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA|BossCine")
	bool bossCineAutoPlay = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA|StartCine")
    ALevelSequenceActor* startCineActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA|BossCine")
    ALevelSequenceActor* bossCineActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA|StartCine")
	FName startTag = TEXT("StartCine");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA|BossCine")
	FName bossTag = TEXT("BossCine");

	UFUNCTION()
	void OnstartCineFinished();

	UFUNCTION()
	void OnbossCineFinished();

	UFUNCTION()
	void CineAutoPlay(ALevelSequenceActor* LevelSequenceActor, bool bAutoPlay);
};
