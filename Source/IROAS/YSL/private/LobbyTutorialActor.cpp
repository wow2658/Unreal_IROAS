// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyTutorialActor.h"
#include <Components/BoxComponent.h>
#include <UMG/Public/Components/WidgetComponent.h>
#include "LobbyTutorialWidget.h"
#include <Kismet/GameplayStatics.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>
#include "ShootableUIActor.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include "BattlePlayerWidget.h"
#include "BattlePlayer.h"
#include "LobbyTutorialOrb.h"
#include "BattlePlayerWidgetManager.h"
#include <Engine/TimerHandle.h>
#include "MovableFrame.h"
#include "PlayerHand.h"
#include "Gun.h"
#include <UMG/Public/Components/ProgressBar.h>
#include <UMG/Public/Components/TextBlock.h>
#include <Components/AudioComponent.h>

ALobbyTutorialActor::ALobbyTutorialActor()
{
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(rootComp);
	widgetComp1 = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp1"));
	widgetComp1->SetupAttachment(RootComponent);
	widgetComp2 = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp2"));
	widgetComp2->SetupAttachment(widgetComp1);
	widgetComp3 = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp3"));
	widgetComp3->SetupAttachment(widgetComp1);
}

void ALobbyTutorialActor::BeginPlay()
{
	Super::BeginPlay();
	
	lobbyWidget1 = Cast<ULobbyTutorialWidget>(widgetComp1->GetWidget());
	lobbyWidget2 = Cast<ULobbyTutorialWidget>(widgetComp2->GetWidget());
	lobbyWidget2->SwitchCanvas(14);
	widgetComp2->SetVisibility(false);
	lobbyWidget3 = Cast<ULobbyTutorialWidget>(widgetComp3->GetWidget());
	lobbyWidget3->SwitchCanvas(14);
	widgetComp3->SetVisibility(false);
	
	for (int8 i = 0; i < 3; i++) {
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FTransform spawnTransform;
		spawnTransform.SetLocation(FVector(0.f, 0.f, -1000.f));
		tutorialOrb_Gun = GetWorld()->SpawnActor<ALobbyTutorialOrb>(bp_tutorialOrb_Gun, spawnTransform, param);

		if (tutorialOrb_Gun != nullptr) {
			tutorialOrb_Gun->niagaraComp->SetVisibility(false);
			tutorialOrb_Gun->boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			tutorialOrb_Gun->widgetActor = this;
			//tutorialOrb_Gun->player = lobbyPlayer;
			tutorialOrb_Gun->audioComp->Deactivate();
			tutorialOrb_Gun->Tags.Add("GunOrbs");
			tutorialOrbPool_Gun.Enqueue(tutorialOrb_Gun);
		}
	}

	for (int8 i = 0; i < 3; i++) {
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FTransform spawnTransform;
		spawnTransform.SetLocation(FVector(0.f, 0.f, -1000.f));
		tutorialOrb_Sword = GetWorld()->SpawnActor<ALobbyTutorialOrb>(bp_tutorialOrb_Sword, spawnTransform, param);

		if (tutorialOrb_Sword != nullptr) {
			tutorialOrb_Sword->niagaraComp->SetVisibility(false);
			tutorialOrb_Sword->boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			tutorialOrb_Sword->widgetActor = this;
			//tutorialOrb_Sword->player = lobbyPlayer;
			tutorialOrb_Sword->audioComp->Deactivate();
			tutorialOrb_Sword->Tags.Add("SwordOrbs");
			tutorialOrbPool_Sword.Enqueue(tutorialOrb_Sword);
		}
	}

	for (int8 i = 0; i < 2; i++) {
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FTransform spawnTransform;
		spawnTransform.SetLocation(FVector(0.f, 0.f, -1000.f));
		tutorialOrb_SwordEnergy = GetWorld()->SpawnActor<ALobbyTutorialOrb>(bp_tutorialOrb_SwordEnergy, spawnTransform, param);

		if (tutorialOrb_SwordEnergy != nullptr) {
			tutorialOrb_SwordEnergy->niagaraComp->SetVisibility(false);
			tutorialOrb_SwordEnergy->boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			tutorialOrb_SwordEnergy->widgetActor = this;
			//tutorialOrb_SwordEnergy->player = lobbyPlayer;
			tutorialOrb_SwordEnergy->audioComp->Deactivate();
			tutorialOrb_SwordEnergy->Tags.Add("SwordEnergyOrbs");
			tutorialOrbPool_SwordEnergy.Enqueue(tutorialOrb_SwordEnergy);
		}
	}

	currentStep = ETutorialStep::S1_RigtMove;
	FTimerHandle startHandle;
	GetWorldTimerManager().SetTimer(startHandle, FTimerDelegate::CreateLambda([&]()
		{
			//SwordSlash();
			ThumbRightMove();
			/*bGunSpawn = true;
			lobbyPlayer->rightHand->tutorialGunSpawn = true;
			lobbyPlayer->rightHand->tutorialSwordSpawn = true;
			lobbyPlayer->leftHand->tutorialSwordSpawn = true;
			lobbyPlayer->leftHand->tutorialGunSpawn = true;*/

		}), 2.f, false);
}

void ALobbyTutorialActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (currentStep == ETutorialStep::S1_RigtMove) {
		if (movableFrame1->TutorialCheckRightMove())
			ClearCurrentStep();
	}
	if (currentStep == ETutorialStep::S2_MiddleMove) {
		if (movableFrame1->TutorialCheckMiddleMove())
			ClearCurrentStep();
	}
	if (currentStep == ETutorialStep::S3_Rotate) {
		if (CheckRightRotate())
			ClearCurrentStep();
	}
	if (currentStep == ETutorialStep::S4_GunSpawn) {
		gunSpawnCount = lobbyPlayer->leftHand->tutorialGunSpawnCount + lobbyPlayer->rightHand->tutorialGunSpawnCount;
		lobbyWidget1->txt_count_1->SetText(FText::AsNumber(gunSpawnCount));
		if (gunSpawnCount >= 3)
			ClearCurrentStep();
	}
	if (currentStep == ETutorialStep::S5_GunAttack) {
		lobbyWidget1->txt_count_2->SetText(FText::AsNumber(orbDestroyedCount));
		GetSpawnedGun();
		if (gun1 != nullptr && gun1->bulletCount <= 0) {
			gun1->bulletCount = 6;
			gun1->SetWidget(gun1->bulletCount);
		}
		if (gun2 != nullptr && gun2->bulletCount <= 0) {
			gun2->bulletCount = 6;
			gun2->SetWidget(gun2->bulletCount);
		}
	}
	if (currentStep == ETutorialStep::S6_SwordSpawn) {
		swordSpawnCount = lobbyPlayer->leftHand->tutorialSwordSpawnCount + lobbyPlayer->rightHand->tutorialSwordSpawnCount;
		lobbyWidget1->txt_count_3->SetText(FText::AsNumber(swordSpawnCount));
		if (swordSpawnCount >= 3)
			ClearCurrentStep();
	}
	if (currentStep == ETutorialStep::S7_SwordAttack) {
		lobbyWidget1->txt_count_4->SetText(FText::AsNumber(orbDestroyedCount));
	}
	if (currentStep == ETutorialStep::S8_SwordGetEnergy) {
		lobbyWidget1->txt_count_5->SetText(FText::AsNumber(orbDestroyedCount));
	}
	if (currentStep == ETutorialStep::S9_GunReload) {
		GetSpawnedGun();
		if (gun1 != nullptr){
			gun1->bulletCount = 1;
			gun1->SetWidget(gun1->bulletCount);
			if (gun1->bInReload)
				ClearCurrentStep();
		}
		if (gun2 != nullptr) {
			gun2->bulletCount = 1;
			gun2->SetWidget(gun2->bulletCount);
			if (gun2->bInReload)
				ClearCurrentStep();
		}
	}
}

void ALobbyTutorialActor::ClearCurrentStep()
{
	if (clearSound != nullptr)
		UGameplayStatics::PlaySound2D(GetWorld(), clearSound);

	lobbyWidget1->PlayAnimationReverse(lobbyWidget1->canvasFade);
	if (lobbyWidget2->GetCanvasIndex() == 5 || lobbyWidget2->GetCanvasIndex() == 12) {
		widgetComp2->SetVisibility(false);
	}
	if (lobbyWidget3->GetCanvasIndex() == 6 || lobbyWidget3->GetCanvasIndex() == 13) {
		widgetComp3->SetVisibility(false);
	}

	StartNextStep();

}

void ALobbyTutorialActor::StartNextStep()
{
	switch (currentStep)
	{
	case ETutorialStep::S1_RigtMove:
		currentStep = ETutorialStep::S2_MiddleMove;
		ThumbLeftMove();
		break;
	case ETutorialStep::S2_MiddleMove:
		currentStep = ETutorialStep::S3_Rotate;
		ThumbRotate();
		break;
	case ETutorialStep::S3_Rotate:
		currentStep = ETutorialStep::S4_GunSpawn;
		GunSpawn();
		break;
	case ETutorialStep::S4_GunSpawn:
		currentStep = ETutorialStep::S5_GunAttack;
		GunAttack();
		break;
	case ETutorialStep::S5_GunAttack:
		currentStep = ETutorialStep::S6_SwordSpawn;
		SwordSpawn();
		break;
	case ETutorialStep::S6_SwordSpawn:
		currentStep = ETutorialStep::S7_SwordAttack;
		SwordAttack();
		break;
	case ETutorialStep::S7_SwordAttack:
		currentStep = ETutorialStep::S8_SwordGetEnergy;
		SwordGetEnergy();
		break;
	case ETutorialStep::S8_SwordGetEnergy:
		currentStep = ETutorialStep::S9_GunReload;
		GunReload();
		break;
	case ETutorialStep::S9_GunReload:
		currentStep = ETutorialStep::S10_SwordSlash;
		SwordSlash();
		break;
	case ETutorialStep::S10_SwordSlash:
		currentStep = ETutorialStep::S11_Default;
		break;
	default:
		break;
	}
}

void ALobbyTutorialActor::ThumbRightMove()
{
	lobbyWidget1->SwitchCanvas(0);
	lobbyWidget1->PlayAnimation(lobbyWidget1->canvasFade);
	lobbyWidget1->PlayAnimation(lobbyWidget1->imageAnim, 0.f, 30);
}

void ALobbyTutorialActor::ThumbLeftMove()
{
	lobbyWidget1->SwitchCanvas(1);
	lobbyWidget1->PlayAnimation(lobbyWidget1->canvasFade);
	lobbyWidget1->PlayAnimation(lobbyWidget1->imageAnim, 0.f, 30);
}

void ALobbyTutorialActor::ThumbRotate()
{
	lobbyWidget1->SwitchCanvas(2);
	lobbyWidget1->PlayAnimation(lobbyWidget1->canvasFade);
	lobbyWidget1->PlayAnimation(lobbyWidget1->imageAnim, 0.f, 30);
}

void ALobbyTutorialActor::GunSpawn()
{
	lobbyWidget1->SwitchCanvas(3);
	lobbyWidget1->PlayAnimation(lobbyWidget1->canvasFade);
	lobbyWidget1->PlayAnimation(lobbyWidget1->imageAnim, 0.f, 30);
	bGunSpawn = true;
	lobbyPlayer->rightHand->tutorialGunSpawn = true;
	lobbyPlayer->leftHand->tutorialGunSpawn = true;
}

void ALobbyTutorialActor::GunAttack()
{
	widgetComp2->SetVisibility(true);
	widgetComp3->SetVisibility(true);
	lobbyWidget1->SwitchCanvas(4);
	lobbyWidget1->PlayAnimation(lobbyWidget1->canvasFade);
	lobbyWidget2->SwitchCanvas(5);
	lobbyWidget2->PlayAnimation(lobbyWidget2->canvasFade);
	lobbyWidget2->PlayAnimation(lobbyWidget2->imageAnim, 0.f, 30);
	lobbyWidget3->SwitchCanvas(6);
	lobbyWidget3->PlayAnimation(lobbyWidget3->canvasFade);
	lobbyWidget3->PlayAnimation(lobbyWidget3->imageAnim, 0.f, 30);
	
	FVector orbLoc = FVector(-160, -5020, 200);
	for (int32 i = 0; i < 3; i++) {
		ALobbyTutorialOrb* orb = nullptr;
		tutorialOrbPool_Gun.Dequeue(orb);

		if (orb != nullptr) {
			orb->SetActorLocation(orbLoc);
			orb->OnOrbBegin();
			orb->SetOrbScale(0.7f);
		}
		orbLoc += FVector(280, 0, 0);
	}
}

void ALobbyTutorialActor::SwordSpawn()
{
	bSwordSpawn = true;
	lobbyPlayer->rightHand->tutorialSwordSpawn = true;
	lobbyPlayer->leftHand->tutorialSwordSpawn = true;
	lobbyWidget1->SwitchCanvas(7);
	lobbyWidget1->PlayAnimation(lobbyWidget1->canvasFade);
	lobbyWidget1->PlayAnimation(lobbyWidget1->imageAnim, 0.f, 30);
}

void ALobbyTutorialActor::SwordAttack()
{
	lobbyWidget1->SwitchCanvas(8);
	lobbyWidget1->PlayAnimation(lobbyWidget1->canvasFade);
	lobbyWidget1->PlayAnimation(lobbyWidget1->imageAnim, 0.f, 30);


	FTimerHandle swordOrbHandle;
	GetWorldTimerManager().SetTimer(swordOrbHandle, FTimerDelegate::CreateLambda([&]()
		{
		FVector orbLoc = FVector(30, -3900, 150);
		for (int32 i = 0; i<3; i++) {
			ALobbyTutorialOrb* orb = nullptr;
			tutorialOrbPool_Sword.Dequeue(orb);


			if (orb != nullptr) {
				orb->SetActorLocation(orbLoc);
				orb->OnOrbBegin();
				orb->SetOrbScale(0.2f);
			}
			orbLoc += FVector(90, 0, 0);
		}
	}), 1.5f, false);
}

void ALobbyTutorialActor::SwordGetEnergy()
{
	lobbyWidget1->SwitchCanvas(9);
	lobbyWidget1->PlayAnimation(lobbyWidget1->canvasFade);
	lobbyWidget1->PlayAnimation(lobbyWidget1->imageAnim, 0.f, 30);

	playerWidget = Cast<UBattlePlayerWidget>(lobbyPlayer->widgetManager->widgetRef);
	if (playerWidget != nullptr) {
		lobbyPlayer->bIsUIMode = false;
		playerWidget->SetTutorialWidget2();
		lobbyPlayer->UseEnergy(90);
	}

	FTimerHandle orbVisibleHandle;
	GetWorldTimerManager().SetTimer(orbVisibleHandle, FTimerDelegate::CreateLambda([&]()
		{
			FVector orbLoc = FVector(50, -3900, 150);
			for (int8 i = 0; i <= 1; i++) {
				ALobbyTutorialOrb* orb = nullptr;
				tutorialOrbPool_SwordEnergy.Dequeue(orb);

				if (orb != nullptr) {
					orb->SetActorLocation(orbLoc);
					orb->OnOrbBegin();
					orb->SetOrbScale(0.2f);
				}
				orbLoc += FVector(140, 0, 0);
			}
		}), 1.5f, false);
}

void ALobbyTutorialActor::GunReload()
{
	bGunReload = true;
	lobbyWidget1->SwitchCanvas(10);
	lobbyWidget1->PlayAnimation(lobbyWidget1->canvasFade);
	lobbyWidget1->PlayAnimation(lobbyWidget1->imageAnim, 0.f, 30);
}

void ALobbyTutorialActor::SwordSlash()
{
	lobbyWidget1->SwitchCanvas(11);
	lobbyWidget1->PlayAnimation(lobbyWidget1->canvasFade);
	widgetComp2->SetVisibility(true);
	lobbyWidget2->SwitchCanvas(12);
	lobbyWidget2->PlayAnimation(lobbyWidget2->canvasFade);
	lobbyWidget2->PlayAnimation(lobbyWidget2->imageAnim, 0.f, 30);
	widgetComp3->SetVisibility(true);
	lobbyWidget3->SwitchCanvas(13);
	lobbyWidget3->PlayAnimation(lobbyWidget3->canvasFade);
	lobbyWidget3->PlayAnimation(lobbyWidget3->imageAnim, 0.f, 30);

	ALobbyTutorialOrb* orb = nullptr;
	tutorialOrbPool_Gun.Dequeue(orb);
	if (orb != nullptr) {
		orb->SetActorLocation(FVector(120, -4845, 300));
		orb->OnOrbBegin();
		orb->SetOrbScale(1.f);
		//orb->boxComp->SetWorldScale3D(FVector(2));
		orb->Tags.Remove("GunOrbs");
		orb->Tags.Remove("SwordEnergyOrbs");
		orb->Tags.Add("SwordSlashOrbs");
	}

	bSwordSlash = true;
	lobbyPlayer->bIsUIMode = true;
}

bool ALobbyTutorialActor::CheckRightRotate()
{
	FRotator rot = lobbyPlayer->GetActorRotation();
	if (rot.Yaw >= 0 && rot.Yaw <= 180)
		return true;
	else
		return false;
}	

bool ALobbyTutorialActor::CheckLeftRotate()
{
	FRotator rot = lobbyPlayer->GetActorRotation();
	if (rot.Yaw <= -90)
		return true;
	else
		return false;
}

void ALobbyTutorialActor::SpawnStartOrb()
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform trans;
	trans.SetLocation(FVector(120, -6755, 680));
	trans.SetScale3D(FVector(2));


	if (bp_startOrb != nullptr)
		startOrb = GetWorld()->SpawnActor<AShootableUIActor>(bp_startOrb, trans, param);

}

void ALobbyTutorialActor::GetSpawnedGun()
{
	if (lobbyPlayer->rightHand->GetAttachedWeapon() != nullptr) {
		gun1 = Cast<AGun>(lobbyPlayer->rightHand->GetAttachedWeapon());
	}
	else
		gun1 = nullptr;

	if (lobbyPlayer->leftHand->GetAttachedWeapon() != nullptr) {
		gun2 = Cast<AGun>(lobbyPlayer->leftHand->GetAttachedWeapon());
	}
	else
		gun2 = nullptr;
}

void ALobbyTutorialActor::DebuggingWidget()
{
	if (lobbyWidget1->GetRenderOpacity() == 0)
		lobbyWidget1->SetRenderOpacity(1.f);
	if (lobbyWidget2->GetRenderOpacity() == 0)
		lobbyWidget2->SetRenderOpacity(1.f);
	if (lobbyWidget3->GetRenderOpacity() == 0)
		lobbyWidget3->SetRenderOpacity(1.f);
}

void ALobbyTutorialActor::DebuggingAllOrbs()
{
	if (currentStep == ETutorialStep::S6_SwordSpawn) {
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyTutorialOrb::StaticClass(), actors);
		for (AActor* gunOrb : actors) {
			ALobbyTutorialOrb* orb = Cast<ALobbyTutorialOrb>(gunOrb);
			if (orb != nullptr && orb->ActorHasTag("GunOrbs")) {
				orb->OnOrbDestroyed();
				orbDestroyedCount = 0;
				/*orb->niagaraComp->SetVisibility(false);
				orb->boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				orb->audioComp->Stop();
				SetActorLocation(FVector(0, 0, -1000));
				tutorialOrbPool_Gun.Enqueue(orb);*/
			}
		}
	}
	else if (currentStep == ETutorialStep::S8_SwordGetEnergy) {
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyTutorialOrb::StaticClass(), actors);
		for (AActor* swordOrb : actors) {
			ALobbyTutorialOrb* orb = Cast<ALobbyTutorialOrb>(swordOrb);
			if (orb != nullptr && orb->ActorHasTag("SwordOrbs")) {
				orb->OnOrbDestroyed();
				orbDestroyedCount = 0;
				/*orb->niagaraComp->SetVisibility(false);
				orb->boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				orb->audioComp->Stop();
				SetActorLocation(FVector(0, 0, -1000));
				tutorialOrbPool_Sword.Enqueue(orb);*/
			}
		}
	}
	else if (currentStep == ETutorialStep::S9_GunReload) {
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyTutorialOrb::StaticClass(), actors);
		for (AActor* swordOrb : actors) {
			ALobbyTutorialOrb* orb = Cast<ALobbyTutorialOrb>(swordOrb);
			if (orb != nullptr && orb->ActorHasTag("SwordEnergyOrbs")) {
				orb->OnOrbDestroyed();
				orbDestroyedCount = 0;
				/*orb->niagaraComp->SetVisibility(false);
				orb->boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				orb->audioComp->Stop();
				SetActorLocation(FVector(0, 0, -1000));
				tutorialOrbPool_Gun.Enqueue(orb);
				tutorialOrbPool_SwordEnergy.Enqueue(orb);*/
			}
		}
	}
	else if (currentStep == ETutorialStep::S11_Default) {
		
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyTutorialOrb::StaticClass(), actors);
		for (AActor* swordOrb : actors) {
			ALobbyTutorialOrb* orb = Cast<ALobbyTutorialOrb>(swordOrb);
			if (orb != nullptr && orb->ActorHasTag("SwordSlashOrbs")) {
				orb->OnOrbDestroyed();
				orbDestroyedCount = 0;
				/*orb->niagaraComp->SetVisibility(false);
				orb->boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				orb->audioComp->Stop();
				SetActorLocation(FVector(0, 0, -1000));
				tutorialOrbPool_Gun.Enqueue(orb);
				tutorialOrbPool_Sword.Enqueue(orb);*/
			}
		}
	}
}

void ALobbyTutorialActor::DebuggingButton()
{
	ClearCurrentStep();
}