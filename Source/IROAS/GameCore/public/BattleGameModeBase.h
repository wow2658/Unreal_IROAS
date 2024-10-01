
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IROASStructs.h"
#include "TimerManager.h"
#include "BattleGameModeBase.generated.h"

class UIROASGameInstance;
struct FMapInfoTable;
class AEnemySpawner;
class ABoss;
class ULevelSequencePlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBossSpawnSignature);

UCLASS()
class IROAS_API ABattleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleGameModeBase();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick( float DeltaSeconds ) override;

public:
	UIROASGameInstance* gi;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMapInfoTable mapInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mysetting | Enemy")
	TArray<AActor*> Enemys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mysetting | Enemy")
	int32 EnemyCnt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mysetting | Enemy")
	int32 CurrentEnemyCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AEnemySpawner*> spawners;


public:
	UFUNCTION()
	void OnLoadComplete();

	// 보스를 소환합니다.
	void SpawnBoss();

	UFUNCTION()
    void OnLevelSequenceFinished();

	// 총에너미 수 위젯에 넣어주기
	int32 GetTotalEnemyCount() { return TotalEnemyCount; }

    // Reference to the LevelSequenceActor in the scene
    UPROPERTY(BlueprintReadWrite, Category = "Sequence")
    class ALevelSequenceActor* MyLevelSequenceActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ABoss* bossRef = nullptr;

	FBossSpawnSignature onBossSpawned;


private:
	// 게임 시작 시 레벨에서의 총 에너미 수
	int32 TotalEnemyCount;

	// 에너미 체크 타이머 핸들
	FTimerHandle CheckEnemyTimerHandle;

	// 보스 블루프린트 클래스 참조.
	UPROPERTY(EditDefaultsOnly, Category = "Boss")
	TSubclassOf<ABoss> BP_BossClass;



};
