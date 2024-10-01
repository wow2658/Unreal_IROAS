

#include "MovableFrame.h"
#include <Components/ArrowComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include "BaseCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>
#include <Components/BoxComponent.h>
#include <Engine/NavigationObjectBase.h>
#include "BattlePlayerIndicator.h"
#include <Kismet/KismetMathLibrary.h>
#include "GameFramework/FloatingPawnMovement.h"
#include <Camera/CameraComponent.h>


AMovableFrame::AMovableFrame(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	snapPoint_1 = CreateDefaultSubobject<UArrowComponent>(TEXT("SnapPoint_1"));
	snapPoint_1->SetupAttachment(RootComponent);
	snapPoint_1->SetRelativeLocation(FVector(0, -200, 0));
	snapPoints.Add(snapPoint_1);

	snapRing_1 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SnapRing_1"));
	snapRing_1->SetupAttachment(snapPoint_1);

	snapPoint_2 = CreateDefaultSubobject<UArrowComponent>(TEXT("SnapPoint_2"));
	snapPoint_2->SetupAttachment(RootComponent);
	snapPoint_2->SetRelativeLocation(FVector(0, -100, 0));
	snapPoints.Add(snapPoint_2);

	snapRing_2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SnapRing_2"));
	snapRing_2->SetupAttachment(snapPoint_2);

	snapPoint_3 = CreateDefaultSubobject<UArrowComponent>(TEXT("SnapPoint_3"));
	snapPoint_3->SetupAttachment(RootComponent);
	snapPoints.Add(snapPoint_3);

	snapRing_3 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SnapRing_3"));
	snapRing_3->SetupAttachment(snapPoint_3);

	snapPoint_4 = CreateDefaultSubobject<UArrowComponent>(TEXT("SnapPoint_4"));
	snapPoint_4->SetupAttachment(RootComponent);
	snapPoint_4->SetRelativeLocation(FVector(0, 100, 0));
	snapPoints.Add(snapPoint_4);

	snapRing_4 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SnapRing_4"));
	snapRing_4->SetupAttachment(snapPoint_4);

	snapPoint_5 = CreateDefaultSubobject<UArrowComponent>(TEXT("SnapPoint_5"));
	snapPoint_5->SetupAttachment(RootComponent);
	snapPoint_5->SetRelativeLocation(FVector(0, 200, 0));
	snapPoints.Add(snapPoint_5);

	snapRing_5 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SnapRing_5"));
	snapRing_5->SetupAttachment(snapPoint_5);

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempNiagara(TEXT("/Script/Niagara.NiagaraSystem'/Game/DVN/VFX/NS_SnapRing.NS_SnapRing'"));
	if (tempNiagara.Succeeded()) {
		snapRing_1->SetAsset(tempNiagara.Object);
		snapRing_2->SetAsset(tempNiagara.Object);
		snapRing_3->SetAsset(tempNiagara.Object);
		snapRing_4->SetAsset(tempNiagara.Object);
		snapRing_5->SetAsset(tempNiagara.Object);
	}

	GetCapsuleComponent()->SetMobility(EComponentMobility::Movable);

	boxComp= CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	boxComp->SetupAttachment(RootComponent);
	boxComp->SetRelativeLocation(FVector(-15.f, 0.f, 40.f));
	boxComp->SetBoxExtent(FVector(1.f, 180.f, 32.f));
	boxComp->SetCollisionObjectType(ECC_WorldDynamic);
	boxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	boxComp->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);


}

void AMovableFrame::BeginPlay()
{
	Super::BeginPlay();

	//onPositionChanged.Broadcast(occupation);

	/*FTimerHandle initHandle;
	GetWorldTimerManager().SetTimer(initHandle, FTimerDelegate::CreateLambda([&]()
		{
			InitializePlayerPostitions();
		}), 1.2f, false);*/

	// 이동 타임라인
	FOnTimelineFloat OnMoveProcess;
	OnMoveProcess.BindUFunction(this, FName("MoveProcess"));
	FOnTimelineEvent onFinishMove;
	onFinishMove.BindUFunction(this, FName("OnMoveFinished"));
	
	moveTimeline.AddInterpFloat(moveCurve, OnMoveProcess);
	moveTimeline.SetTimelineFinishedFunc(onFinishMove);
	moveTimeline.SetTimelineLength(moveTime);
}

void AMovableFrame::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	moveTimeline.TickTimeline(DeltaSeconds);
}

void AMovableFrame::InitializePlayerPostitions()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), actors);
	if (actors.Num() >= 1) {
		player_1 = Cast<ABaseCharacter>(actors[0]);
		if (player_1 != nullptr) {
			if (actors.Num() == 2) {
				player_2 = Cast<ABaseCharacter>(actors[1]);
				if (player_2 != nullptr) {
					playerLoc_1 = 1;
					occupation[playerLoc_1] = true;
					player_1->SetActorLocation(snapPoints[playerLoc_1]->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);

					FTimerHandle rotateHandle;
					GetWorldTimerManager().SetTimer(rotateHandle, FTimerDelegate::CreateLambda([&]()
					{
						player_1->SetActorRotation(GetActorRotation());
						//player_1->SetActorRotation(GetActorForwardVector().ToOrientationRotator());
						player_1->ReCallibrateCamera(GetActorRotation());
					}), 1.2f, false);
					
					//player_1->SetActorRotation(GetActorForwardVector().ToOrientationQuat());
					
					/*
					player_1->camera->PostProcessSettings.bOverride_MotionBlurAmount = true;
					player_1->camera->PostProcessSettings.bOverride_MotionBlurMax = true;
					player_1->camera->PostProcessSettings.bOverride_MotionBlurTargetFPS = true;
					player_1->camera->PostProcessSettings.MotionBlurAmount = 0.f;
					player_1->camera->PostProcessSettings.MotionBlurMax = 20.f;
					player_1->camera->PostProcessSettings.MotionBlurTargetFPS = 7;
					*/
					playerLoc_2 = 3;
					occupation[playerLoc_2] = true;
					player_2->SetActorLocation(snapPoints[playerLoc_2]->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
					player_2->SetActorRotation(GetActorForwardVector().ToOrientationQuat());
					player_2->ReCallibrateCamera(GetActorRotation());
					/*
					player_2->camera->PostProcessSettings.bOverride_MotionBlurAmount = true;
					player_2->camera->PostProcessSettings.bOverride_MotionBlurMax = true;
					player_2->camera->PostProcessSettings.bOverride_MotionBlurTargetFPS = true;
					player_2->camera->PostProcessSettings.MotionBlurAmount = 0.f;
					player_2->camera->PostProcessSettings.MotionBlurMax = 20.f;
					player_2->camera->PostProcessSettings.MotionBlurTargetFPS = 7;
					*/
				}
			}
			else {
				playerLoc_1 = 2;
				occupation[playerLoc_1] = true;
				player_1->SetActorLocation(snapPoints[playerLoc_1]->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
				//FTimerHandle rotateHandle;
				//GetWorldTimerManager().SetTimer(rotateHandle, FTimerDelegate::CreateLambda([&]()
				//	{

				//		player_1->SetActorRotation(GetActorRotation());
				//		//player_1->SetActorRotation(GetActorForwardVector().ToOrientationRotator());
				//		//player_1->ReCallibrateCamera(GetActorRotation());
				//	}), 1.2f, false);

			}
		}
	}
	onPositionChanged.Broadcast(occupation);
}

void AMovableFrame::ResetPosition()
{
	if (player_1 != nullptr) {
		if (player_2 != nullptr) {
			playerLoc_1 = 1;
			occupation[playerLoc_1] = true;
			player_1->SetActorLocation(snapPoints[playerLoc_1]->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
			player_1->SetActorRotation(GetActorForwardVector().ToOrientationQuat());
			player_1->ReCallibrateCamera(GetActorRotation());

			playerLoc_2 = 3;
			occupation[playerLoc_2] = true;
			player_2->SetActorLocation(snapPoints[playerLoc_2]->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
			player_2->SetActorRotation(GetActorForwardVector().ToOrientationQuat());
			player_2->ReCallibrateCamera(GetActorRotation());
		}
		else {
			playerLoc_1 = 2;
			occupation[playerLoc_1] = true;
			player_1->SetActorLocation(snapPoints[playerLoc_1]->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
			player_1->SetActorRotation(GetActorForwardVector().ToOrientationQuat());
			player_1->ReCallibrateCamera(GetActorRotation());

		}
	}
	onPositionChanged.Broadcast(occupation);
}

void AMovableFrame::MovePlayerLeft(ABaseCharacter* player)
{
	currPosition = player->GetActorLocation();
	if (player == player_1) {
		int8 tempLoc = FindAvailableLocationLeft(playerLoc_1);
		if (tempLoc != playerLoc_1) {
			//currLoc = snapPoints[playerLoc_1]->GetComponentLocation();
			nextPosition = snapPoints[tempLoc]->GetComponentLocation();
			nextPosition.Z = currPosition.Z;
			//currLoc.Z = currLoc.Z;

			occupation[playerLoc_1] = false;
			playerLoc_1 = tempLoc;
			occupation[playerLoc_1] = true;
			
			FVector dir = UKismetMathLibrary::FindLookAtRotation(currPosition, nextPosition).Vector();

			//player_1->camera->PostProcessSettings.MotionBlurAmount = 1.0f;
			moveTimeline.PlayFromStart();

			if (moveSound != nullptr) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), moveSound, player_1->GetActorLocation());
			}
			//player_1->SetActorLocation(snapPoints[playerLoc_1]->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);

			//player_1->floatingPawnMovment->AddInputVector();
			/*
			if (playerIndicator != nullptr)
				playerIndicator->IndicateStart(EIndicatorType::LeftMove);
			*/
		}
		else if (immovableSound != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), immovableSound, player_1->GetActorLocation());
		}
	}
	else if (player == player_2) {
		int8 tempLoc = FindAvailableLocationLeft(playerLoc_2);
		if (tempLoc != playerLoc_2) {
			//currLoc = snapPoints[playerLoc_2]->GetComponentLocation();
			nextPosition = snapPoints[tempLoc]->GetComponentLocation();
			nextPosition.Z = currPosition.Z;
			//currLoc.Z = currPlayerLoc.Z;

			occupation[playerLoc_2] = false;
			playerLoc_2 = tempLoc;
			occupation[playerLoc_2] = true;

			FVector dir = UKismetMathLibrary::FindLookAtRotation(currPosition, nextPosition).Vector();

			//player_2->camera->PostProcessSettings.MotionBlurAmount = 1.0f;
			 moveTimeline.PlayFromStart();
			 if (moveSound != nullptr) {
				 UGameplayStatics::PlaySoundAtLocation(GetWorld(), moveSound, player_2->GetActorLocation());
			 }
			//player_2->SetActorLocation(snapPoints[playerLoc_2]->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
			/*
			if (playerIndicator != nullptr)
				playerIndicator->IndicateStart(EIndicatorType::LeftMove);
			*/
		}
		else if (immovableSound != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), immovableSound, player_2->GetActorLocation());
		}
	}
	onPositionChanged.Broadcast(occupation);
}

void AMovableFrame::MovePlayerRight(ABaseCharacter* player)
{
	currPosition = player->GetActorLocation();
	if (player == player_1) {
		int8 tempLoc = FindAvailableLocationRight(playerLoc_1);
		if (tempLoc != playerLoc_1) {
			//currLoc = snapPoints[playerLoc_1]->GetComponentLocation();
			nextPosition = snapPoints[tempLoc]->GetComponentLocation();
			nextPosition.Z = currPosition.Z;
			//currLoc.Z = playerCurrentLoc.Z;

			occupation[playerLoc_1] = false;
			playerLoc_1 = tempLoc;
			occupation[playerLoc_1] = true;

			FVector dir = UKismetMathLibrary::FindLookAtRotation(currPosition, nextPosition).Vector();

			//player_1->camera->PostProcessSettings.MotionBlurAmount = 1.0f;
			moveTimeline.PlayFromStart();
			
			if (moveSound != nullptr) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), moveSound, player_1->GetActorLocation());
			}

			//player_1->SetActorLocation(snapPoints[playerLoc_1]->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
			/*
			if (playerIndicator != nullptr)
				playerIndicator->IndicateStart(EIndicatorType::RightMove);
			*/
		}
		else if (immovableSound != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), immovableSound, player_1->GetActorLocation());
		}
	}
	else if (player == player_2) {
		int8 tempLoc = FindAvailableLocationRight(playerLoc_2);
		if (tempLoc != playerLoc_2) {
			//currLoc = snapPoints[playerLoc_2]->GetComponentLocation();
			nextPosition = snapPoints[tempLoc]->GetComponentLocation();
			nextPosition.Z = currPosition.Z;
			//currLoc.Z = playerCurrentLoc.Z;

			occupation[playerLoc_2] = false;
			playerLoc_2 = tempLoc;
			occupation[playerLoc_2] = true;

			FVector dir = UKismetMathLibrary::FindLookAtRotation(currPosition, nextPosition).Vector();

			//player_2->camera->PostProcessSettings.MotionBlurAmount = 1.0f;

			moveTimeline.PlayFromStart();
			if (moveSound != nullptr) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), moveSound, player_2->GetActorLocation());
			}

			//player_2->SetActorLocation(snapPoints[playerLoc_2]->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
			/*
			if (playerIndicator != nullptr)
				playerIndicator->IndicateStart(EIndicatorType::RightMove);
			*/
		}
		else if (immovableSound != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), immovableSound, player_2->GetActorLocation());
		}
	}
	onPositionChanged.Broadcast(occupation);
}

int8 AMovableFrame::FindAvailableLocationLeft(int8 currLocation)
{
	int8 nextLoc = currLocation - 1;
	while (nextLoc >= 0 && occupation[nextLoc]) {
		nextLoc--;
	}
	nextLoc = FMath::Max<int8>(0, nextLoc);
	/*for (uint8 i = currLocation - 1; i >= 0; i--) {
		if (occupation[i]) continue;
		else {
			currLocation = i;
			break;
		}
	}*/
	return nextLoc;
}

int8 AMovableFrame::FindAvailableLocationRight(int8 currLocation)
{
	for (int8 i = currLocation + 1; i < 5; i++) {
		if (occupation[i]) continue;
		else {
			currLocation = i;
			break;
		}
	}
	return currLocation;
}

bool AMovableFrame::TutorialCheckRightMove()
{
	if (occupation[4]) 
		return true;
	else 
		return false;
}

bool AMovableFrame::TutorialCheckMiddleMove()
{
	if (occupation[2])
		return true;
	else
		return false;
}

void AMovableFrame::MoveProcess(float alpha)
{
	if (player_1 != nullptr) {
		player_1->SetActorLocation(FMath::Lerp(currPosition, nextPosition, alpha));
	}
}

void AMovableFrame::OnMoveFinished()
{
	if (player_1 != nullptr) {
		//player_1->camera->PostProcessSettings.MotionBlurAmount = 0.f;
	}
	if (player_2 != nullptr) {
		//player_2->camera->PostProcessSettings.MotionBlurAmount = 0.f;
	}
}

