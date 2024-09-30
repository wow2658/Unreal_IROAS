// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OnDamage_Interface.h"
#include "BossFSM.h"
#include "Boss.generated.h"

class ABossSlash;
class ARock;

UCLASS()
class IROAS_API ABoss : public APawn, public IOnDamage_Interface
{
	GENERATED_BODY()

public:
	ABoss();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossInfo)
	float hp = 500.f;

	// 스킬쿨타임 (안쓰임)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossInfo)
	float skillCoolTime = 5.f;

	// 기본데미지세기 (Rock데미지는 보스기본데미지의 두배, Slash는 그대로 가져감)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossInfo)
	float defaultAttackPower = 10.f;

	// Boss Size (보스의 루트 캡슐 컴포넌트 크기에 영향을 줌, Mesh사이즈도 같이 영향줌)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossInfo)
	float BossSize = 3.f;

	// Flag (ABP에서 트랜지션으로 활용, )
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossFuntions)
	bool inAction = false;


public: // 멀티플레이어 인식 및 어그로 스위칭 //

	// GetAllActorsOfClass로 넣을 플레이어 액터배열
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossFuntions)
	TArray<AActor*> Players;
	// 플레이어를 찾는다
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void FindTarget();

	// Boss 어그로 스위칭을 위한 index
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossInfo)
	uint8 index_Player = 0;
	// 타겟을 바꾼다
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void SwapTarget();

public: // 보스 패턴 //

	// 바위던지기
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void ThrowRock();
	UPROPERTY(EditAnywhere, Category = "BossFuntions")
	TArray<TSubclassOf<ARock>> RockArray;

	// 검기날리기
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void BossSlash();

	// 사자후
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void LionRoar();

	// 염력
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void Telekinesis();

public:

	//데미지를 입힌다
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void ApplyDamage();

	//데미지를 입는다
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void OnTakeDamage();

	//사망한다
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void Die();

	//차후 서버 RPC 원격함수콜 리플렉션 작성을 위해 남겨놓는 주석
	//UFUNCTION(Server, Reliable, BlueprintCallable, Category = BoomerFunctions)
	//UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = BoomerFunctions)

public:
	// 노 콜리전
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	class UCapsuleComponent* CapsuleCollision;
	// Overlap Enemybody
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	class USkeletalMeshComponent* BossMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	USceneComponent* SlashPoint;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	USceneComponent* RockPoint;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	UBossFSM* BossFSM;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	UBossAnim* BossAnim;
	virtual void OnDamage_Interface(float DamageAmount) override;

public: // 오브젝트풀링 //

	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void CreateSlashPool();
	void PutObjectPool();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossFuntions)
	TSubclassOf<ABossSlash> BP_BossSlash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossFuntions)
	TArray<ABossSlash*> BossSlashPool;

	uint8 BossSlashPoolPoolSize = 8;
	uint8 PoolIndex = 0;

public:

	// 블링크 효과를 위한 나이아가라 시스템
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossFuntions)
	class UNiagaraSystem* BlinkVFX;
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void SpawnBlinkVFX();

	// 블링크 중 리본
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossFuntions)
	class UNiagaraComponent* NiagaraRibbonComponent01;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossFuntions)
	class UNiagaraComponent* NiagaraRibbonComponent02;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossFuntions)
	class UNiagaraComponent* NiagaraRibbonComponent03;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossFuntions)
	class UNiagaraComponent* NiagaraRibbonComponent04;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossFuntions)
	class UNiagaraComponent* NiagaraRibbonComponent05;

	// 블링크 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossFuntions")
	class USoundCue* BlinkSound;

	
public: // 피격 아웃라인 // 이제 안쓰이는 함수들

	// 기본 재질을 저장하기 위한 변수
	UPROPERTY(VisibleAnywhere)
	UMaterialInterface* OriginalMaterial;

	// 오버레이 재질을 적용하기 위한 Dynamic Material Instance
	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* HitOverlayMaterialInstance;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* DieOverlayMaterialInstance;

	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void OnHitOverlayMaterial();

	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void OnDieOverlayMaterial();

	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void ResetMaterial();

	FTimerHandle TimerHandle_ResetMaterial_FromHit;
	FTimerHandle TimerHandle_ResetMaterial_FromRage;
	FTimerHandle TimerHandle_StopAnim;
};
