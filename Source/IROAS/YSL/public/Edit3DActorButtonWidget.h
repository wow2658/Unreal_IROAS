// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Edit3DActorButtonWidget.generated.h"

UCLASS()
class IROAS_API UEdit3DActorButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_Button;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySetting)
	class UEditStaticMeshComponent* buttonMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySetting)
	TSubclassOf<class AEditableActor> spawnableMesh;

	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnhovered();
	UFUNCTION()
	void OnPressed();

	UFUNCTION()
	void ChangeTimeZone(ETimeZone tz);
	// 누가 timezone을 바꿔야 할까?? player component로??
	ETimeZone timezone;

	FVector curSize = FVector(1.0f);
	float hoverSize = 0.5f;
	float unHoverSize = 0.4f;

private:
	class AEditorPlayer* editor;
};
