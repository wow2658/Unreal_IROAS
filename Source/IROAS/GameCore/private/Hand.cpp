

#include "Hand.h"
#include <HeadMountedDisplay/Public/MotionControllerComponent.h>

AHand::AHand()
{
	PrimaryActorTick.bCanEverTick = true;

	defaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = defaultSceneRoot;

	motionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	motionController->SetupAttachment(defaultSceneRoot);
}

void AHand::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHand::SetVRHandIsRight(bool isRight)
{
	bIsRight = isRight;
}


const FVector AHand::GetControllerWorldLocation()
{
	return motionController->GetComponentLocation();
}

