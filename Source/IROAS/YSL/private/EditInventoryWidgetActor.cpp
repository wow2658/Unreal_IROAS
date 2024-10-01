// Fill out your copyright notice in the Description page of Project Settings.


#include "EditInventoryWidgetActor.h"
#include "EditInventoryWidget.h"
#include <UMG/Public/Components/WidgetComponent.h>
#include "EditStaticMeshComponent.h"
#include "EditButtonWidget.h"

AEditInventoryWidgetActor::AEditInventoryWidgetActor()
{
	PrimaryActorTick.bCanEverTick = true;
	numStaticMeshComp = 24;

	for (int i = 0; i < numStaticMeshComp; i++)
	{
		FString compName = FString::Printf(TEXT("StaticMeshComp_%d"), i);
		FName componentFName(*compName);
		UEditStaticMeshComponent* newMesh = CreateDefaultSubobject<UEditStaticMeshComponent>(componentFName);
		staticMeshComp.Add(newMesh);
		newMesh->SetupAttachment(widgetComp);
		newMesh->SetRelativeRotation(FRotator(0,-90,0));
		newMesh->SetRelativeScale3D(FVector(0.4f));
	}

#if WITH_EDITOR
	FProperty* Property = FindFieldChecked<FProperty>(GetClass(), GET_MEMBER_NAME_CHECKED(AEditInventoryWidgetActor, staticMeshComp));
	Property->SetPropertyFlags(CPF_Edit);
	Property->SetPropertyFlags(CPF_BlueprintVisible);
#endif                         
}

void AEditInventoryWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	if (widgetComp)
	{
		inventoryWidget = Cast<UEditInventoryWidget>(widgetComp->GetWidget());
		inventoryWidget->GetOwnerActor(this);
		inventoryWidget->OnInitialized();
	}
}

void AEditInventoryWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//void AEditInventoryWidgetActor::ChangeVisibility(EStaticMeshType meshType)
//{
//	if (meshType == staticMeshType) {
//		//SetVisibility(true);
//		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("visible onnnnnn")));
//	}
//	//else
//		//SetVisibility(false);
//
//}
