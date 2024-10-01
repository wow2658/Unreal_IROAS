// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayer.h"
#include "PlayerHand.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "BasicMovementComponent.h"
#include "PlayerHandAnim.h"
#include <Engine/Engine.h>
#include <Components/CapsuleComponent.h>

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/GameModeBase.h"


ALobbyPlayer::ALobbyPlayer() :
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CapsuleCollision->SetupAttachment(vrRoot);
	CapsuleCollision->SetRelativeLocation(FVector(0, 0, -(desiredEyeHeight / 2.0f)));
	CapsuleCollision->SetCollisionProfileName(FName("PlayerBody"));

	// 컴포넌트 패턴
	// moveComp = CreateDefaultSubobject<UBasicMovementComponent>(TEXT("MoveComponent"));

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	}

}

void ALobbyPlayer::BeginPlay()
{
	Super::BeginPlay();
	InitializeHMD();
	InitializeVRHand();

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
}

void ALobbyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);




}

void ALobbyPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
		
	UEnhancedInputComponent* eInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (eInputComponent != nullptr) {

		eInputComponent->BindAction(inputActions[0], ETriggerEvent::Triggered, this, &ALobbyPlayer::Rotate);
		eInputComponent->BindAction(inputActions[0], ETriggerEvent::Completed, this, &ALobbyPlayer::Rotate);
		eInputComponent->BindAction(inputActions[1], ETriggerEvent::Triggered, this, &ALobbyPlayer::Rotate);
		eInputComponent->BindAction(inputActions[1], ETriggerEvent::Completed, this, &ALobbyPlayer::Rotate);
		// moveComp->SetupPlayerInputComponent(eInputComponent, inputActions);
		eInputComponent->BindAction(inputActions[2], ETriggerEvent::Started, this, &ALobbyPlayer::HandleLeftGripPressed);
		eInputComponent->BindAction(inputActions[2], ETriggerEvent::Completed, this, &ALobbyPlayer::HandleLeftGripReleased);
		eInputComponent->BindAction(inputActions[3], ETriggerEvent::Started, this, &ALobbyPlayer::HandleRightGripPressed);
		eInputComponent->BindAction(inputActions[3], ETriggerEvent::Completed, this, &ALobbyPlayer::HandleRightGripReleased);
		eInputComponent->BindAction(inputActions[4], ETriggerEvent::Started, this, &ALobbyPlayer::HandleLeftTriggerPressed);
		eInputComponent->BindAction(inputActions[4], ETriggerEvent::Completed, this, &ALobbyPlayer::HandleLeftTriggerReleased);
		eInputComponent->BindAction(inputActions[5], ETriggerEvent::Started, this, &ALobbyPlayer::HandleRightTriggerPressed);
		eInputComponent->BindAction(inputActions[5], ETriggerEvent::Completed, this, &ALobbyPlayer::HandleRightTriggerReleased);

	}

	
}

void ALobbyPlayer::InitializeVRHand()
{
	//Super::InitializeVRHand();
	if (vrHandClass == nullptr)
		return;

	FTransform trans;
	trans.Identity;

	FAttachmentTransformRules attachrule(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	//왼손
	leftHand = GetWorld()->SpawnActorDeferred<APlayerHand>(vrHandClass, trans, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (nullptr != leftHand)
	{
		leftHand->SetVRHandIsRight(false);
	}
	leftHand->FinishSpawning(trans);
	leftHand->AttachToComponent(vrRoot, attachrule);

	//오른손
	rightHand = GetWorld()->SpawnActorDeferred<APlayerHand>(vrHandClass, trans, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (nullptr != rightHand)
	{
		rightHand->SetVRHandIsRight(true);
	}
	rightHand->FinishSpawning(trans);
	rightHand->AttachToComponent(vrRoot, attachrule);
}

void ALobbyPlayer::HandleLeftGripPressed()
{

}

void ALobbyPlayer::HandleLeftGripReleased()
{

}

void ALobbyPlayer::HandleLeftTriggerPressed()
{

}

void ALobbyPlayer::HandleLeftTriggerReleased()
{

}

void ALobbyPlayer::HandleLeftThumbstickAxis(const FInputActionValue& Value)
{

}

void ALobbyPlayer::HandleLeftThumbstickLeft()
{

}

void ALobbyPlayer::HandleLeftThumbstickRight()
{

}

void ALobbyPlayer::HandleLeftMenu()
{

}

void ALobbyPlayer::HandleRightGripPressed()
{
	
}

void ALobbyPlayer::HandleRightGripReleased()
{

}

void ALobbyPlayer::HandleRightTriggerPressed()
{

}

void ALobbyPlayer::HandleRightTriggerReleased()
{

}

void ALobbyPlayer::HandleRightThumbstickAxis(const FInputActionValue& Value)
{

}

void ALobbyPlayer::HandleRightThumbstickLeft()
{

}

void ALobbyPlayer::HandleRightThumbstickRight()
{

}

void ALobbyPlayer::CreateGameSession()
{
	// Called when pressing the 1 key
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
}

void ALobbyPlayer::JoinGameSession()
{
	// Find game sessions
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void ALobbyPlayer::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Created session: %s"), *SessionName.ToString())
			);
		}

		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(FString("/Game/GTL/GTL_Lobby?listen")); 
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session!"))
			);
		}
	}
}

void ALobbyPlayer::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	for (auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("Id: %s, User: %s"), *Id, *User)
			);
		}
		if (MatchType == FString("FreeForAll"))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Cyan,
					FString::Printf(TEXT("Joining Match Type: %s"), *MatchType)
				);
			}

			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
		}
	}
}
void ALobbyPlayer::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}
	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString::Printf(TEXT("Connect string: %s"), *Address)
			);
		}

		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

void ALobbyPlayer::Move(const FInputActionValue& Value)
{
	FVector2D movementVector = Value.Get<FVector2D>();
	//UE_LOG(LogTemp, Warning, TEXT("X: %.1f  Y: %.1f\n"), movementVector.X, movementVector.Y);
	UE_LOG(LogTemp, Warning, TEXT("Input!!\n"));
	const FRotator rotation = GetControlRotation();
	const FRotator yawRotation(0, rotation.Yaw, 0);

	//Get Forward Vector
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);

	//Get Right Vector
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	//MoveForward
	FVector up = GetActorUpVector();

	FVector camForward = camera->GetForwardVector();

	camForward -= UKismetMathLibrary::ProjectVectorOnToVector(camForward, up);
	camForward.Normalize();

	AddActorWorldOffset(FVector(camForward * movementVector.X * 3));

	//MoveSide
	FVector camRight = camera->GetRightVector();

	camRight -= UKismetMathLibrary::ProjectVectorOnToVector(camRight, up);
	camRight.Normalize();

	AddActorWorldOffset(FVector(camRight * movementVector.Y * 3));
}

void ALobbyPlayer::Rotate(const FInputActionValue& Value)
{
	FVector2D rightConInput = Value.Get<FVector2D>();

	pc->AddYawInput(rightConInput.X);
	pc->AddPitchInput(rightConInput.Y);
}

//void ALobbyPlayer::OpenLobby()
//{
//	UWorld* World = GetWorld();
//	if (World)
//	{
//		World->ServerTravel("/Game/GTL/GTL_Level?listen");
//	}
//}
//
//void ALobbyPlayer::CallOpenLevel(const FString& Address)
//{
//	UGameplayStatics::OpenLevel(this, *Address);
//}
//
//void ALobbyPlayer::CallClientTravel(const FString& Address)
//{	
//	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
//	if (PlayerController)
//	{	
//		AGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AGameModeBase>();
//		if (GameMode)
//		{
//			GameMode->bUseSeamlessTravel = true;
//			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
//		}
//	}
//}
