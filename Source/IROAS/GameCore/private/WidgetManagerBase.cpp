// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetManagerBase.h"
#include "BaseCharacter.h"

UWidgetManagerBase::UWidgetManagerBase()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UWidgetManagerBase::BeginPlay()
{
	Super::BeginPlay();

	baseCharacter = Cast<ABaseCharacter>(GetOwner());
	
}


void UWidgetManagerBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

