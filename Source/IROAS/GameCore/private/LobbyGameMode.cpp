

#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "HttpRequestActor.h"
#include "IROASGameInstance.h"
#include "LobbyTutorialActor.h"
#include "BattlePlayer.h"
#include "LobbyTutorialOrb.h"
#include "SnapMovementComponent.h"
#include "ShootableUIActor.h"
#include "PlayerHand.h"

void ALobbyGameMode::StartPlay()
{
	Super::StartPlay();

	gi = Cast<UIROASGameInstance>(GetGameInstance());
	if (gi != nullptr) {
		myID = gi->MyID;
		isSteamId = gi->bIsSteamId;
		isFirstTime = gi->bIsFirstTime;
		FTransform trans;
		trans.Identity;
		reqActor = GetWorld()->SpawnActorDeferred<AHttpRequestActor>(AHttpRequestActor::StaticClass(), trans, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (reqActor != nullptr) {
			reqActor->gameModeType = EGameModeType::LobbyMode;
		}
		reqActor->FinishSpawning(trans);
		if (reqActor != nullptr) {
			//디버깅용 기능들
			/*
			if (!bIsDebugMode) {
				//reqActor->PostRequestLogin(myID, isSteamId);
				reqActor->PostRequestLogin("76561197960435530", true);
			}
			else {
				if (bIsDebugTutorialOn) {
					SpawnTutorialActor();
				}
				else {
					SpawnStartOrb();
				}
			}
			*/

			//무조건 튜토리얼 진행
			//reqActor->PostRequestLogin("76561197960435530", true);
			reqActor->PostRequestLogin(myID, isSteamId);
		}

		/*
		if (isFirstTime && bp_tutorialActor != nullptr) {
			FVector tutorialLoc = FVector(120, -4290, 253);
			FRotator tutorialRoc = FRotator(0, 90, 0);
			tutorialActor = GetWorld()->SpawnActor<ALobbyTutorialActor>(bp_tutorialActor, tutorialLoc, tutorialRoc);
			tutorialActor->lobbyPlayer = lobbyPlayer;
			tutorialActor->movableFrame1 = lobbyPlayer->snapMoveComp->moveableFrame_1;
			tutorialActor->movableFrame2 = lobbyPlayer->snapMoveComp->moveableFrame_2;
		}
		*/
	}
	
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

		if (GEngine)
		{
			/*
			GEngine->AddOnScreenDebugMessage(
				-1,
				600.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers)
			);

			APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
			if (PlayerState)
			{
				FString PlayerName = PlayerState->GetPlayerName();
				PlayerNames.Add(PlayerName);
				GEngine->AddOnScreenDebugMessage(
					-1,
					60.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has joined the game!"), *PlayerName)
				);

				for (int32 i = 0; i < PlayerNames.Num(); i++)
				{
					FString PlayerNamePrint = PlayerNames[i];
					GEngine->AddOnScreenDebugMessage(
						-1,
						60.f,
						FColor::Cyan,
						FString::Printf(TEXT("%s 's index is %d"), *PlayerNamePrint,i)
					);
				}
				myID = PlayerNames[0];
				GEngine->AddOnScreenDebugMessage(
					-1,
					60.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s Is My NickName"), *myID)
				);
			}
			*/
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
	if (PlayerState)
	{
		/*
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		GEngine->AddOnScreenDebugMessage(
			-1,
			600.f,
			FColor::Yellow,
			FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers - 1)
		);

		FString PlayerName = PlayerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(
			-1,
			60.f,
			FColor::Cyan,
			FString::Printf(TEXT("%s has exited the game!"), *PlayerName)
		);
		*/
	}
}

bool ALobbyGameMode::GetMapListFromDB()
{
	if (reqActor == nullptr) return false;


	reqActor->SendRequestForMapData("http://52.79.89.164/api/v1/maps?size=10&offset=1&sort=createTime&direction=asc");

	return true;
}

void ALobbyGameMode::DeliverNicknameToGI()
{
	if (gi != nullptr) {
		gi->userNickName = userNickname;
		gi->token = token;
		GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Nickname(GameInstance): %s"), *gi->userNickName));
	}
}

void ALobbyGameMode::SpawnTutorialActor()
{
	if (isFirstTime && bp_tutorialActor != nullptr) {
		FVector tutorialLoc = FVector(120, -4290, 293);
		FRotator tutorialRoc = FRotator(0, 90, 0);
		tutorialActor = GetWorld()->SpawnActor<ALobbyTutorialActor>(bp_tutorialActor, tutorialLoc, tutorialRoc);
		tutorialActor->lobbyPlayer = lobbyPlayer;
		tutorialActor->movableFrame1 = lobbyPlayer->snapMoveComp->moveableFrame_1;
		tutorialActor->movableFrame2 = lobbyPlayer->snapMoveComp->moveableFrame_2;
	}
}

void ALobbyGameMode::SpawnStartOrb()
{
	
	if (lobbyPlayer != nullptr) {
		lobbyPlayer->rightHand->tutorialGunSpawn = true;
		lobbyPlayer->rightHand->tutorialSwordSpawn = true;
		lobbyPlayer->leftHand->tutorialSwordSpawn = true;
		lobbyPlayer->leftHand->tutorialGunSpawn = true;
	}

	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform trans;
	trans.SetLocation(FVector(120, -6755, 680));
	trans.SetScale3D(FVector(3));


	if (bp_startOrb != nullptr)
		startOrb = GetWorld()->SpawnActor<AShootableUIActor>(bp_startOrb, trans, param);
}

void ALobbyGameMode::TestAddRanking(const FString& _creatorMapID, const FString& _playStartTime, const FString& _playEndTime, bool _bCleared)
{
	//reqActor->PostRequestNewRank(_creatorMapID, _playStartTime, _playEndTime, _bCleared);
}

void ALobbyGameMode::TestGetLeaderBoard(const FString& _creatorMapId, int32 _pageNum, int32 _pageSize, const FString& _sort, const FString& _direction)
{
	//reqActor->GetRequestLeaderboard(_creatorMapId, 0, 10, "playTime.playMilliSecond", "asc");
}
