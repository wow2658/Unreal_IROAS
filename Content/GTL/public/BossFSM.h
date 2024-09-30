// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/StaticMeshActor.h"
#include "IROASEnums.h"
#include "BossFSM.generated.h"

class ABoss;
class UBossAnim;
class ABattleGameStateBase;
class ABattleGameModeBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class IROAS_API UBossFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossFSM();

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY()
	ABattleGameModeBase* gmb = nullptr;
	UPROPERTY()
	ABattleGameStateBase* gsb = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	EEnemyState mBossState = EEnemyState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	float idleDelayTime = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	float AttackReadyTime = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	float MovetoAttackTime = 1.93 / 1.5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	float currentTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	float DieDelayTime = 5;

	// Animation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Anim)
	class UBossAnim* BossAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Anim)
	class AAIController* ai;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Anim)
	ABoss* Boss;

public:
	//FSM와 Tick함수에서 최초 1회만 시작하기 위한 플래그 bool 변수들
	void IdleState();
	bool isIdle = false;
	void DeadState();
	bool isDead = false;
	void MoveState();
	bool isMove = false;
	void AttackState();
	bool isAttack = false;
	void DamageState();
	bool isDamage = false;
	void BlinkState();
	bool isBlink = false;
	void GroggyState();
	bool isGroggy = false;
	UFUNCTION()
	void OnGameFinished(bool isPlayerDead);

	// BossAnim에 전송하여 애니메이션을 동기화하기 위한 변수
	uint8 PatternCnt = 0;
	uint8 randomMovecnt = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	int32 hitcnt;
	TMap<uint8, FString> MoveDirectionMap;

	// 공격 패턴 순서 커스텀 용도 (아직 딱히 사용되지는 않았음, ex_0이라면 Rock, 1이라면 Slash)
	TArray<uint8> ArrPatternCnt = { 0,1,1,0,1,0,0,1 };

	// 블링크 Location
	UFUNCTION()
	void FindAllBossLocations();
	// 월드상에 배치된 Static 액터 Ex_꼬깔콘들 중 Tag로 BossLocation이 들어간 것들을 찾아 넣는공간, 아직은 이게 충족되지 않으면 Crash내버림.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	TArray<AStaticMeshActor*> BossLocations;

	int32 CurrentBossLocationIndex = 2; // 최초시작은 0 1 "2" 3 4 중 가운데에서 시작하기때문
	int32 PreviousBossLocationIndex; // 이전에 있었던 장소
	int32 NextBossLocationIndex; // 다음에 갈 장소 (pre 와 next를 딱히 사용하고 있지는 않음)

	float BlinkTimer = 0;;
	float BlinkDuration = 0.1;
	float BlinkAlpha;
	int32 BlinkCnt = 0;

	// 블링크 패턴
	void BlinkPattern01();
	TArray<int32> BlinkPattern01Index = { 2,4,1,3 };
	void BlinkPattern02();
	TArray<int32> BlinkPattern02Index = { 1,4,0,2 };
	void BlinkPattern03();
	TArray<int32> BlinkPattern03Index = { 3,0,2,0,1 };

	
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void RibbonActive();
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void RibbonDeactivate();
};
