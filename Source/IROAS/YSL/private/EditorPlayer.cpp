// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorPlayer.h"
#include "EditorHand.h"
#include <Components/ArrowComponent.h>
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EditorZoomSpline.h"
#include <Components/SplineMeshComponent.h>
#include <Camera/CameraComponent.h>
#include <Components/TextRenderComponent.h>
#include <UMG/Public/Components/WidgetInteractionComponent.h>
#include <Kismet/GameplayStatics.h>
#include "EditorZoomSpline.h"
#include "EditableActor.h"
#include <Components/SphereComponent.h>
#include "EditableSpawner.h"
#include "EditableProp.h"
#include "EditMenuWidgetActor.h"
#include "EditMenuWidget.h"
#include "EditGameStateBase.h"
#include <Materials/MaterialInstanceDynamic.h>
#include <Materials/MaterialInterface.h>
#include <Engine/PostProcessVolume.h>
#include <HeadMountedDisplay/Public/MotionControllerComponent.h>

AEditorPlayer::AEditorPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	textRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("test text"));
	textRender->SetupAttachment(camera);

	//moveComp = CreateDefaultSubobject<UBasicMovementComponent>(TEXT("Move Component"));
}

void AEditorPlayer::BeginPlay()
{
	Super::BeginPlay();

	InitializeVRHand();
	GetZoomSplines();

	gameState = Cast<AEditGameStateBase>(GetWorld()->GetGameState());

	pc = Cast<APlayerController>(GetController());
	if (pc != nullptr)
	{
		UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subSys != nullptr)
		{
			subSys->ClearAllMappings();
			subSys->AddMappingContext(inputMapping, 0);
		}
	}

	TArray<AActor*> actors;
	GetController()->GetOverlappingActors(actors, APostProcessVolume::StaticClass());
	if (actors.Num() > 0) {
		APostProcessVolume* postProcessVolume = Cast<APostProcessVolume>(actors[0]);
		matInstance = Cast<UMaterialInstanceDynamic>(postProcessVolume->Settings.WeightedBlendables.Array[0].Object);
		if (matInstance != nullptr)
			matInstance->SetScalarParameterValue(FName("StencilValue"), 1.0f);
	}
		
	//UMaterialInterface* postProcessMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/YSL/Mat/M_Select"));
	//if (postProcessMaterial) {
	//	matInstance = UMaterialInstanceDynamic::Create(postProcessMaterial, nullptr);
	//	if (matInstance) {
	//		/*FName paramName = FName(TEXT("StencilValue"));
	//		float paramValue;
	//		if (matInstance->GetScalarParameterValue(paramName, paramValue))
	//			stencilValue = paramValue;*/
	//		APostProcessVolume* postProcessVolume = nullptr;

	//		TArray<AActor*> actors;
	//		GetController()->GetOverlappingActors(actors, APostProcessVolume::StaticClass());
	//		if (actors.Num() > 0) {
	//			postProcessVolume = Cast<APostProcessVolume>(actors[0]);
	//			if (postProcessVolume) {
	//				FWeightedBlendable blendable;
	//				blendable.Object = matInstance;
	//				blendable.Weight = 0.1f;
	//				postProcessVolume->Settings.WeightedBlendables.Array.Add(blendable);
	//			}
	//		}
	//	}
	//}
}

void AEditorPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 왼손 보정
	leftHand->motionController->SetRelativeLocation(FMath::Lerp<FVector>(lHandPrevTrans.GetLocation(), leftHand->motionController->GetRelativeLocation(), 0.7));
	leftHand->motionController->SetRelativeRotation(FMath::Lerp<FRotator>(lHandPrevTrans.GetRotation().Rotator(), leftHand->motionController->GetRelativeRotation(), 0.7));
	lHandPrevTrans = leftHand->motionController->GetRelativeTransform();
	// 오른손 보정
	rightHand->motionController->SetRelativeLocation(FMath::Lerp<FVector>(rHandPrevTrans.GetLocation(), rightHand->motionController->GetRelativeLocation(), 0.7));
	rightHand->motionController->SetRelativeRotation(FMath::Lerp<FRotator>(rHandPrevTrans.GetRotation().Rotator(), rightHand->motionController->GetRelativeRotation(), 0.7));
	rHandPrevTrans = rightHand->motionController->GetRelativeTransform();
	if(bLeftGripped && bRightGripped)
		ZoomIn();

	textRender->SetText(FText::FromString(FString::Printf(TEXT("%d, %d"), currentCameraIndex, prevCameraIndex)));
}

void AEditorPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* eInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	if (eInputComponent != nullptr) {
		eInputComponent->BindAction(inputActions[0], ETriggerEvent::Started, this, &AEditorPlayer::HandleLeftThumbstickLeft);
		eInputComponent->BindAction(inputActions[1], ETriggerEvent::Started, this, &AEditorPlayer::HandleLeftThumbstickRight);
		eInputComponent->BindAction(inputActions[2], ETriggerEvent::Started, this, &AEditorPlayer::HandleLeftGripPressed);
		eInputComponent->BindAction(inputActions[2], ETriggerEvent::Completed, this, &AEditorPlayer::HandleLeftGripReleased); 
		eInputComponent->BindAction(inputActions[3], ETriggerEvent::Triggered, this, &AEditorPlayer::HandleRightGripPressed);
		eInputComponent->BindAction(inputActions[3], ETriggerEvent::Completed, this, &AEditorPlayer::HandleRightGripReleased);
		eInputComponent->BindAction(inputActions[4], ETriggerEvent::Started, this, &AEditorPlayer::HandleRightTriggerPressed);
		eInputComponent->BindAction(inputActions[4], ETriggerEvent::Completed, this, &AEditorPlayer::HandleRightTriggerReleased);
		eInputComponent->BindAction(inputActions[5], ETriggerEvent::Triggered, this, &AEditorPlayer::HandleRightThumbstickAxis);
		eInputComponent->BindAction(inputActions[5], ETriggerEvent::Completed, this, &AEditorPlayer::HandleRightThumbstickAxis);
		eInputComponent->BindAction(inputActions[6], ETriggerEvent::Started, this, &AEditorPlayer::HandleLeftMenu);
		eInputComponent->BindAction(inputActions[7], ETriggerEvent::Started, this, &AEditorPlayer::HandleRightAButtonPressed);
		eInputComponent->BindAction(inputActions[8], ETriggerEvent::Started, this, &AEditorPlayer::HandleRightBButtonPressed);

	}
}

void AEditorPlayer::InitializeVRHand()
{
	if(vrHandClass == nullptr) return;

	FTransform trans;
	trans.Identity;
	
	FAttachmentTransformRules attachrule(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	//왼손
	leftHand = GetWorld()->SpawnActorDeferred<AEditorHand>(vrHandClass, trans, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (nullptr != leftHand)
	{
		leftHand->SetVRHandIsRight(false);
	}
	leftHand->FinishSpawning(trans);
	leftHand->AttachToComponent(vrRoot, attachrule);

	//오른손
	rightHand = GetWorld()->SpawnActorDeferred<AEditorHand>(vrHandClass, trans, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (nullptr != rightHand)
	{
		rightHand->SetVRHandIsRight(true);
	}
	rightHand->FinishSpawning(trans);
	rightHand->AttachToComponent(vrRoot, attachrule);
}

void AEditorPlayer::ZoomIn()
{
	// 왼손, 오른손의 거리값 가져오기
	FVector leftLoc = leftHand->controllerMesh->GetComponentLocation();
	FVector rightLoc = rightHand->controllerMesh->GetComponentLocation();
	dist = FVector::Distance(leftLoc, rightLoc);

	if (!checkStartDist) {
		prevZoomRate = dist;
		checkStartDist = true;
	}

	currentZoomRate = dist/prevZoomRate;
	
	if (currentZoomRate < 1)
		currentZoomRate = -1;
	else
		currentZoomRate = 1;

	currentZoomRate *= dist * 0.5f;
	FVector startPos = zoomSplines[currentCameraIndex]->startPosition->GetComponentLocation();
	FVector endPos = zoomSplines[currentCameraIndex]->endPosition->GetComponentLocation();
	float maxDistance = (endPos-startPos).Size();
	float minDistance = 0.f;
	FVector zoomRate = zoomSplines[currentCameraIndex]->snapPoint->GetComponentLocation() + GetActorForwardVector() * currentZoomRate;
	float distanceFromStart = (zoomRate - startPos).Size();
	float t = distanceFromStart/maxDistance;
	 
	if (distanceFromStart >= minDistance && distanceFromStart <= maxDistance)
		zoomSplines[currentCameraIndex]->snapPoint->SetWorldLocation(zoomRate);
	else {
		if(distanceFromStart<minDistance)
			zoomSplines[currentCameraIndex]->snapPoint->SetWorldLocation(startPos);
		else if (distanceFromStart>maxDistance)
			zoomSplines[currentCameraIndex]->snapPoint->SetWorldLocation(endPos);
	}
	
	SetActorLocation(zoomSplines[currentCameraIndex]->snapPoint->GetComponentLocation());
}

void AEditorPlayer::SwitchCamera(bool sth)
{
	
}

void AEditorPlayer::GetZoomSplines()
{
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEditorZoomSpline::StaticClass(), foundActors);
	for (int i = 0; i<foundActors.Num(); i++) {
		AEditorZoomSpline* spline = Cast<AEditorZoomSpline>(foundActors[i]);
		if (spline != nullptr)
			zoomSplines.Add(spline);
	}
	SetActorLocation(zoomSplines[0]->snapPoint->GetComponentLocation());
	SetActorRotation(zoomSplines[0]->snapPoint->GetComponentRotation());
}

void AEditorPlayer::SetEditableSpawner(class AEditableSpawner* spawner)
{
	editableSpawner = spawner;
	if (editableSpawner != nullptr) {
		//스폰 UI
		leftHand->menuWidgetActor->menuWidget->PanelVisibilityOn(leftHand->menuWidgetActor->menuWidget->canvas_SpawnProperty, true);
	}
}

void AEditorPlayer::SetEditableProp(class AEditableProp* prop)
{
	editableProp = prop;
}

void AEditorPlayer::SaveSpawnerInfo()
{
	editableSpawner->spawnerInfo.enemyType = editableSpawner->enemyType;
	
	gameState->OnSpawnerInfoUpdate(editableSpawner);
	/*editableSpawner->spawnerInfo.x = editableSpawner->GetActorLocation().X;
	editableSpawner->spawnerInfo.y = editableSpawner->GetActorLocation().Y;
	editableSpawner->spawnerInfo.z = editableSpawner->GetActorLocation().Z;*/
}

void AEditorPlayer::SavePropInfo()
{
	/*editableProp->propInfo.x = editableProp->GetActorLocation().X;
	editableProp->propInfo.y = editableProp->GetActorLocation().Y;
	editableProp->propInfo.z = editableProp->GetActorLocation().Z;*/
	editableProp->propInfo.yaw = editableProp->GetActorRotation().Yaw;

	gameState->OnPropInfoUpdate(editableProp);
}

void AEditorPlayer::HandleLeftGripPressed()
{
	Super::HandleLeftGripPressed();

	bLeftGripped = true;
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("LEFT GripPressed!")));
}

void AEditorPlayer::HandleLeftGripReleased()
{
	bLeftGripped = false;
	checkStartDist = false;
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("LEFT GripReleassed!")));
}

void AEditorPlayer::HandleLeftThumbstickLeft()
{
	Super::HandleLeftThumbstickLeft();
	// 왼쪽으로 카메라 스위치
	if (zoomSplines.Num() > 1) {
		prevCameraIndex = currentCameraIndex;
		currentCameraIndex--;
		if (currentCameraIndex < 0) {
			currentCameraIndex = 3;
			prevCameraIndex = 0;
		}
		FVector prevLoc = zoomSplines[prevCameraIndex]->snapPoint->GetRelativeLocation();
		zoomSplines[currentCameraIndex]->snapPoint->SetRelativeLocation(prevLoc);
		SetActorLocation(zoomSplines[currentCameraIndex]->snapPoint->GetComponentLocation());
		SetActorRotation(zoomSplines[currentCameraIndex]->snapPoint->GetComponentRotation());
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("LEFT")));
	}
}

void AEditorPlayer::HandleLeftThumbstickRight()
{
	Super::HandleLeftThumbstickRight();
	// 오른쪽으로 카메라 스위치
	if (zoomSplines.Num() > 1) {
		prevCameraIndex = currentCameraIndex;
		currentCameraIndex++;
		if (currentCameraIndex > 3)
			currentCameraIndex = 0;
		FVector prevLoc = zoomSplines[prevCameraIndex]->snapPoint->GetRelativeLocation();
		zoomSplines[currentCameraIndex]->snapPoint->SetRelativeLocation(prevLoc);
		SetActorLocation(zoomSplines[currentCameraIndex]->snapPoint->GetComponentLocation());
		SetActorRotation(zoomSplines[currentCameraIndex]->snapPoint->GetComponentRotation());
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("RIGHT")));
	}
}

void AEditorPlayer::HandleLeftMenu()
{
	Super::HandleLeftMenu();
	// 메뉴 ui 띄우기
	if (!leftHand->menuWidgetComp->IsVisible())
		leftHand->menuWidgetComp->SetVisibility(true);
	else {
		leftHand->menuWidgetComp->SetVisibility(false);
		leftHand->inventoryWidgetComp->SetVisibility(false);
		if (OnStaticVisibleOff.IsBound())
			OnStaticVisibleOff.Broadcast();
	}
}

void AEditorPlayer::HandleRightGripPressed()
{
	Super::HandleRightGripPressed();
	bRightGripped = true;
	rightHand->widgetinteractor->PressPointerKey(EKeys::LeftMouseButton);
	if (spawnableActor != nullptr) {
		rightHand->bSpawnTrace = true;
		rightHand->bSelectTrace = false;
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Right Grip Pressed!")));
	}
	if (rightHand->interactableActor != nullptr) {
		rightHand->bSpawnTrace = true;
		rightHand->bSelectTrace = false;
		interactableActor = nullptr;
		bRelocate = true;
		spawnableActor = rightHand->interactableActor;
	}
}

void AEditorPlayer::HandleRightGripReleased()
{
	bRightGripped = false;
	checkStartDist = false;
	rightHand->widgetinteractor->ReleasePointerKey(EKeys::LeftMouseButton);
	if (spawnableActor != nullptr) {
		if (bRelocate) {
			if (spawnableActor->cantSpawn) return;
			if (spawnRoatation != relocateRotation)
				spawnableActor->SetActorLocationAndRotation(spawnLocation, relocateRotation);
			else
				spawnableActor->SetActorLocationAndRotation(spawnLocation, spawnRoatation);

			if (spawnableActor->ActorHasTag(FName("PlayerStartPoint")))
				gameState->OnPlayerStartUpdate();
		}
		else
			spawnableActor->Destroy();
		rightHand->bSpawnTrace = false;
		rightHand->bSelectTrace = true;
		bRelocate = false;
		spawnableActor = nullptr;
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Right GripReleassed!")));
	}
}

void AEditorPlayer::HandleRightTriggerPressed()
{
	if (spawnableActor != nullptr) {
		if (!spawnableActor->cantSpawn) {
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AEditableActor* spawnedObject = GetWorld()->SpawnActor<AEditableActor>(spawnableActor_bp, spawnLocation, spawnRoatation, param);
		UGameplayStatics::PlaySound2D(GetWorld(), canSpawnSound);
		
		}
		else
		UGameplayStatics::PlaySound2D(GetWorld(), cantSpawnSound);
	}
	

	if (rightHand->bCanSelect) {
		//interactableActors.Add(rightHand->interactableActor);
		if (interactableActor != nullptr) interactableActor->ShowRedBoundary(false);
		interactableActor = rightHand->interactableActor;
		spawnableActor = nullptr;
		interactableActor->SetPlayerPtr();
		interactableActor->ShowRedBoundary(true);
		if (editableProp != nullptr) {
			editableProp->ShowRotateUI(true);
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("show rotate ui")));
		}
	}
	else {
		if (interactableActor == nullptr) return;
		if(editableSpawner != nullptr) {
			SaveSpawnerInfo();
			editableSpawner = nullptr;
			leftHand->menuWidgetActor->menuWidget->PanelVisibilityOn(leftHand->menuWidgetActor->menuWidget->canvas_SpawnProperty, false);
		}
		else if(editableProp != nullptr) {
			SavePropInfo();
			editableProp->ShowRotateUI(false);
			editableProp = nullptr;
		}
		interactableActor->ShowRedBoundary(false);
		//interactableActors.SetNum(0);
		interactableActor = nullptr;
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("emptyyyyyyyyyyy")));
	}
}

void AEditorPlayer::HandleRightTriggerReleased()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Right Trigger Releassed!")));
}

void AEditorPlayer::HandleRightThumbstickAxis(const FInputActionValue& Value)
{
	float rotateValue = Value.Get<float>();
	if (rotateValue != 0) rotateValue = FMath::Sign(rotateValue);

	if (interactableActor != nullptr/*!interactableActors.IsEmpty()*/) {
		interactableActor->AddActorWorldRotation(FRotator(0, rotateValue * 1.3f, 0));
		relocateRotation = interactableActor->GetActorRotation();
		//if (interactableActors.Num() < 2) {
		//	interactableActors[0]->AddActorWorldRotation(FRotator(0, rotateValue*1.3f, 0)); 
		//}
		//else {
		//	// 다중 선택 시 구현
		//	/*for (AEditableActor* actor : interactableActors) {
		//		actor->AddActorWorldRotation(FRotator(0, rotateValue, 0));
		//	}*/

		//}
	}
}

void AEditorPlayer::HandleRightAButtonPressed()
{
	if (interactableActor == nullptr) return;

	if (editableSpawner != nullptr) {
		editableSpawner->Destroy();
		editableSpawner = nullptr;
	}
	else if (editableProp != nullptr) {
		editableProp->Destroy();
		editableProp = nullptr;
	}
	else if (interactableActor->ActorHasTag(FName("PlayerStartPoint"))) return;
	// 배치할 공간이 없어서 계속 들고 있을 때 삭제 가능
	else if (spawnableActor != nullptr) {
		spawnableActor->Destroy();
		spawnableActor = nullptr;
	}

	// 다중 선택 가능시
	/*if (!interactableActors.IsEmpty()) {
		for (AEditableActor* actor : interactableActors) {
			if (AEditableSpawner* spawner = Cast<AEditableSpawner>(actor)) {
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("destroy spawner")));
				spawner->Destroy();
			}
			else if (AEditableProp* prop = Cast<AEditableProp>(actor)) {
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("destroy prop")));
				prop->Destroy();
			}
			else if (actor->ActorHasTag(FName("PlayerStartPoint"))) return;
			interactableActors.Empty();
		}
	}*/
}

void AEditorPlayer::HandleRightBButtonPressed()
{
	// 수동 저장 기능
	gameState->SaveMapData();
}
