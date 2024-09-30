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

	// ü��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossInfo)
	float hp = 500.f;

	// ��ų��Ÿ�� (�Ⱦ���)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossInfo)
	float skillCoolTime = 5.f;

	// �⺻���������� (Rock�������� �����⺻�������� �ι�, Slash�� �״�� ������)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossInfo)
	float defaultAttackPower = 10.f;

	// Boss Size (������ ��Ʈ ĸ�� ������Ʈ ũ�⿡ ������ ��, Mesh����� ���� ������)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossInfo)
	float BossSize = 3.f;

	// Flag (ABP���� Ʈ���������� Ȱ��, )
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossFuntions)
	bool inAction = false;


public: // ��Ƽ�÷��̾� �ν� �� ��׷� ����Ī //

	// GetAllActorsOfClass�� ���� �÷��̾� ���͹迭
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossFuntions)
	TArray<AActor*> Players;
	// �÷��̾ ã�´�
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void FindTarget();

	// Boss ��׷� ����Ī�� ���� index
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BossInfo)
	uint8 index_Player = 0;
	// Ÿ���� �ٲ۴�
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void SwapTarget();

public: // ���� ���� //

	// ����������
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void ThrowRock();
	UPROPERTY(EditAnywhere, Category = "BossFuntions")
	TArray<TSubclassOf<ARock>> RockArray;

	// �˱⳯����
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void BossSlash();

	// ������
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void LionRoar();

	// ����
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void Telekinesis();

public:

	//�������� ������
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void ApplyDamage();

	//�������� �Դ´�
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void OnTakeDamage();

	//����Ѵ�
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void Die();

	//���� ���� RPC �����Լ��� ���÷��� �ۼ��� ���� ���ܳ��� �ּ�
	//UFUNCTION(Server, Reliable, BlueprintCallable, Category = BoomerFunctions)
	//UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = BoomerFunctions)

public:
	// �� �ݸ���
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

public: // ������ƮǮ�� //

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

	// ��ũ ȿ���� ���� ���̾ư��� �ý���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossFuntions)
	class UNiagaraSystem* BlinkVFX;
	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void SpawnBlinkVFX();

	// ��ũ �� ����
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

	// ��ũ ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossFuntions")
	class USoundCue* BlinkSound;

	
public: // �ǰ� �ƿ����� // ���� �Ⱦ��̴� �Լ���

	// �⺻ ������ �����ϱ� ���� ����
	UPROPERTY(VisibleAnywhere)
	UMaterialInterface* OriginalMaterial;

	// �������� ������ �����ϱ� ���� Dynamic Material Instance
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
