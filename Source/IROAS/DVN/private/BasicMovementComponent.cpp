

#include "BasicMovementComponent.h"
#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
//#include "EnhancedInputComponent.h"
//#include "InputActionValue.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>

UBasicMovementComponent::UBasicMovementComponent()
{

}

void UBasicMovementComponent::BeginPlay()
{
	Super::BeginPlay();


}

void UBasicMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UBasicMovementComponent::Move(const FInputActionValue& Value)
{
/*
	FVector2D movementVector = Value.Get<FVector2D>();
	//UE_LOG(LogTemp, Warning, TEXT("X: %.1f  Y: %.1f\n"), movementVector.X, movementVector.Y);
	UE_LOG(LogTemp, Warning, TEXT("Input!!\n"));
	const FRotator rotation = player->GetControlRotation();
	const FRotator yawRotation(0, rotation.Yaw, 0);

	//Get Forward Vector
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);

	//Get Right Vector
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	//MoveForward
	FVector up = player->GetActorUpVector();

	FVector camForward = player->camera->GetForwardVector();

	camForward -= UKismetMathLibrary::ProjectVectorOnToVector(camForward, up);
	camForward.Normalize();

	player->AddActorWorldOffset(FVector(camForward * movementVector.X * 3));

	//MoveSide
	FVector camRight = player->camera->GetRightVector();

	camRight -= UKismetMathLibrary::ProjectVectorOnToVector(camRight, up);
	camRight.Normalize();

	player->AddActorWorldOffset(FVector(camRight * movementVector.Y * 3));
*/
}

void UBasicMovementComponent::Rotate(const FInputActionValue& Value)
{
	FVector2D rightConInput = Value.Get<FVector2D>();
	
	player->pc->AddYawInput(rightConInput.X);
	player->pc->AddPitchInput(rightConInput.Y);
}
