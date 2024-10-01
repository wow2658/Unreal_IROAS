// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootableUIActor.h"
#include <Components/BoxComponent.h>
#include <Components/TextRenderComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "EnergyBullet.h"
#include <Curves/CurveVector.h>
#include "SlashProjectile.h"
#include "BattlePlayer.h"
#include <UMG/Public/Components/SizeBox.h>
#include "BattlePlayerWidgetManager.h"
#include "BattlePlayerWidget.h"
#include "Sword.h"
#include "Gun.h"
#include "PlayerHand.h"
#include <UMG/Public/Components/TextBlock.h>
#include "BattleResultActor.h"
#include <UMG/Public/Components/WidgetComponent.h>
#include <Components/AudioComponent.h>
#include <Sound/AmbientSound.h>
#include "LobbyTutorialWidget.h"

AShootableUIActor::AShootableUIActor()
{
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(rootComp);
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Button"));
	boxComp->SetupAttachment(RootComponent);
	boxComp->SetBoxExtent(FVector(130));
	boxComp->SetCollisionProfileName(FName("EnemyBody"));
	niagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraFX"));
	niagaraComp->SetupAttachment(boxComp);
	niagaraComp->bAutoActivate = false;
	textRenderComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComp"));
	textRenderComp->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	textRenderComp->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	textRenderComp->SetupAttachment(niagaraComp);
	textRenderComp->SetVisibility(true);
	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	widgetComp->SetupAttachment(boxComp);
	widgetComp->SetVisibility(false);
	widgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	audioComp->SetupAttachment(boxComp);
	
	floatingCurve1 = nullptr;
	floatingTimeLineLength = 3.0f;
}

void AShootableUIActor::BeginPlay()
{
	Super::BeginPlay();
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AShootableUIActor::OnOrbCrashed);

	player = Cast<ABattlePlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	pc = Cast<APlayerController>(player->GetController());


	if (ActorHasTag("Reward")) {
		SetRewardText();
		niagaraComp->OnComponentActivated.AddDynamic(this, &AShootableUIActor::OnTextAppear);
		if (audioComp->IsPlaying())
			audioComp->Stop();
	}
	else if (ActorHasTag("LobbyStart")) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), appearSound, GetActorLocation());
		audioComp->FadeIn(0.5f, 0.5f);

		// LogoMesh 스폰
		logoMesh = nullptr;
		if (bp_LogoMesh != nullptr) {
			FActorSpawnParameters Param;
			Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FTransform trans;
			trans.SetLocation(FVector(-2025, 323, 400));
			trans.SetScale3D(FVector(2));

			logoMesh = GetWorld()->SpawnActor<AActor>(bp_LogoMesh, trans);
			logoMesh->SetActorHiddenInGame(true);
		}
	}

	// floating Curve
	CurveSetting();
	floatingStartTime = FMath::FRandRange(0.5f, 1.2f);

	// textrenderComp Curve
	FOnTimelineFloat OnTextAppear;
	OnTextAppear.BindUFunction(this, FName("TextAppearProcess"));

	textAppearTimeline.AddInterpFloat(textAppearCurve, OnTextAppear);
	textAppearTimeline.SetTimelineLength(textAppearTime);

	textAppearTimeline.PlayFromStart();
}

void AShootableUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	curtime += DeltaTime;
	if (curtime >= floatingStartTime)
		floatingTimeLine.TickTimeline(DeltaTime);

	textAppearTimeline.TickTimeline(DeltaTime);
}

void AShootableUIActor::OnOrbCrashed(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnergyBullet* bullet = Cast<AEnergyBullet>(OtherActor);
	if (bullet != nullptr) {
		if (ActorHasTag("LobbyStart"))
			OnLobbyStartOrbShot();
		else if (ActorHasTag("Reward"))
			OnGettingReward();
		else if (ActorHasTag("StartTrigger"))
			OnStartTriggerShot(bullet);
		else
			OnBasicOrbShot();
	}

	ASlashProjectile* swordSlash = Cast<ASlashProjectile>(OtherActor);
	if (swordSlash != nullptr) {
		if (ActorHasTag("StartTrigger"))
			OnStartTriggerSlashed();
	}
}

void AShootableUIActor::OnBasicOrbShot()
{
	niagaraComp->SetVisibility(false);
	textRenderComp->SetVisibility(false);
	audioComp->Stop();
	boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (selectFX != nullptr) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), selectFX, GetActorLocation(), GetActorRotation(), GetActorScale3D() * 1.3f);
	}
	if (selectSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), selectSound, GetActorLocation());
	}

	OnFinishedOrbShot();
}

void AShootableUIActor::OnLobbyStartOrbShot()
{
	if (player->ActorHasTag("LobbyPlayer")) {
		player->widgetComp->SetVisibility(false);
		player->rightHand->tutorialGunSpawn = false;
		player->rightHand->tutorialSwordSpawn = false;
		player->leftHand->tutorialGunSpawn = false;
		player->leftHand->tutorialSwordSpawn = false;
		player->bCanMove = false;
	}

	niagaraComp->SetVisibility(false);
	textRenderComp->SetVisibility(false);
	boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (selectFX != nullptr) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), selectFX, GetActorLocation(), GetActorRotation(), GetActorScale3D() * 1.3f);
	}
	if (selectSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), selectSound, GetActorLocation());
	}
	
	// 검은 배경 공간으로 이동
	pc->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 1.4f, FLinearColor::Black, false, true);

	FTimerHandle introDelay1;
	GetWorldTimerManager().SetTimer(introDelay1, FTimerDelegate::CreateLambda([&]() {
		player->SetActorLocation(FVector(-2025, 1183, 91));
		SetActorLocation(FVector(-2025, 323, 400), false, nullptr, ETeleportType::TeleportPhysics);
		SetActorScale3D(FVector(0.8f));
		pc->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 2.0f, FLinearColor::Black, false, true);
		FTimerHandle introDelay2;
		GetWorldTimerManager().SetTimer(introDelay2, FTimerDelegate::CreateLambda([&]() {
			UGameplayStatics::SpawnSound2D(GetWorld(), introSound);
			widgetComp->SetVisibility(true);
			ULobbyTutorialWidget* lobbyWidget = Cast<ULobbyTutorialWidget>(widgetComp->GetWidget());
			if (lobbyWidget != nullptr) {
				lobbyWidget->SwitchCanvas(15);
				lobbyWidget->PlayAnimation(lobbyWidget->textFade);
				floatingTimeLine.Stop();
				/*GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("LOBBY widget")));*/
			}
			UGameplayStatics::PlaySound2D(GetWorld(), narrationSound, 10.f);
			audioComp->SetVolumeMultiplier(2.f);
			}), 2.0f, false);
		}), 1.5f, false);
	
	FTimerHandle animDelay;
	GetWorldTimerManager().SetTimer(animDelay, FTimerDelegate::CreateLambda([&]()
		{
			widgetComp->SetVisibility(false);
			if (logoMesh)
			{
				logoMesh->SetActorHiddenInGame(false);
			}
			
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("%d"), logoMesh->IsHidden()));

			audioComp->SetVolumeMultiplier(4.f);
			//pc->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 1.0f, FLinearColor::Black);
	}), 23.5f, false);

	//logoMesh->SetActorLocation(GetActorLocation());
	

	/*FTimerHandle fadeDelay;
	GetWorldTimerManager().SetTimer(fadeDelay, FTimerDelegate::CreateLambda([&]() {
		
		
		pc->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 3.0f, FLinearColor::Black);	
	}), 24.5f, false);*/

	FTimerHandle moveDelay;
	GetWorldTimerManager().SetTimer(moveDelay, FTimerDelegate::CreateLambda([&]() {
		
		pc->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 1.2f, FLinearColor::Black, true, true);
	}), 26.f, false);

	FTimerHandle levelDelay;
	GetWorldTimerManager().SetTimer(levelDelay, FTimerDelegate::CreateLambda([&]() {
		UGameplayStatics::OpenLevel(GetWorld(), levelName);
	}), 27.5f, false);
}

void AShootableUIActor::OnStartTriggerSlashed()
{
	niagaraComp->SetVisibility(false);
	textRenderComp->SetVisibility(false);
	audioComp->Stop();
	boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (selectFX != nullptr) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), selectFX, GetActorLocation(), GetActorRotation(), FVector(30.f), true, true);
	}
	if (selectSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), selectSound, player->GetActorLocation() + FVector(200, 0, 300), 16.f, 0.7f);
	}
	Destroy();
}

void AShootableUIActor::OnStartTriggerShot(AEnergyBullet* bullet)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), bulletClashSound, GetActorLocation());
	FVector inversedDir = bullet->GetActorForwardVector() * (-1);
	bullet->SetActorRotation(inversedDir.Rotation());
}

void AShootableUIActor::SetRewardText()
{
	if (ActorHasTag(FName("Attack")))
		textRenderComp->SetText(FText::FromString(FString::Printf(TEXT("Attack +%.0f"), attackRewardValue)));
	else if (ActorHasTag(FName("HP")))
		textRenderComp->SetText(FText::FromString(FString::Printf(TEXT("Max HP +%.0f"), healthRewardValue)));
	else if (ActorHasTag(FName("Energy")))
		textRenderComp->SetText(FText::FromString(FString::Printf(TEXT("Max Energy +%.0f"), energyRewardValue)));
}

void AShootableUIActor::OnOrbActivate()
{
	niagaraComp->Activate(true);
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	audioComp->FadeIn(0.5f);
}

void AShootableUIActor::OnGettingReward()
{
	niagaraComp->SetVisibility(false);
	textRenderComp->SetVisibility(false);
	audioComp->Stop();
	boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (selectFX != nullptr) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), selectFX, GetActorLocation(), GetActorRotation(), GetActorScale3D() * 1.3f);
	}
	if (selectSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), selectSound, GetActorLocation());
	}

	if (ActorHasTag(FName("Attack"))) {
		ASword* sword = nullptr;
		TQueue<ASword*> tempSwordPool;
		while (!player->swordPool.IsEmpty()) {
			sword = nullptr;
			player->swordPool.Dequeue(sword);
			sword->normalAttackDamage += attackRewardValue;
			sword->slashDamage += attackRewardValue;
			tempSwordPool.Enqueue(sword);
		}
		while (!tempSwordPool.IsEmpty()) {
			sword = nullptr;
			tempSwordPool.Dequeue(sword);
			player->swordPool.Enqueue(sword);
		}
	
		AGun* gun = nullptr;
		TQueue<AGun*> tempGunPool;
		while (!player->gunPool.IsEmpty()) {
			gun = nullptr;
			player->gunPool.Dequeue(gun);
			gun->gunDamage += attackRewardValue;
			gun->IncreaseBulletDamage(attackRewardValue);
			tempGunPool.Enqueue(gun);
		}
		while (!tempGunPool.IsEmpty()) {
			gun = nullptr;
			tempGunPool.Dequeue(gun);
			player->gunPool.Enqueue(gun);
		}

		// 손에 들린 무기 공격력 높여주기
		GetHandedWeaponReward();
	}
	else if (ActorHasTag(FName("HP"))) {
		player->maxHealth += healthRewardValue;
		player->widgetManager->widgetRef->txt_MaxHealthCount->SetText(FText::AsNumber(player->maxHealth));
		player->OnHeal(healthRewardValue);
	}
	else if (ActorHasTag(FName("Energy"))) {
		player->maxEnergy += energyRewardValue;
		player->widgetManager->widgetRef->txt_MaxEnergyCount->SetText(FText::AsNumber(player->maxEnergy));
		player->GainEnergy(energyRewardValue);
	}

	rewardActor->Destroy();
}

void AShootableUIActor::GetHandedWeaponReward()
{
	if (player->leftHand->GetAttachedWeapon() != nullptr) {
		if (player->leftHand->GetAttachedWeapon()->weaponType == EWeaponType::Sword) {
			ASword* tempLeftSword = Cast<ASword>(player->leftHand->attachedActor);
			tempLeftSword->normalAttackDamage += attackRewardValue;
			tempLeftSword->slashDamage += attackRewardValue;
		}
		else if (player->leftHand->GetAttachedWeapon()->weaponType == EWeaponType::Gun) {
			AGun* tempLeftGun = Cast<AGun>(player->leftHand->attachedActor);
			tempLeftGun->gunDamage += attackRewardValue;
			tempLeftGun->IncreaseBulletDamage(attackRewardValue);
		}
	}
	
	if (player->rightHand->GetAttachedWeapon() != nullptr) {
		if (player->rightHand->GetAttachedWeapon()->weaponType == EWeaponType::Sword) {
			ASword* tempRightSword = Cast<ASword>(player->rightHand->attachedActor);
			tempRightSword->normalAttackDamage += attackRewardValue;
			tempRightSword->slashDamage += attackRewardValue;
		}
		else if (player->rightHand->GetAttachedWeapon()->weaponType == EWeaponType::Gun) {
			AGun* tempRightGun = Cast<AGun>(player->rightHand->attachedActor);
			tempRightGun->gunDamage += attackRewardValue;
			tempRightGun->IncreaseBulletDamage(attackRewardValue);
		}
	}
}

void AShootableUIActor::CurveSetting()
{
	if (floatingCurve1 != nullptr) {
		// VectorCurve의 Z축에 대한 키 값 랜덤 변경
	/*	floatingCurve1->FloatCurves[2].Keys[1].Value = FMath::RandRange(0.5f, 0.9f);
		floatingCurve1->FloatCurves[2].Keys[2].Value = FMath::RandRange(-0.9f, -0.5f);*/

		FOnTimelineVector onProgressFloating;
		onProgressFloating.BindUFunction(this, FName("FloatingCurveStart"));
		FOnTimelineEvent onFinishedFloating;
		onFinishedFloating.BindUFunction(this, FName("FloatingCurveEnd"));


		floatingTimeLine.AddInterpVector(floatingCurve1, onProgressFloating);
		floatingTimeLine.SetTimelineFinishedFunc(onFinishedFloating);
		floatingTimeLine.SetTimelineLength(floatingTimeLineLength);
		floatingTimeLine.SetLooping(true);
	}
	floatingTimeLine.PlayFromStart();
}

void AShootableUIActor::FloatingCurveStart(FVector value)
{
	boxComp->AddLocalOffset(value);
}

void AShootableUIActor::FloatingCurveEnd()
{
	bInFloating = false;
}

void AShootableUIActor::OnTextAppear(UActorComponent* Component, bool bReset)
{
	if (textAppearCurve != nullptr)
		textAppearTimeline.PlayFromStart();
}

void AShootableUIActor::TextAppearProcess(float alpha)
{
	//alpha = FMath::Clamp(alpha, 0, 1);
	if (textRenderComp != nullptr) {
		textRenderComp->SetRelativeScale3D(FVector(FMath::Lerp(0.f, 1.f, alpha)));
		/*GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("%.2f"), alpha));*/
	}
}

