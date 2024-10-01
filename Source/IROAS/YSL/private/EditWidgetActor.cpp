// Fill out your copyright notice in the Description page of Project Settings.


#include "EditWidgetActor.h"
#include <UMG/Public/Components/WidgetComponent.h>
#include <Kismet/GameplayStatics.h>
#include "EditorPlayer.h"

AEditWidgetActor::AEditWidgetActor()
{
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(rootComp);

	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	widgetComp->SetupAttachment(RootComponent);
}

void AEditWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	

	FTimerHandle waitHandle;
	GetWorldTimerManager().SetTimer(waitHandle, FTimerDelegate::CreateLambda([&]()
		{
			editor = Cast<AEditorPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			//inventoryWidgetActor = Cast<AEditInventoryWidgetActor>(editor->leftHand->inventoryWidgetComp->GetChildActor());
		}), 1.0f, false);
}

void AEditWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

