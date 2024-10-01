

#include "BaseCharacter.h"
#include "Components/SceneComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

//EnhancedInput
#include <../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h>
#include <../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h>



ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	defaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = defaultSceneRoot;

	vrRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRootComponent"));
	vrRoot->SetupAttachment(RootComponent);

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	camera->SetupAttachment(vrRoot);

	floatingPawnMovment = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloaingPawnMovementComponent"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//InitializeHMD();
	//InitializeVRHand();

}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/*APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc == nullptr) return;

	UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>((pc->GetLocalPlayer()));

	subSystem->ClearAllMappings();
	subSystem->AddMappingContext(inputMapping, 0);

	UEnhancedInputComponent* eInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);*/

	// Bind
	

}

void ABaseCharacter::CompensateEyeHeight()
{
	FVector location = GetActorLocation();
	FVector cameraLocation = camera->GetRelativeLocation();

	float distanceToCamera = cameraLocation.Z;
	float vrRootRelativeZ = desiredEyeHeight - distanceToCamera;
	FVector RelativeLocation = { 0, 0, vrRootRelativeZ };
	vrRoot->SetRelativeLocation(RelativeLocation);
}

void ABaseCharacter::InitializeHMD()
{
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(trackingOrigin);

	FTimerHandle waitHandle;
	GetWorldTimerManager().SetTimer(waitHandle, FTimerDelegate::CreateLambda([&]()
		{
			CompensateEyeHeight();
			if (ActorHasTag("LobbyPlayer")) {
				vrRoot->SetWorldRotation(FRotator(0, -90, 0));
				camera->SetRelativeRotation(FRotator(0));
				camera->SetWorldRotation(FRotator(0, -90, 0));
			}
		}), 1.0f, false);
}

void ABaseCharacter::ReCallibrateCamera(const FRotator rot)
{
	camera->SetWorldRotation(rot);
	vrRoot->SetWorldRotation(rot);
	//camera->SetWorldRotation(GetActorRotation().Quaternion());
}

