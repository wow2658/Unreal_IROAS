// Fill out your copyright notice in the Description page of Project Settings.


#include "EditButtonWidget.h"
#include <UMG/Public/Components/Button.h>
#include "EditMenuWidget.h"
#include "EditMenuWidgetActor.h"
#include "EditInventoryWidget.h"
#include "EditInventoryWidgetActor.h"
#include <UMG/Public/Components/WidgetComponent.h>
#include "EditorPlayer.h"
#include "EditorHand.h"
#include "EditStaticMeshComponent.h"
#include <Kismet/GameplayStatics.h>
#include <UMG/Public/Components/CanvasPanel.h>
#include "EditGameStateBase.h"

void UEditButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	editor = Cast<AEditorPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	inventoryWidgetActor = Cast<AEditInventoryWidgetActor>(editor->leftHand->inventoryWidgetComp->GetChildActor());
	owningActor = Cast<AEditMenuWidgetActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AEditMenuWidgetActor::StaticClass()));
	menuWidget = Cast<UEditMenuWidget>(owningActor->widgetComp->GetWidget());

	btn_Button->OnHovered.AddDynamic(this, &UEditButtonWidget::OnHovered);
	btn_Button->OnUnhovered
	.AddDynamic(this, &UEditButtonWidget::OnUnhovered);
	btn_Button->OnPressed.AddDynamic(this, &UEditButtonWidget::OnPressed);
}

void UEditButtonWidget::OnHovered()
{
	if (buttonMesh != nullptr)
		buttonMesh->SetRelativeScale3D(FVector(hoverSize));
}

void UEditButtonWidget::OnUnhovered()
{
	if (buttonMesh != nullptr)
		buttonMesh->SetRelativeScale3D(FVector(unHoverSize));
}

void UEditButtonWidget::OnPressed()
{
	if (GetName().Contains(TEXT("9")) || GetName().Contains(TEXT("10"))) return;

	UEditInventoryWidget* inventoryWidget = Cast<UEditInventoryWidget>(inventoryWidgetActor->widgetComp->GetWidget());
	
	owningActor->pressedCheckMesh->SetVisibility(true);
	owningActor->editor->leftHand->inventoryWidgetComp->SetVisibility(true);

	if (GetName().Contains(FString::Printf(TEXT("0")))) {
		//스포너버튼
		owningActor->pressedCheckMesh->SetRelativeLocation(owningActor->staticMeshComp[0]->GetRelativeLocation());
		inventoryWidget->SwitchCanvas(0);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_PropCategory, false);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_EnemyProperty, true);
		if (OnStaticVisibleChange.IsBound())
			OnStaticVisibleChange.Broadcast(EStaticMeshType::In_Spawner);
		if (OnCategoryButtonVisibleChange.IsBound())
			OnCategoryButtonVisibleChange.Broadcast(false);
	}
	else if (GetName().Contains(FString::Printf(TEXT("1")))) {
		//프랍버튼
		owningActor->pressedCheckMesh->SetRelativeLocation(owningActor->staticMeshComp[1]->GetRelativeLocation());
		inventoryWidget->SwitchCanvas(1);
	/*	if (menuWidget->canvas_PropCategory->GetVisibility() == ESlateVisibility::Hidden)*/
		menuWidget->PanelVisibilityOn(menuWidget->canvas_PropCategory, true);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_EnemyProperty, false);
		if (OnStaticVisibleChange.IsBound())
			OnStaticVisibleChange.Broadcast(EStaticMeshType::In_Construction);
		if (OnCategoryButtonVisibleChange.IsBound())
			OnCategoryButtonVisibleChange.Broadcast(true);
	}
	else if (GetName().Contains(FString::Printf(TEXT("2")))) {
		//타임존 버튼
		owningActor->pressedCheckMesh->SetRelativeLocation(owningActor->staticMeshComp[2]->GetRelativeLocation());
		inventoryWidget->SwitchCanvas(4);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_PropCategory, false);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_EnemyProperty, false);
		if (OnStaticVisibleChange.IsBound())
			OnStaticVisibleChange.Broadcast(EStaticMeshType::In_TimeZone);
		if (OnCategoryButtonVisibleChange.IsBound())
			OnCategoryButtonVisibleChange.Broadcast(false);
	}
	else if (GetName().Contains(FString::Printf(TEXT("3")))) {
		//패스스루 버튼
		owningActor->pressedCheckMesh->SetRelativeLocation(owningActor->staticMeshComp[3]->GetRelativeLocation());
		inventoryWidget->SwitchCanvas(5);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_PropCategory, false);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_EnemyProperty, false);
		if (OnStaticVisibleChange.IsBound())
			OnStaticVisibleChange.Broadcast(EStaticMeshType::In_Passthrough);
		if (OnCategoryButtonVisibleChange.IsBound())
			OnCategoryButtonVisibleChange.Broadcast(false);
	}
	else if (GetName().Contains(FString::Printf(TEXT("4")))) {
		//홈 버튼
		owningActor->pressedCheckMesh->SetRelativeLocation(owningActor->staticMeshComp[4]->GetRelativeLocation());
		inventoryWidget->SwitchCanvas(4);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_PropCategory, false);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_EnemyProperty, false);

		// 서버에 저장(로컬에도 저장함)
		editor->gameState->UploadMapData();

		// 서버말고 로컬에 저장
		//editor->gameState->SaveMapData();
		
		//로비맵으로 나가기
		UGameplayStatics::OpenLevel(GetWorld(), FName("Proto_LobbyLevel"));
	}
	else if (GetName().Contains(FString::Printf(TEXT("5")))) {
		//카테고리 내 빌딩버튼
		inventoryWidget->SwitchCanvas(1);
		if (menuWidget->canvas_PropCategory->GetVisibility() == ESlateVisibility::Hidden)
			menuWidget->PanelVisibilityOn(menuWidget->canvas_PropCategory, true);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_EnemyProperty, false);
		if (OnStaticVisibleChange.IsBound())
			OnStaticVisibleChange.Broadcast(EStaticMeshType::In_Construction);
		if (OnCategoryButtonVisibleChange.IsBound())
			OnCategoryButtonVisibleChange.Broadcast(true);
	}
	else if (GetName().Contains(FString::Printf(TEXT("6")))) {
		//카테고리 내 프랍버튼
		inventoryWidget->SwitchCanvas(2);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("click INPROPPPPP")));
		if (menuWidget->canvas_PropCategory->GetVisibility() == ESlateVisibility::Hidden)
			menuWidget->PanelVisibilityOn(menuWidget->canvas_PropCategory, true);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_EnemyProperty, false);
		if (OnStaticVisibleChange.IsBound()) {
			OnStaticVisibleChange.Broadcast(EStaticMeshType::In_Prop);
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("INPROPPPPPP")));
		}
		if (OnCategoryButtonVisibleChange.IsBound())
			OnCategoryButtonVisibleChange.Broadcast(true);
	}
	else if (GetName().Contains(FString::Printf(TEXT("7")))) {
		//카테고리 내 폴리지버튼
		inventoryWidget->SwitchCanvas(3);
		if (menuWidget->canvas_PropCategory->GetVisibility() == ESlateVisibility::Hidden)
			menuWidget->PanelVisibilityOn(menuWidget->canvas_PropCategory, true);
		menuWidget->PanelVisibilityOn(menuWidget->canvas_EnemyProperty, false);
		if (OnStaticVisibleChange.IsBound()) {
			OnStaticVisibleChange.Broadcast(EStaticMeshType::In_Foliage);
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("FOLIAGEEEEE")));
		}
		if (OnCategoryButtonVisibleChange.IsBound())
			OnCategoryButtonVisibleChange.Broadcast(true);
	else {
		//카테고리 내 나이아가라버튼
	}
}
}
