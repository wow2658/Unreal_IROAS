

#include "BattlePlayerIKAnimComp.h"
#include "BattlePlayer.h"
#include "BattlePlayerAnimInstance.h"
#include "PlayerHand.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "HeadMountedDisplay/Public/MotionControllerComponent.h"

UBattlePlayerIKAnimComp::UBattlePlayerIKAnimComp()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UBattlePlayerIKAnimComp::BeginPlay()
{
	Super::BeginPlay();

	player = GetOwner<ABattlePlayer>();
	if (player != nullptr) {
		playerAnim = Cast<UBattlePlayerAnimInstance>(player->characterMeshComp->GetAnimInstance());
	}
	
}


void UBattlePlayerIKAnimComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (player != nullptr && playerAnim != nullptr) {
		playerAnim->headLoc = player->camera->GetComponentLocation();
		playerAnim->headRot = player->camera->GetComponentRotation();

		playerAnim->leftHandLoc = player->leftHand->motionController->GetComponentLocation();
		playerAnim->leftHandRot = player->leftHand->motionController->GetComponentRotation();

		playerAnim->rightHandLoc = player->rightHand->motionController->GetComponentLocation();
		playerAnim->rightHandRot = player->rightHand->motionController->GetComponentRotation();
	}

}

