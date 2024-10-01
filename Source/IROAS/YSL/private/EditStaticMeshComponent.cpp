// Fill out your copyright notice in the Description page of Project Settings.


#include "EditStaticMeshComponent.h"
#include "EditButtonWidget.h"
#include "EditorPlayer.h"
#include <Kismet/GameplayStatics.h>

void UEditStaticMeshComponent::BeginPlay()
{
	editor = Cast<AEditorPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	editor->OnStaticVisibleOff.AddDynamic(this, &UEditStaticMeshComponent::VisibilityOff);
}

void UEditStaticMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
}

void UEditStaticMeshComponent::ChangeVisibility(EStaticMeshType meshType)
{
	if (meshType == staticMeshType) {
		this->SetVisibility(true);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("visible onnnnnn")));
	}
	else
		SetVisibility(false);
}

void UEditStaticMeshComponent::CategoryChangeVisibility(bool canVisible)
{
	if(ComponentHasTag(FName("CategoryButton")))
		SetVisibility(canVisible);
}

void UEditStaticMeshComponent::VisibilityOff()
{
	SetVisibility(false);
}

void UEditStaticMeshComponent::BindDelegate()
{
	if (owingButton != nullptr) {
		owingButton->OnStaticVisibleChange.AddDynamic(this, &UEditStaticMeshComponent::ChangeVisibility);
		owingButton->OnCategoryButtonVisibleChange.AddDynamic(this, &UEditStaticMeshComponent::CategoryChangeVisibility);
	}
} 
