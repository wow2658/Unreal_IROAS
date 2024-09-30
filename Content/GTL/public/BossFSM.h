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
	//FSM�� Tick�Լ����� ���� 1ȸ�� �����ϱ� ���� �÷��� bool ������
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

	// BossAnim�� �����Ͽ� �ִϸ��̼��� ����ȭ�ϱ� ���� ����
	uint8 PatternCnt = 0;
	uint8 randomMovecnt = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	int32 hitcnt;
	TMap<uint8, FString> MoveDirectionMap;

	// ���� ���� ���� Ŀ���� �뵵 (���� ���� �������� �ʾ���, ex_0�̶�� Rock, 1�̶�� Slash)
	TArray<uint8> ArrPatternCnt = { 0,1,1,0,1,0,0,1 };

	// ��ũ Location
	UFUNCTION()
	void FindAllBossLocations();
	// ����� ��ġ�� Static ���� Ex_�����ܵ� �� Tag�� BossLocation�� �� �͵��� ã�� �ִ°���, ������ �̰� �������� ������ Crash������.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	TArray<AStaticMeshActor*> BossLocations;

	int32 CurrentBossLocationIndex = 2; // ���ʽ����� 0 1 "2" 3 4 �� ������� �����ϱ⶧��
	int32 PreviousBossLocationIndex; // ������ �־��� ���
	int32 NextBossLocationIndex; // ������ �� ��� (pre �� next�� ���� ����ϰ� ������ ����)

	float BlinkTimer = 0;;
	float BlinkDuration = 0.1;
	float BlinkAlpha;
	int32 BlinkCnt = 0;

	// ��ũ ����
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
