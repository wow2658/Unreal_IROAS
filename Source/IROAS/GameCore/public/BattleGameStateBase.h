
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "IROASStructs.h"
#include "BattleGameStateBase.generated.h"

class UIROASGameInstance;
class ABattleGameModeBase;
class AEnemySpawner;
class UStaticMesh;
class AStaticMeshActor;
class UDataTable;
class AShootableUIActor;
class ABattlePlayer;
class AHttpRequestActor;
class URankingBoardWidget;
class ABattleResultActor;
class ABoss;
class UBossFSM;
class AAmbientSound;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameFinishSignature, bool, bPlayerWin);

UCLASS()
class IROAS_API ABattleGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	ABattleGameStateBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UIROASGameInstance* gi;
	ABattleGameModeBase* gm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString token = "";
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString userNickName = "";
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 mapID = 3;
	bool bIsSteamId = false;

	FMapInfoTable* mapBuildInfo = nullptr;
	// 랭킹조회 결과
	TArray<FLeaderboardInfo> leaderBoards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* mapDatas;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemySpawner> bp_spawner;

	UPROPERTY(EditDefaultsOnly, Category = SpawnSetting)
	TSubclassOf<class AMeleeEnemy> Melee1Factory;

	UPROPERTY(EditDefaultsOnly, Category = SpawnSetting)
	TSubclassOf<class ARangedGroundEnemy> RangedGround1Factory;

	UPROPERTY()
	TArray<AEnemySpawner*> spawners;
	UPROPERTY()
	TArray<UStaticMesh*> propMeshes;
	UPROPERTY()
	TArray<AStaticMeshActor*> props;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, FString> meshPaths;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, UStaticMesh*> meshAssets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCustomMap = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLobby = true;
	UPROPERTY()
	AAmbientSound* bgmPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Haptics")
	UHapticFeedbackEffect_Base* roarHaptic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* waveBgm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* bossBgm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float slowMoTime = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float sloMoRatio = 0.2f;
	float slomoCurrTime = 0.f;
	bool bIsSlowMotion = false;

public:
	UFUNCTION()
	void BuildMap();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AHttpRequestActor* reqActor = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString playStartTime = "";
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString playClearTime = "";
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bClearYN = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsGameStarted = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsGameFinished = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bWaveInProgress = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 totalEnemyCnt = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 monolithCnt = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABoss* bossRef = nullptr;

	UBossFSM* bossFsmRef = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 currWave = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Timer")
	float gameTimeElapsed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Timer")
	float gameStartTime = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Timer")
	float uiModetime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AShootableUIActor> bp_startTrigger;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AShootableUIActor* startTriggerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABattleResultActor> bp_rankingUI;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABattleResultActor* rankingUIRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABattleResultActor> bp_rewardUI;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABattleResultActor* rewardUIRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABattlePlayer* player_1 = nullptr;

	//TArray<TArray<AEnemySpawner*>> spawners;
	UPROPERTY(VisibleAnywhere)
	TArray<AEnemySpawner*> wave_1_spawners;
	UPROPERTY(VisibleAnywhere)
	TArray<AEnemySpawner*> wave_2_spawners;

	TArray<TArray<AEnemySpawner*>> allSpawners;
	

	//게임 종료타이밍에 처리해야할 작업들 바인딩을 위한 Delegate
	UPROPERTY()
	FGameFinishSignature onGameFinishDelegate;

	//디버깅용 leaderBoards
	TArray<FLeaderboardInfo> tempLeaderBoards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDebuggingMode = false;

	//Play 정보 수집 위한 Broker
	//AEventBroker* broker;

	//Play 정보 Post를 위한 구조체
	FPlayResultInfo playResultInfo;

public:
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int64 killTotalEnemy = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int64 killRangedEnemy = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int64 killMeleeEnemy = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString choiceReward = "";
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int64 firstWaveTakenTime = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int64 secondWaveTakenTime = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 headShot = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 shootCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 feverMode = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 recoveryEnergy = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 recoveryEnergybyMelee = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 recoveryEnergybyRanged = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 accuracyHit = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 reloadCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 playerHitCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 summonCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 spawnerHitCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float accuracyHitByGun = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 startTriggerHP = 0;*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 elapsedTimeMiliSec = 0;


public:
	UFUNCTION()
	void StartGame(AActor* DestroyedActor);
	UFUNCTION()
	void OnRewardSelected(AActor* DestroyedActor);
	UFUNCTION()
	void StartNextWave();
	UFUNCTION()
	void OnGameFinished();
	UFUNCTION()
	void OnGameFailed();
	UFUNCTION()
	void OnGameCleared();
	UFUNCTION()
	void GetbossRef();

	// 백데이터 넘길 변수 정리 함수
	void SetData();
	void DebuggingLearderBoards();

	UFUNCTION(BlueprintCallable)
	void DebuggingReward();
};
