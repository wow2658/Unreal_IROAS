// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IROASEnums.h"
#include "EditButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaticVisibleChange, EStaticMeshType, MeshType); 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategoryButtonVisibleChange, bool, value);

UCLASS()
class IROAS_API UEditButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_Button;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySetting)
	class UStaticMeshComponent* buttonMesh;

	UPROPERTY(BlueprintAssignable, Category = MySetting)
	FOnStaticVisibleChange OnStaticVisibleChange;

	UPROPERTY(BlueprintAssignable, Category = MySetting)
	FOnCategoryButtonVisibleChange OnCategoryButtonVisibleChange;

	float hoverSize = 13;
	float unHoverSize = 10;


	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnhovered();
	UFUNCTION()
	void OnPressed();

	class UEditMenuWidget* menuWidget;
	class AEditMenuWidgetActor* owningActor;
	class AEditInventoryWidgetActor* inventoryWidgetActor;
	class AEditorPlayer* editor;
	class UEditStaticMeshComponent* inventoryStaticMesh;

};
