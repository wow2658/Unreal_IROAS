

#include "SnapMovementComponent.h"
#include "BaseCharacter.h"
#include "MovableFrame.h"
#include <Kismet/GameplayStatics.h>

USnapMovementComponent::USnapMovementComponent()
{

}

void USnapMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> frames;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMovableFrame::StaticClass(), frames);
	for (AActor* frame : frames) {
		AMovableFrame* tempFrame = Cast<AMovableFrame>(frame);
		if (tempFrame != nullptr ) {
			if (tempFrame->PlayerStartTag == FName("wave_1")) {
				moveableFrame_1 = tempFrame;
			}
			else if (tempFrame->PlayerStartTag == FName("wave_2")) {
				moveableFrame_2 = tempFrame;
			}
			else if (tempFrame->PlayerStartTag == FName("wave_3")) {
				moveableFrame_3 = tempFrame;
			}
		}
	}
	currentFrame = moveableFrame_1;
	currentFrame->InitializePlayerPostitions();
	/*
	AMovableFrame* tempFrame = Cast<AMovableFrame>(UGameplayStatics::GetActorOfClass(GetWorld(), AMovableFrame::StaticClass()));
	if (tempFrame != nullptr) {
		moveableFrame_1 = tempFrame;
	}
	*/
}

void USnapMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	GEngine->AddOnScreenDebugMessage(2, 90, FColor::Green, FString::Printf(TEXT("CurrentFrame: %s"), *currentFrame->PlayerStartTag.ToString()));
}

void USnapMovementComponent::SwitchMovableFrame()
{
	if (moveableFrame_2 != nullptr && currentFrame == moveableFrame_1) {
		currentFrame = moveableFrame_2;
		currentFrame->InitializePlayerPostitions();
		player->SetActorRotation(currentFrame->GetActorRotation());
	}
	else if (moveableFrame_3 != nullptr && currentFrame == moveableFrame_2) {
		currentFrame = moveableFrame_3;
		currentFrame->InitializePlayerPostitions();
		player->SetActorRotation(currentFrame->GetActorRotation());
	}
	/*if (moveableFrame_2 != currentFrame) {
		currentFrame = moveableFrame_2;
		currentFrame->InitializePlayerPostitions();
		player->SetActorRotation(currentFrame->GetActorRotation());
	}*/
}

void USnapMovementComponent::MoveLeft()
{
	currentFrame->MovePlayerLeft(player);
}

void USnapMovementComponent::MoveRight()
{
	currentFrame->MovePlayerRight(player);
}
