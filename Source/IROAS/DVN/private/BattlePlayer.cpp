

#include "BattlePlayer.h"
#include "PlayerHand.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "BasicMovementComponent.h"
#include "SnapMovementComponent.h"
#include "PlayerHandAnim.h"
#include <Engine/Engine.h>
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BattlePlayerIKAnimComp.h"
#include "BattlePlayerWidgetManager.h"
#include "UMG/Public/Components/WidgetComponent.h"
#include <HeadMountedDisplay/Public/MotionControllerComponent.h>
#include "Sword.h"
#include "Gun.h"
#include "BattlePlayerIndicator.h"
#include "BattleGameStateBase.h"
#include "MovableFrame.h"
#include "LobbyGameMode.h"
#include "LobbyTutorialActor.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include "RageTriggerBox.h"
#include <LevelSequence/Public/LevelSequenceActor.h>


ABattlePlayer::ABattlePlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	//desiredEyeHeight = 200.f;
	characterMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	characterMeshComp->SetupAttachment(RootComponent);
	characterMeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	characterMeshComp->bOwnerNoSee = true;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempSkeletal(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/ParagonRevenant/Characters/Heroes/Revenant/Meshes/Revenant.Revenant'"));
	if (tempSkeletal.Succeeded()) {
		characterMeshComp->SetSkeletalMesh(tempSkeletal.Object);
	}

	desiredHalfHeight = (desiredEyeHeight + capsuleRadius) / 2.f;
	capsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	capsuleComp->SetupAttachment(RootComponent);
	capsuleComp->SetCapsuleRadius(capsuleRadius);
	capsuleComp->SetCapsuleHalfHeight(desiredHalfHeight);
	capsuleComp->SetRelativeLocation(FVector(0, 0, desiredHalfHeight));
	capsuleComp->SetCollisionProfileName(FName("PlayerBody"));

	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	widgetComp->SetupAttachment(camera);

	/*moveTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MoveTrail"));
	moveTrail->SetupAttachment(camera);
	moveTrail->SetRelativeLocation(FVector(60.f, 0.f, 0.f));
	moveTrail->SetVisibility(false);*/

	// 컴포넌트 패턴
	moveComp = CreateDefaultSubobject<UBasicMovementComponent>(TEXT("Move Component"));
	snapMoveComp = CreateDefaultSubobject<USnapMovementComponent>(TEXT("Snap Move Component"));
	ikAnimComp = CreateDefaultSubobject<UBattlePlayerIKAnimComp>(TEXT("IKAnim Component"));
	widgetManager = CreateDefaultSubobject<UBattlePlayerWidgetManager>(TEXT("WidgetManagerComponent"));

}

void ABattlePlayer::BeginPlay()
{
	Super::BeginPlay();

	debugModeExecuted = false;
	debugX = false;
	debugA = false;

	gsb = GetWorld()->GetGameState<ABattleGameStateBase>();
	if (gsb != nullptr) {
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Player Registered to GameState!")));
		gsb->player_1 = this;
	}

	// weaponPool
	if (bp_Sword != nullptr && bp_Gun != nullptr) {
		for (int32 i = 0; i < 3; i++) {
			FActorSpawnParameters Param;
			Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FVector spawnLoc = FVector(0.f, 0.f, -1000.f);
			FRotator spawnRot = GetActorRotation();

			ASword* sword = GetWorld()->SpawnActor<ASword>(bp_Sword, spawnLoc, spawnRot, Param);
			sword->owningPlayer = this;
			swordPool.Enqueue(sword);

			AGun* gun = GetWorld()->SpawnActor<AGun>(bp_Gun, spawnLoc, spawnRot, Param);
			gun->owningPlayer = this;
			gunPool.Enqueue(gun);
		}
	}
	InitializeHMD();
	InitializeVRHand();
	InitializeIndicator();
	InitializeLoadingCube();
	SetHealth(maxHealth);
	SetEnergy(maxEnergy);
	widgetManager->SetHealthPercent();
	widgetManager->SetEnergyPercent();
	widgetManager->bgsb = gsb;

	pc = Cast<APlayerController>(GetController());

	if (pc != nullptr)
	{
		//pc->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 5.0f, FLinearColor::Black);

		UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subSys != nullptr)
		{
			subSys->ClearAllMappings();
			subSys->AddMappingContext(inputMapping, 0);
		}
	}
	
	if (ActorHasTag("LobbyPlayer")) {
		lgm = GetWorld()->GetAuthGameMode<ALobbyGameMode>();
		if (lgm) {
			lgm->lobbyPlayer = this;
			widgetManager->SetTutorialWidget();
			bIsUIMode = true;
		}
	}
	
	if (!ActorHasTag("LobbyPlayer"))
	{
		
		rageBox = Cast<ARageTriggerBox>(UGameplayStatics::GetActorOfClass(GetWorld(), ARageTriggerBox::StaticClass()));
		if (rageBox)
		{
			startCineActor = rageBox->startCineActor;
			bossCineActor = rageBox->bossCineActor;
		}


		StartTransform = GetActorTransform();
		SetActorLocation(FVector(StartTransform.GetLocation().X, StartTransform.GetLocation().Y, -1000.f));

		if (startCineActor)
		{
			startCineActor->GetSequencePlayer()->OnFinished.AddDynamic(this, &ABattlePlayer::OnStartCineFinished);
		}
		
		
	}
	

}

void ABattlePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (debugX && debugA && !debugModeExecuted) {
		debugModeExecuted = true;
		if (ActorHasTag("LobbyPlayer")) {
			LobbyModeDebug();
		}
		else BattleModeDebug();
	}

	GEngine->AddOnScreenDebugMessage(3, 50.0f, FColor::Red, FString::Printf(TEXT("vrRoot: %.2f\t cam: %.2f"), vrRoot->GetComponentLocation().Z, camera->GetComponentLocation().Z));
	if (bIsDead) return;

	//camera->SetRelativeLocation(FMath::Lerp<FVector>(hmdPrevTrans.GetLocation(), camera->GetRelativeLocation(), 0.6));
	//camera->SetRelativeRotation(FMath::Lerp<FRotator>(hmdPrevTrans.GetRotation().Rotator(), camera->GetRelativeRotation(), 0.7));
	
	FVector capsuleLoc = camera->GetComponentLocation();
	capsuleLoc.Z -= capsuleRadius;
	capsuleComp->SetWorldLocation(capsuleLoc);

	// 왼손 보정
	leftHand->motionController->SetWorldLocation(FMath::VInterpTo(lPrevloc, leftHand->motionController->GetComponentLocation(), DeltaTime, 40.f));
	leftHand->motionController->SetWorldRotation(FQuat::Slerp(lPrevQuat, leftHand->motionController->GetComponentQuat(), 0.5));

	lPrevloc = leftHand->motionController->GetComponentLocation();
	lPrevQuat = leftHand->motionController->GetComponentQuat();

	// 오른손 보정
	rightHand->motionController->SetWorldLocation(FMath::VInterpTo(rPrevloc, rightHand->motionController->GetComponentLocation(), DeltaTime, 40.f));
	rightHand->motionController->SetWorldRotation(FQuat::Slerp(rPrevQuat, rightHand->motionController->GetComponentQuat(), 0.5));

	rPrevloc = rightHand->motionController->GetComponentLocation();
	rPrevQuat = rightHand->motionController->GetComponentQuat();
	
	
	float speed = FVector::Dist(GetActorLocation(), actorPrevLoc);
	GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Green, FString::Printf(TEXT("\nSpeed: %.2f\n"), speed));
	//if (speed > maxSpeed) moveTrail->SetVisibility(true);
	//if (speed < minSpeed) moveTrail->SetVisibility(false);
	actorPrevLoc = GetActorLocation();

	if (health < 1.0f && !bIsDead) {
		DieProcess();
	}


}

void ABattlePlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* eInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (eInputComponent != nullptr) {

		//moveComp->SetupPlayerInputComponent(eInputComponent, inputActions);
		//snapMoveComp->SetupPlayerInputComponent(eInputComponent, inputActions);

		eInputComponent->BindAction(inputActions[0], ETriggerEvent::Triggered, this, &ABattlePlayer::HandleRightThumbstickAxis);
		eInputComponent->BindAction(inputActions[0], ETriggerEvent::Completed, this, &ABattlePlayer::HandleRightThumbstickAxis);

		// Grips
		eInputComponent->BindAction(inputActions[2], ETriggerEvent::Started, this, &ABattlePlayer::HandleLeftGripPressed);
		eInputComponent->BindAction(inputActions[2], ETriggerEvent::Completed, this, &ABattlePlayer::HandleLeftGripReleased);
		eInputComponent->BindAction(inputActions[3], ETriggerEvent::Started, this, &ABattlePlayer::HandleRightGripPressed);
		eInputComponent->BindAction(inputActions[3], ETriggerEvent::Completed, this, &ABattlePlayer::HandleRightGripReleased);

		// Triggers
		eInputComponent->BindAction(inputActions[4], ETriggerEvent::Started, this, &ABattlePlayer::HandleLeftTriggerPressed);
		eInputComponent->BindAction(inputActions[4], ETriggerEvent::Triggered, this, &ABattlePlayer::HandleLeftTriggerPressed);
		eInputComponent->BindAction(inputActions[4], ETriggerEvent::Completed, this, &ABattlePlayer::HandleLeftTriggerReleased);
		eInputComponent->BindAction(inputActions[5], ETriggerEvent::Started, this, &ABattlePlayer::HandleRightTriggerPressed);
		eInputComponent->BindAction(inputActions[5], ETriggerEvent::Triggered, this, &ABattlePlayer::HandleRightTriggerPressed);
		eInputComponent->BindAction(inputActions[5], ETriggerEvent::Completed, this, &ABattlePlayer::HandleRightTriggerReleased);
		eInputComponent->BindAction(inputActions[6], ETriggerEvent::Started, this, &ABattlePlayer::HandleLeftThumbstickLeft);
		eInputComponent->BindAction(inputActions[7], ETriggerEvent::Started, this, &ABattlePlayer::HandleLeftThumbstickRight);
		// Test
		eInputComponent->BindAction(inputActions[9], ETriggerEvent::Completed, this, &ABattlePlayer::HandleRightThumbstickRight);
		eInputComponent->BindAction(inputActions[10], ETriggerEvent::Started, this, &ABattlePlayer::HandleRightButtonA);
		eInputComponent->BindAction(inputActions[10], ETriggerEvent::Completed, this, &ABattlePlayer::HandleRightButtonACompleted);

		eInputComponent->BindAction(inputActions[11], ETriggerEvent::Started, this, &ABattlePlayer::HandleLeftButtonX);
		eInputComponent->BindAction(inputActions[11], ETriggerEvent::Completed, this, &ABattlePlayer::HandleLeftButtonXCompleted);
	}
}


void ABattlePlayer::InitializeVRHand()
{
	//Super::InitializeVRHand();

	if (vrHandClass == nullptr)
		return;

	FTransform trans;
	trans.Identity;

	FAttachmentTransformRules attachrule(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	//왼손
	leftHand = GetWorld()->SpawnActorDeferred<APlayerHand>(vrHandClass, trans, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (nullptr != leftHand)
	{
		leftHand->SetVRHandIsRight(false);
	}
	leftHand->FinishSpawning(trans);
	leftHand->AttachToComponent(vrRoot, attachrule);
	leftHand->player = this;

	//오른손
	rightHand = GetWorld()->SpawnActorDeferred<APlayerHand>(vrHandClass, trans, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (nullptr != rightHand)
	{
		rightHand->SetVRHandIsRight(true);
	}
	rightHand->FinishSpawning(trans);
	rightHand->AttachToComponent(vrRoot, attachrule);
	rightHand->player = this;
}

void ABattlePlayer::InitializeIndicator()
{
	FTransform trans;
	trans.Identity;

	FAttachmentTransformRules attachrule(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	if (widgetManager->bp_Indicator != nullptr) {
		widgetManager->indicatorRef = GetWorld()->SpawnActor<ABattlePlayerIndicator>(widgetManager->bp_Indicator, trans);
		widgetManager->indicatorRef->SetActorRotation(GetActorRotation() + FRotator(0, -90, 0));
		widgetManager->indicatorRef->AttachToComponent(camera, attachrule);
		snapMoveComp->moveableFrame_1->playerIndicator = widgetManager->indicatorRef;
		snapMoveComp->moveableFrame_2->playerIndicator = widgetManager->indicatorRef;
	}

	
}


void ABattlePlayer::InitializeLoadingCube()
{
	if (ActorHasTag("LobbyPlayer")) return;

	FTransform trans;
	trans.Identity;

	FAttachmentTransformRules attachrule(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	if (widgetManager->bp_LoadingCube != nullptr) {
		widgetManager->loadingCubeRef = GetWorld()->SpawnActor<AActor>(widgetManager->bp_LoadingCube, trans);
		widgetManager->loadingCubeRef->AttachToComponent(camera, attachrule);
	}
	// Loading Cube 띄우기
	widgetManager->loadingCubeRef->SetActorHiddenInGame(false);
	widgetManager->SetFirstLodingCube();
	widgetManager->SwitchCanvas(1);
	
}

void ABattlePlayer::ReloadAllGuns()
{

	AGun* gun = nullptr;
	TQueue<AGun*> tempGunPool;
	while (gunPool.IsEmpty()) {
		gun = nullptr;
		gunPool.Dequeue(gun);
		gun->Reload();
		tempGunPool.Enqueue(gun);
	}
	while (!tempGunPool.IsEmpty()) {
		gun = nullptr;
		tempGunPool.Dequeue(gun);
		gunPool.Enqueue(gun);
	}

	if (leftHand->GetAttachedWeapon() != nullptr) {
		if (leftHand->GetAttachedWeapon()->weaponType == EWeaponType::Gun) {
			AGun* tempLeftGun = Cast<AGun>(leftHand->attachedActor);
			tempLeftGun->Reload();
		}
	}

	if (rightHand->GetAttachedWeapon() != nullptr) {
		if (rightHand->GetAttachedWeapon()->weaponType == EWeaponType::Gun) {
			AGun* tempRightGun = Cast<AGun>(rightHand->attachedActor);
			tempRightGun->Reload();
		}
	}
	
}

void ABattlePlayer::HandleLeftGripPressed()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("LEFT GripPress!")));

	// 튜토리얼 위한 것
	if (lgm != nullptr && lgm->tutorialActor!= nullptr && !lgm->tutorialActor->bGunSpawn) return;

	if (leftHand != nullptr) {
		leftHand->OnGripPressed();
		leftHand->handAnim->PoseAlphaGrasp = 1.0f;
	}
}

void ABattlePlayer::HandleLeftGripReleased()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("LEFT GripRelease!")));
	if (leftHand != nullptr) {
		leftHand->OnGripReleased();
		leftHand->handAnim->PoseAlphaGrasp = 0.0f;
	}
}

void ABattlePlayer::HandleLeftTriggerPressed()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("LEFT TriggerPress!")));
	if (leftHand != nullptr) {
		leftHand->OnTriggerPressed();
		leftHand->handAnim->PoseAlphaIndexCurl = 1.0f;
	}
}

void ABattlePlayer::HandleLeftTriggerReleased()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("LEFT TriggerRelease!")));
	if (leftHand != nullptr) {
		leftHand->OnTriggerReleased();
		leftHand->handAnim->PoseAlphaIndexCurl = 0.0f;
	}
}

void ABattlePlayer::HandleLeftThumbstickAxis(const FInputActionValue& Value)
{

}

void ABattlePlayer::HandleLeftThumbstickLeft()
{
	if (snapMoveComp != nullptr && bCanMove) {
		snapMoveComp->MoveLeft();
		//widgetManager->IndicateCall(EIndicatorType::LeftMove);
	}
}

void ABattlePlayer::HandleLeftThumbstickRight()
{
	if (snapMoveComp != nullptr && bCanMove) {
		snapMoveComp->MoveRight();
		//widgetManager->IndicateCall(EIndicatorType::RightMove);
	}
}

void ABattlePlayer::HandleLeftMenu()
{

}

void ABattlePlayer::HandleRightGripPressed()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("RightGripPress!")));
	if (rightHand != nullptr) {
		rightHand->OnGripPressed();
		rightHand->handAnim->PoseAlphaGrasp = 1.0f;
	}
}

void ABattlePlayer::HandleRightGripReleased()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("RightGripRelease!")));
	if (rightHand != nullptr) {
		rightHand->OnGripReleased();
		rightHand->handAnim->PoseAlphaGrasp = 0.0f;
	}
}

void ABattlePlayer::HandleRightTriggerPressed()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("RightTriggerPress!")));
	if (rightHand != nullptr) {
		rightHand->OnTriggerPressed();
		rightHand->handAnim->PoseAlphaIndexCurl = 1.0f;
	}
}

void ABattlePlayer::HandleRightTriggerReleased()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("RightTriggerRelease!")));
	if (rightHand != nullptr) {
		rightHand->OnTriggerReleased();
		rightHand->handAnim->PoseAlphaIndexCurl = 0.0f;
	}
}

void ABattlePlayer::HandleRightThumbstickAxis(const FInputActionValue& Value)
{
	if (moveComp != nullptr) {
		moveComp->Rotate(Value);
	}
}

void ABattlePlayer::HandleRightThumbstickRight()
{
	//if (httpRequestActor != nullptr)
	//{
	//	//httpRequestActor->PostResultRequest("http://192.168.1.142:8001/api/v1/player/add", "");
	//}
}

void ABattlePlayer::HandleRightButtonA()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("RightButton A Pressed!")));
	//로비맵으로 나가기
	//UGameplayStatics::OpenLevel(GetWorld(), FName("Proto_LobbyLevel"));
	debugA = true;
}

void ABattlePlayer::HandleRightButtonACompleted()
{
	debugA = false;
}

void ABattlePlayer::HandleLeftButtonX()
{
	debugX = true;
}

void ABattlePlayer::HandleLeftButtonXCompleted()
{
	debugX = false;
}

void ABattlePlayer::BattleModeDebug() //skip
{
	
	if (rageBox)
	{

		if (startCineActor->GetSequencePlayer()->IsPlaying())
		{
			startCineActor->GetSequencePlayer()->GoToEndAndStop();
			SetActorLocation(FVector(StartTransform.GetLocation().X, StartTransform.GetLocation().Y, StartTransform.GetLocation().Z));
		}

		if (bossCineActor->GetSequencePlayer()->IsPlaying())
		{
			bossCineActor->GetSequencePlayer()->GoToEndAndStop();
		}

	}	

}

void ABattlePlayer::LobbyModeDebug()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("Beta_StoryLevel"));
}

FVector ABattlePlayer::GetCameraLoc()
{
	return camera->GetComponentLocation();
}

void ABattlePlayer::OnDamage_Interface(float DamageAmount)
{
	OnDamage(DamageAmount);
}

void ABattlePlayer::OnDamage(float deal)
{
	if (bIsDead) return;
	health = FMath::Max(0, health - deal);
	widgetManager->IndicateCall(EIndicatorType::Damage);
	widgetManager->SetHealthPercent();
}

void ABattlePlayer::OnHeal(float heal)
{
	if (bIsDead) return;

	health = FMath::Min(maxHealth, health + heal);
	widgetManager->IndicateCall(EIndicatorType::HealthHeal);
	widgetManager->SetHealthPercent();
}

bool ABattlePlayer::UseEnergy(float energyCost)
{
	if (bIsUIMode) return true;
	if (bIsDead || energy < energyCost) return false;
	else {
		energy -= energyCost;
		widgetManager->SetEnergyPercent();
		return true;
	}
}

void ABattlePlayer::GainEnergy(float amount)
{
	if (bIsDead) return;

	energy = FMath::Min(maxEnergy, energy + amount);
	if (gainEnergySound != nullptr)
		UGameplayStatics::PlaySound2D(GetWorld(), gainEnergySound);
	widgetManager->IndicateCall(EIndicatorType::EnergyHeal);
	widgetManager->SetEnergyPercent();
}

void ABattlePlayer::DieProcess()
{
	bIsDead = true;
	UGameplayStatics::PlaySound2D(GetWorld(), deadSound);
	widgetManager->DieIndicate();
	gsb->OnGameFinished();
}

void ABattlePlayer::HitbyMeleeSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), hitbyMeleeSound);
}

void ABattlePlayer::HitbyRangedSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), hitbyRangedSound);
}

void ABattlePlayer::HitbyBossSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), hitbyBossSound);
}

void ABattlePlayer::OnStartCineFinished()
{
	SetActorLocation(FVector(StartTransform.GetLocation().X, StartTransform.GetLocation().Y, StartTransform.GetLocation().Z));
}

