// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorHand.h"
#include <UMG/Public/Components/WidgetInteractionComponent.h>
#include "EditorPlayer.h"
#include "MovableFrame.h"
#include "EditMenuWidgetActor.h"
#include <Components/ChildActorComponent.h>
#include <UMG/Public/Components/WidgetComponent.h>
#include "EditInventoryWidgetActor.h"
#include "EditableActor.h"
#include <Camera/CameraComponent.h>
#include "MotionControllerComponent.h"
#include "Kismet/GameplayStatics.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraDataInterfaceArrayFunctionLibrary.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>

AEditorHand::AEditorHand()
{
	PrimaryActorTick.bCanEverTick = true;

	controllerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Controller"));
	controllerMesh->SetupAttachment(motionController);
	widgetinteractor = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("Widget Interaction"));
	widgetinteractor->SetupAttachment(controllerMesh);
	niddlePointer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Niddle Pointer"));
	niddlePointer->SetupAttachment(controllerMesh, FName("WidgetPoint"));
	menuWidgetComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("Menu Widget"));
	menuWidgetComp->SetupAttachment(controllerMesh);
	menuWidgetComp->SetRelativeLocationAndRotation(FVector(5, 25, 1), FRotator(90, -160, 20));
	menuWidgetComp->SetRelativeScale3D(FVector(0.04));
	menuWidgetComp->SetVisibility(false);
	inventoryWidgetComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("Inventory Widget"));
	inventoryWidgetComp->SetupAttachment(menuWidgetComp);
	inventoryWidgetComp->SetRelativeLocation(FVector(9, 75, -10));
	inventoryWidgetComp->SetRelativeScale3D(FVector(1));
	inventoryWidgetComp->SetVisibility(false);
	/*pointerFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Pointer Effect"));
	pointerFX->SetupAttachment(niddlePointer);
	pointerFX->SetVisibility(false);
	pointerImpactPointFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Pointer Impact Point Effect"));
	pointerImpactPointFX->SetupAttachment(pointerFX);
	pointerImpactPointFX->SetVisibility(false);*/
}

void AEditorHand::BeginPlay()
{	
	Super::BeginPlay();
	InitializeVRHand();
	InitializeWidget();
	editor = GetOwner<AEditorPlayer>();
	movableFrame = Cast<AMovableFrame>(UGameplayStatics::GetActorOfClass(GetWorld(), AMovableFrame::StaticClass()));
}

void AEditorHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bSelectTrace) SelectLineTrace();
	if (bSpawnTrace) SpawnLineTrace();
	if (niddlePointer != nullptr) DrawPointerLine();
}

void AEditorHand::InitializeVRHand()
{
	if(controllerMesh == nullptr) return;

	if (bIsRight == false)
	{
		if (leftMesh != nullptr)
		{
			controllerMesh->SetStaticMesh(leftMesh);
		}
		motionController->SetTrackingSource(EControllerHand::Left);
	}
	else
	{
		if (rightMesh != nullptr)
		{
			controllerMesh->SetStaticMesh(rightMesh);
		}
		motionController->SetTrackingSource(EControllerHand::Right);
	}
}

void AEditorHand::InitializeWidget()
{
	menuWidgetActor = Cast<AEditMenuWidgetActor>(menuWidgetComp->GetChildActor());
	inventoryWidgetActor = Cast<AEditInventoryWidgetActor>(inventoryWidgetComp->GetChildActor());
	if (bIsRight) {
		if(menuWidgetComp->IsVisible())	menuWidgetComp->SetVisibility(false);
		if(inventoryWidgetComp->IsVisible()) inventoryWidgetComp->SetVisibility(false);
		menuWidgetActor->widgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		inventoryWidgetActor->widgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//if(!pointerFX->IsVisible()) pointerFX->SetVisibility(true);
		//if(!pointerImpactPointFX->IsVisible()) pointerImpactPointFX->SetVisibility(true);
	}
	else
		if(niddlePointer->IsVisible())	niddlePointer->SetVisibility(false);
}

void AEditorHand::SpawnLineTrace()
{
	if (!bSpawnTrace) return;

	TArray<FHitResult> hitInfos;
	FCollisionQueryParams param;
	param.AddIgnoredActor(this);
	FVector startPos = niddlePointer->GetComponentLocation();
	FVector endPos = startPos + niddlePointer->GetUpVector() * 7000;
	/*FVector startPos = widgetinteractor->GetComponentLocation();
	FVector endPos = startPos + controllerMesh->GetForwardVector() * 7000;*/

	if (GetWorld()->LineTraceMultiByChannel(hitInfos, startPos, endPos, ECC_Visibility, param))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("SpawnLine")));
		for (const FHitResult hitInfo : hitInfos) {
			if (AActor* hitObject = Cast<AActor>(hitInfo.GetActor())) {
				if (hitObject->GetName().Contains(FString::Printf(TEXT("Floor")))) {
					//DrawDebugLine(GetWorld(), startPos, endPos, FColor::Purple, false, 3, 3);
					FVector onGround = hitInfo.Location;
					onGround.Z = 0;
					editor->spawnLocation = onGround;
					// 에디터 카메라 전방방향 받는 것
					editor->spawnRoatation = (editor->camera->GetForwardVector() * (-1)).Rotation();
					editor->spawnRoatation.Roll = 0;
					editor->spawnRoatation.Pitch = 0;
					
					if (editor->spawnableActor != nullptr) {
						editor->spawnableActor->SetActorLocation(editor->spawnLocation);
						if (editor->bRelocate)
							editor->spawnableActor->SetActorRotation(editor->relocateRotation);
						else
							editor->spawnableActor->SetActorRotation(editor->spawnRoatation);
						editor->spawnableActor->CheckSpawn();
					}
					
				}
			}
		}
	}
}

void AEditorHand::SelectLineTrace()
{
	if(!bSelectTrace) return;

	FHitResult hitInfo;
	FCollisionQueryParams param;
	param.AddIgnoredActor(this);
	FVector startPos = niddlePointer->GetComponentLocation();
	FVector endPos = startPos + niddlePointer->GetUpVector() * 7000;
	
	if (GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_GameTraceChannel7, param)) {
		//DrawDebugLine(GetWorld(), startPos, endPos, FColor::Emerald, false, 3, 3);
		if (AEditableActor* editableActor = Cast<AEditableActor>(hitInfo.GetActor())) {
			editableActor->ShowRedBoundary(true);
			bCanSelect = true;
			interactableActor = editableActor;
		}
		else {
			if (interactableActor != nullptr) {
				if(editor->interactableActor == interactableActor) return;
				interactableActor->ShowRedBoundary(false);
			}
			bCanSelect = false;
			interactableActor = nullptr;
		}
	}
}

void AEditorHand::DrawPointerLine()
{
	//pointerLinePositions.Empty();
	FVector startPos = niddlePointer->GetComponentLocation();
	FVector direction = niddlePointer->GetUpVector();
	FVector endPos = startPos + direction * 10000;
	FCollisionQueryParams param;
	param.AddIgnoredActor(this);

	FHitResult hitInfo;
	
	if (GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_GameTraceChannel7, param)) {
		if (bIsRight) {
			/*pointerImpactPointFX->SetVisibility(true);
			pointerImpactPointFX->SetWorldLocation(hitInfo.ImpactPoint);*/
			debugLineAlpha += 0.01f;
			debugLineAlpha = FMath::Clamp(debugLineAlpha, 0.0f, 1.0f);
			FColor debugColor = FColor(0, 139, 179, debugLineAlpha*255);

			FVector lineEndPos = hitInfo.ImpactPoint;

			DrawDebugLine(GetWorld(), startPos, lineEndPos, debugColor, false, -1.0f, 0, 0.15f);
			DrawDebugPoint(GetWorld(), lineEndPos, 6.0f, debugColor);
		
		}
	}
}
