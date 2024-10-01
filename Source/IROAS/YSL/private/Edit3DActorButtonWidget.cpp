// Fill out your copyright notice in the Description page of Project Settings.


#include "Edit3DActorButtonWidget.h"
#include <UMG/Public/Components/Button.h>
#include <Kismet/GameplayStatics.h>
#include "EnemySpawner.h"
#include "EditorPlayer.h"
#include "MovableFrame.h"
#include "EditableActor.h"
#include "EditStaticMeshComponent.h"

void UEdit3DActorButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	editor = Cast<AEditorPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	btn_Button->OnHovered.AddDynamic(this, &UEdit3DActorButtonWidget::OnHovered);
	btn_Button->OnUnhovered
		.AddDynamic(this, &UEdit3DActorButtonWidget::OnUnhovered);
	btn_Button->OnPressed.AddDynamic(this, &UEdit3DActorButtonWidget::OnPressed);

	curSize = buttonMesh->GetRelativeScale3D();
}

void UEdit3DActorButtonWidget::OnHovered()
{
	if (buttonMesh != nullptr) {
		buttonMesh->SetRelativeScale3D(curSize *1.2f);
	}
}

void UEdit3DActorButtonWidget::OnUnhovered()
{
	if (buttonMesh != nullptr) {
		buttonMesh->SetRelativeScale3D(curSize);
	}	
}


void UEdit3DActorButtonWidget::OnPressed()
{
	if (spawnableMesh != nullptr) {
		if (GetName().Contains(FString::Printf(TEXT("20")))) // 아침
			ChangeTimeZone(ETimeZone::Morning);
		else if (GetName().Contains(FString::Printf(TEXT("21")))) // 점심
			ChangeTimeZone(ETimeZone::Noon);
		else if (GetName().Contains(FString::Printf(TEXT("22")))) // 저녁
			ChangeTimeZone(ETimeZone::Evening);
		else if (GetName().Contains(FString::Printf(TEXT("23")))) // 패스 스루 버튼
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("PASS THROUGH")));
		else {
			FActorSpawnParameters param;
			param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AEditableActor* spawnedObject = GetWorld()->SpawnActor<AEditableActor>(spawnableMesh, param);
			editor->spawnableActor = spawnedObject;
			editor->spawnableActor_bp = spawnableMesh;
			//editor->spawnableActor->GetOwner(editor);
		}
	}
}

void UEdit3DActorButtonWidget::ChangeTimeZone(ETimeZone tz)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("%s"), tz));
}
