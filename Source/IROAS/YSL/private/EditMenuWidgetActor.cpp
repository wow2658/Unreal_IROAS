// Fill out your copyright notice in the Description page of Project Settings.


#include "EditMenuWidgetActor.h"
#include <UMG/Public/Components/WidgetComponent.h>
#include "EditMenuWidget.h"
#include "EditStaticMeshComponent.h"
#include "EditInventoryWidgetActor.h"
#include <Kismet/GameplayStatics.h>

AEditMenuWidgetActor::AEditMenuWidgetActor()
{
	PrimaryActorTick.bCanEverTick = true;
	numStaticMeshComp = 5;

	for (int i = 0; i < numStaticMeshComp; i++)
	{
		FString compName = FString::Printf(TEXT("StaticMeshComp_%d"), i);
		FName componentFName(*compName);
		UStaticMeshComponent* newMesh = CreateDefaultSubobject<UStaticMeshComponent>(componentFName);
		staticMeshComp.Add(newMesh);
		newMesh->SetupAttachment(widgetComp);
	}

	for (int i = 0; i < 6; i++) {
		FString compName = FString::Printf(TEXT("EditStaticMeshComp_%d"), i);
		FName componentFName(*compName);
		UEditStaticMeshComponent* newMesh = CreateDefaultSubobject<UEditStaticMeshComponent>(componentFName);
		editstaticMeshComp.Add(newMesh);
		newMesh->SetupAttachment(widgetComp);
	}

	pressedCheckMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pressed Check"));
	pressedCheckMesh->SetupAttachment(widgetComp);
	pressedCheckMesh->SetRelativeRotation(FRotator(-90, 0, 0));
	pressedCheckMesh->SetVisibility(false);

#if WITH_EDITOR
	FProperty* StaticMeshCompProperty = FindFieldChecked<FProperty>(GetClass(), GET_MEMBER_NAME_CHECKED(AEditMenuWidgetActor, staticMeshComp));
	StaticMeshCompProperty->SetPropertyFlags(CPF_Edit);
	StaticMeshCompProperty->SetPropertyFlags(CPF_BlueprintVisible);
	FProperty* EditStaticMeshCompProperty = FindFieldChecked<FProperty>(GetClass(), GET_MEMBER_NAME_CHECKED(AEditMenuWidgetActor, editstaticMeshComp));
	EditStaticMeshCompProperty->SetPropertyFlags(CPF_Edit);
	EditStaticMeshCompProperty->SetPropertyFlags(CPF_BlueprintVisible);
#endif
}

void AEditMenuWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	inventoryWidgetActor = Cast<AEditInventoryWidgetActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AEditInventoryWidgetActor::StaticClass()));

	if (widgetComp)
	{
		menuWidget = Cast<UEditMenuWidget>(widgetComp->GetWidget());
		menuWidget->GetOwnerActor(this);
		menuWidget->OnInitialized();
	}
}

void AEditMenuWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
