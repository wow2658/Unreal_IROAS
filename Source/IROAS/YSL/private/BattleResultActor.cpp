// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleResultActor.h"
#include <UMG/Public/Components/WidgetComponent.h>
#include <Components/ChildActorComponent.h>
#include "ShootableUIActor.h"
#include "RankingBoardWidget.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <Components/TextRenderComponent.h>

ABattleResultActor::ABattleResultActor()
{
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(rootComp);
	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	widgetComp->SetupAttachment(RootComponent);
	widgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	childComp1 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildComponent1"));
	childComp1->SetupAttachment(widgetComp);
	childComp2 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildComponent2"));
	childComp2->SetupAttachment(widgetComp);
	childComp3 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildComponent3"));
	childComp3->SetupAttachment(widgetComp);
}

void ABattleResultActor::BeginPlay()
{
	Super::BeginPlay();

	TArray<UChildActorComponent*> childComps;
	childComps.Add(childComp1);
	childComps.Add(childComp2);
	childComps.Add(childComp3);

	for (int32 i = 0; i < childComps.Num(); i++) {
		AShootableUIActor* orbActor = Cast<AShootableUIActor>(childComps[i]->GetChildActor());
		if (orbActor) {
			orbActor->GetRewardActor(this);
			orbActors.Add(orbActor);
		}
	}
	resultWidget = Cast<URankingBoardWidget>(widgetComp->GetWidget());
	
	RankingActor();
	FailActor();
}

void ABattleResultActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABattleResultActor::RankingActor()
{
	if (ActorHasTag("Ranking")) {
		orbActors[0]->niagaraComp->Deactivate();
		orbActors[1]->niagaraComp->Deactivate();
		FTimerHandle widgetHandle;
		GetWorldTimerManager().SetTimer(widgetHandle, FTimerDelegate::CreateLambda([&]() {
			resultWidget->PlayAnimation(resultWidget->RankingIn, 0.0f);
			}), 1.f, false);
		FTimerHandle orbHandle;
		GetWorldTimerManager().SetTimer(orbHandle, FTimerDelegate::CreateLambda([&]() {
			rankingScale = true;
			if (childComp1 != nullptr) {
				orbActors[0]->niagaraComp->Activate(true);
				orbActors[0]->textRenderComp->SetVisibility(true);
			}
			if (childComp2 != nullptr) {
				orbActors[1]->niagaraComp->Activate(true);
				orbActors[1]->textRenderComp->SetVisibility(true);
			}
			}), 2.5f, false);
	}
}	

void ABattleResultActor::FailActor()
{
	if (ActorHasTag("Fail")) {
		resultWidget->SwitchCanvas(1);
		orbActors[0]->niagaraComp->Deactivate();
		orbActors[1]->niagaraComp->Deactivate();
		FTimerHandle orbHandle;
		GetWorldTimerManager().SetTimer(orbHandle, FTimerDelegate::CreateLambda([&]() {
			rankingScale = true;
			if (childComp1 != nullptr) {
				orbActors[0]->niagaraComp->Activate(true);
				orbActors[0]->textRenderComp->SetVisibility(true);
			}
			if (childComp2 != nullptr) {
				orbActors[1]->niagaraComp->Activate(true);
				orbActors[1]->textRenderComp->SetVisibility(true);
			}
			}), 0.8f, false);
	}
}

