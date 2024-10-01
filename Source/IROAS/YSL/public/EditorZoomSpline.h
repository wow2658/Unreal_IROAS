// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EditorZoomSpline.generated.h"

UCLASS()
class IROAS_API AEditorZoomSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	AEditorZoomSpline();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	class USplineMeshComponent* zoomSplineComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	class USceneComponent* startPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	class USceneComponent* endPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	class UArrowComponent* snapPoint;

	class AEditorPlayer* editor;
};
