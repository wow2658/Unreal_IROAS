
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "IROASStructs.h"
#include "LobbyGameMode.generated.h"

class UIROASGameInstance;
class AHttpRequestActor;
class AShootableUIActor;
class USoundBase;
class UNiagaraSystem;
class ABattlePlayer;
class ALobbyTutorialActor;

UCLASS()
class IROAS_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void StartPlay() override;

public:
	UIROASGameInstance* gi;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AHttpRequestActor* reqActor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMapInfoTable> mapList;

	TArray<FString> PlayerNames;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString myID = "";
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString userNickname = "";
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isSteamId = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString token;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString resultMessage = "";

	AShootableUIActor* startOrb;
	UPROPERTY(Editanywhere, Category = MySettings)
	TSubclassOf<AShootableUIActor> bp_startOrb;
	UPROPERTY(Editanywhere, Category = MySettings)
	bool bIsDebugMode = false;
	UPROPERTY(Editanywhere, Category = MySettings)
	bool bIsDebugTutorialOn = false;

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	bool GetMapListFromDB();

	void DeliverNicknameToGI();

	void SpawnTutorialActor();

	void SpawnStartOrb();

	UFUNCTION(BlueprintCallable)
	void TestAddRanking(const FString& creatorMapID, const FString& playStartTime, const FString& playEndTime, bool bCleared);
	UFUNCTION(BlueprintCallable)
	void TestGetLeaderBoard(const FString& creatorMapId, int32 pageNum, int32 pageSize, const FString& sort, const FString& direction);

	
	// 튜토리얼, 인트로 관련
	ABattlePlayer* lobbyPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ALobbyTutorialActor* tutorialActor;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ALobbyTutorialActor> bp_tutorialActor;

	bool isFirstTime = false;
};
