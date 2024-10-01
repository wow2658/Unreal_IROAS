// Fill out your copyright notice in the Description page of Project Settings.


#include "BossFSM.h"
#include "BossAnim.h"
#include <AIModule/Classes/AIController.h>
#include <NavigationSystem/Public/NavigationSystem.h>
#include "NiagaraComponent.h"
#include "BattlePlayer.h"
#include "Boss.h"
#include "BossSlash.h"
#include "Rock.h"
#include "Kismet/GameplayStatics.h"
#include "BaseEnemy.h"
#include "Engine/DirectionalLight.h"
#include "EngineUtils.h"
#include "Algo/Sort.h"
#include "Sound/SoundCue.h"
#include "BattleGameModeBase.h"
#include "BattleGameStateBase.h"



UBossFSM::UBossFSM()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	MoveDirectionMap.Add(0, "left");
	MoveDirectionMap.Add(1, "right");

}

void UBossFSM::InitializeComponent()
{
	Super::InitializeComponent();
	Boss = Cast<ABoss>(GetOwner());
	if (Boss && Boss->BossMesh)
	{
		BossAnim = Cast<UBossAnim>(Boss->BossMesh->GetAnimInstance());
	}

	ai = Cast<AAIController>(Boss->GetController()); // ��������� ����

	gmb = GetWorld()->GetAuthGameMode<ABattleGameModeBase>();
	if (gmb != nullptr) {
		gsb = gmb->GetGameState<ABattleGameStateBase>();
		gsb->onGameFinishDelegate.AddDynamic(this, &UBossFSM::OnGameFinished);
	}

}

void UBossFSM::BeginPlay()
{
	Super::BeginPlay();
	FindAllBossLocations();
	
	
}

void UBossFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// FSM ������ ���
	switch (mBossState)
	{
	case EEnemyState::Idle:
		IdleState(); 
		//Attack(ThrowRock)		Rock.cpp���� Rock�� �ı����� ���(EndPlay����) IdleState�� ��ȯ�ϴ� ��������
		//Attack(BossSlash)		BossAnim.cpp�� AnimNotify_End_Slash()���� IdleState�� ��ȯ�ϴ� ��������
		//Damage				BossFSM�� DamageState()���� �Ѿ���� ��������
		//Blink					BossFSM�� Blink ����� ����ϴ� �Լ� BlinkPattern01~BlinkPattern03�� ���� �Ѿ���� ��������
		break;
		
	case EEnemyState::Dead:
		DeadState();
		//		BossAnim.cpp�� AnimNotify_Die()���� �Ѿ�� Ȯ������ ��������
		//		Boss.cpp�� �������̽� ���� ���뿡�� �Ѿ���� ��������
		break;
	case EEnemyState::Move:
		MoveState();
		//		BossFSM�� IdleState()���� �Ѿ���� ��������
		break;
	case EEnemyState::Attack:
		AttackState();
		//		BossFSM�� IdleState()���� �Ѿ���� ��������
		//		���±��� ������ ��ų State�� ���� ���������ʾ���, ������ ����ؼ� Enum���� �̸� ��������.
		break;
	case EEnemyState::Damage:
		DamageState();
		//		Boss.cpp�� �������̽� ���� ���뿡�� �Ѿ���� ��������
		break;
	case EEnemyState::Blink:  // Attack���߿� ����ϸ� Blink�ϴ� ���� �߻� -> Rock Destroy������ ��ȯ�ϴ� ���������� ���� ���׷� �ľ� �� �ذ�
		BlinkState();
		//		BossFSM�� IdleState()���� �Ѿ���� ��������
		//		BossFSM�� DamageState()���� �Ѿ���� ��������
		break;
	case EEnemyState::Groggy:
		GroggyState();
		break;		
	}
}
/************************************************************************************************

IdleState : ������ ����¼��� ���ϸ� idleDelayTime�Ŀ� ��ų�� ������ �غ� �Ѵ�.

*************************************************************************************************/

void UBossFSM::IdleState()
{
	if (!isIdle && BossAnim != nullptr)
	{
		BossAnim->BossAnimState = mBossState;
		currentTime = 0;
		isIdle = true; // �÷��� ���ݴ� ����
		isAttack = false; // �ٸ� �÷��׵� �ѹ��� ���ݴ� ����
		isMove = false;

		//randomMovecnt = FMath::RandRange(0, 4);
		randomMovecnt = (randomMovecnt + 1)%6;
		//randomMovecnt = 3; �׽�Ʈ�� ���� �ϵ��ڵ�
		BossAnim->brandomMovecnt = randomMovecnt; // ���� �߹��� ����� ������ �� State�� �������� �װ��� �ִϸ��̼ǿ� ����ȭ
	}

	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > idleDelayTime && BossAnim != nullptr) // ���� 1��
	{
		switch (randomMovecnt)
		{
		case 0: mBossState = EEnemyState::Move; isIdle = false; BossAnim->BossAnimState = mBossState; currentTime = 0; break;
		case 1: mBossState = EEnemyState::Move; isIdle = false; BossAnim->BossAnimState = mBossState; currentTime = 0; break;
		case 2: mBossState = EEnemyState::Blink; isIdle = false; BossAnim->BossAnimState = mBossState; currentTime = 0; break;
		case 3: break;
		case 4: mBossState = EEnemyState::Blink; isIdle = false; BossAnim->BossAnimState = mBossState; currentTime = 0; break;
		case 5: break;
		}
	}

	// ��ũ�� ������ Idle�� ���ƿ´�. ���⿡�� ������ ĥ���� �ְ� ���ѹ� Blink�� �Ҽ����ְ�, Attack�� �����Ҽ��� �ִ°��̴�.
	if (currentTime > idleDelayTime && BossAnim != nullptr)
	{
		// idleDelayTime��ŭ �ð��� �귶���� AttackState�� �Ѿ��.
		mBossState = EEnemyState::Attack;
		isIdle = false; // ������ Idle���·� ���ƿ����� ����1ȸ ���� ���� ���� �ʱ�ȭ
		BossAnim->BossAnimState = mBossState; // Attack ���·� �Ѿ�鼭 �ִϸ��̼ǵ� ����ȭ

		// ���� State������ currentTime�� Ȱ���� �� �ֵ��� 0���� �ʱ�ȭ
		currentTime = 0;
	}
}

void UBossFSM::DeadState()
{
	if (!isDead && BossAnim != nullptr)
	{
		BossAnim->BossAnimState = EEnemyState::Dead; // ���� �ľ��ϴ��� �ѹ��� �� �Լ�
		RibbonDeactivate();
		//�ó׸�ƽ �÷��� (�º�Ÿ��)
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			PlayerController->SetViewTargetWithBlend(Boss, 1.f, EViewTargetBlendFunction::VTBlend_EaseInOut,2.0f);
		}
		//�ݸ��� ����
		Boss->SetActorEnableCollision(false);
		//�÷��̾� �׸� �Ĵٺ��� -> �̰� tick���ٰ� �����߰�
		isDead = true;
		currentTime=0;

	}

	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > 2.5f  && isDead)// ���ΰ� Ȯ�� �� 1�ʵڿ�
	{
		Boss->bIsCameraBlending = true;

		
		//isDead = false; �ٸ����·� ���� �Ȱ��״ϱ� �켱 ��Ȱ��ȭ
	}
}

void UBossFSM::MoveState()
{
	if (!isMove && BossAnim != nullptr)
	{
		
		if (MoveDirectionMap[randomMovecnt] == "left" && MoveDirectionMap.Contains(0))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("left")));

		}
		else if (MoveDirectionMap[randomMovecnt] == "right" && MoveDirectionMap.Contains(1))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("right")));

		}
		isMove = true;

	}

	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > MovetoAttackTime)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("MoveState")));
		Boss->BossMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		mBossState = EEnemyState::Attack;
		isMove = false;
		BossAnim->BossAnimState = mBossState;
		currentTime = 0;
	}
}

void UBossFSM::AttackState()
{

	if (!isAttack && BossAnim != nullptr && Boss != nullptr)
	{
		BossAnim->BossAnimState = mBossState;

		Boss->inAction = false;
		currentTime = 0;
		Boss->SwapTarget();
		isAttack = true;
	}

	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > AttackReadyTime)
	{
		if (!Boss->inAction && !BossAnim->inAction_Anim)
		{
			//FMath::RandRange(0, 2),,, �ÿ������� ���������� �����ֱ� ���ؼ� PatternCnt= 0 1 0 1 �ݺ�, ���߿� ��ų�� �þ�� ArrPatternCnt���� ���� ��Ʈ�� ����
			if (PatternCnt % 2 == 0)
			{
				Boss->ThrowRock(); // �̰� ��ȯ�� Rock.cpp���� Idle�� �Ѿ�� ������ ����
				PatternCnt = PatternCnt % 2;
			}
			else if (PatternCnt % 2 == 1)
			{
				Boss->BossSlash(); // �̰� �ߵ���Ű�� AnimNotify_End_Slash()���� Idle�� �Ѿ�� ������ ����
				PatternCnt = PatternCnt % 2;
			}

			// �ִϸ��̼ǵ� �ߵ��ϵ��� ����ȭ �����ֱ�
			BossAnim->bPatternCnt = PatternCnt % 2;
			// ���� �޺��� �ߵ��ϵ��� PatternCnt�����ֱ�
			PatternCnt++;

			//Ż���Ҷ� �⺻����
			currentTime = 0;

			// ���� ThrowRock�� BossSlash�� ���� ���� FSM�� �ĳ��� ��Ʈ�� �ϴ°��� �ƴ϶�
			// ��� Attack State �����̸鼭 �ִϸ��̼Ǹ� State�� inAction�� inAction_Anim�� �̿��ؼ� ������

			BossAnim->BossAnimState = mBossState;
			Boss->inAction = true; // Projectiles���� true�� Ȱ��
			BossAnim->inAction_Anim = true;

		}
	}
}

void UBossFSM::DamageState()
{
	BossAnim->BossAnimState = EEnemyState::Damage;
	
	if (!isDamage)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("1")));
		currentTime = 0;
		isDamage = true;
	}


	currentTime += GetWorld()->DeltaTimeSeconds;
	// �ǰ� �ִϸ��̼� ����ð�
	if (currentTime > 1.33 && hitcnt < 2 && BossAnim->BossAnimState != EEnemyState::Dead)
	{
		
		BossAnim->BossAnimState = EEnemyState::Idle;
		mBossState = EEnemyState::Idle;
		isDamage = false;
		currentTime=0;
	}
	else if (hitcnt >= 2 && BossAnim->BossAnimState != EEnemyState::Dead) 
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("3")));
		BossAnim->BossAnimState = EEnemyState::Idle;
		mBossState = EEnemyState::Idle;
		hitcnt = 0;
		BossAnim->bhitcnt = hitcnt;
		isDamage = false;
	}
}

void UBossFSM::BlinkState()
{
	if (!isBlink)
	{
		BossAnim->BossAnimState = mBossState;
		// ��ũ ������ �����մϴ�
		PreviousBossLocationIndex = CurrentBossLocationIndex;
		//NextBossLocationIndex = FMath::RandRange(0, 2);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("BlinkOn")));
		//���� ���̾ư���
		Boss->SpawnBlinkVFX();
		RibbonActive();
		isBlink = true;
	}

	// ��¥ �����̴� ����
	if (CurrentBossLocationIndex == 2 && isBlink)
	{
		//2 4 1 3
		BlinkPattern01();
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Blink01")));
	}
	else if (CurrentBossLocationIndex == 1 && isBlink)
	{
		//1 4 0 2
		BlinkPattern02();
	}
	else if (CurrentBossLocationIndex == 3 && isBlink)
	{
		//3 0 2 0 1
		BlinkPattern03();
	}







	//currentTime += GetWorld()->DeltaTimeSeconds;
	//if (currentTime > 3)
	//{
	//	//Ż��
	//	//mBossState = EEnemyState::Groggy;
	//	currentTime = 0;
	//	isBlink = false;
	//}
}

void UBossFSM::FindAllBossLocations()
{
	if (Boss)
	{
		UWorld* World = Boss->GetWorld();
		if (World)
		{
			// ��� ���Ϳ� ���� �ݺ��ڸ� �����մϴ�. AStaticMeshActor Ÿ�Կ� ���ؼ��� �ݺ��մϴ�.
			for (TActorIterator<AStaticMeshActor> It(World); It; ++It)
			{
				AStaticMeshActor* MeshActor = *It;

				// 'BossLocation' �±׸� ������ �ִ��� Ȯ���մϴ�.
				if (MeshActor && MeshActor->ActorHasTag(FName("BossLocation")))
				{
					// ������ �����ϴ� ���͸� TArray�� �߰��մϴ�.
					BossLocations.Add(MeshActor);
					if (BossLocations.Num()>0 && MeshActor!=nullptr)
					{
						Algo::Sort(BossLocations, [](const AStaticMeshActor* A, const AStaticMeshActor* B) {
							// ù ��° �±׸� �������� ����
							if (A->Tags.Num() > 0 && B->Tags.Num() > 0)
							{
								return A->Tags[1].Compare(B->Tags[1]) < 0;
							}
							return false; // �±װ� ���� ���
							});
						/*Algo::Sort(BossLocations, [](const AStaticMeshActor* A, const AStaticMeshActor* B) {
							return A->GetActorLabel() < B->GetActorLabel();
							});*/
					}
					
				}

			}

			/*for (AStaticMeshActor* BossLocation : BossLocations) ������ Ȯ�ο�
			{
				if (BossLocation)
				{
					FString LocationString = BossLocation->GetActorLocation().ToString();
					UE_LOG(LogTemp, Warning, TEXT("Boss Location: %s"), *LocationString);
				}
			}*/
		}
	}

	if (BossLocations.Num() > 0 && BossLocations[0] != nullptr) // �迭�� ��Ұ� �ְ�, ù ��° ��Ұ� null�� �ƴ� ���
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, BossLocations[0]->GetActorLabel());
	}
}



void UBossFSM::BlinkPattern01()
{
	//2 4 1 3
	BlinkTimer += GetWorld()->DeltaTimeSeconds;
	if (BlinkTimer <= BlinkDuration)
	{
	
		BlinkAlpha = BlinkTimer / BlinkDuration;
		int32 CurrentIndex = BlinkPattern01Index[BlinkCnt % BlinkPattern01Index.Num()];
		int32 NextIndex = BlinkPattern01Index[(BlinkCnt + 1) % BlinkPattern01Index.Num()];

		FVector StartLocation = BossLocations[CurrentIndex]->GetActorLocation();
		FVector EndLocation = BossLocations[NextIndex]->GetActorLocation();

		FVector NewLocation = FMath::Lerp(StartLocation, EndLocation,BlinkAlpha);

		Boss->SetActorLocation(NewLocation);
	}
	else if (BlinkTimer >= BlinkDuration)
	{
		RibbonDeactivate();
		if (BlinkTimer>= BlinkDuration+0.5)
		{
			BlinkAlpha = 0;
			BlinkTimer = 0;
			BlinkCnt++;
			// ����� ���̾ư���
			if (BlinkCnt <= 2)
			{
				Boss->SpawnBlinkVFX();
				RibbonActive();

			}
			
		}

		// ��� �̵��� �Ϸ�Ǿ����� Ȯ���մϴ�.
		if (BlinkCnt >= BlinkPattern01Index.Num() - 1)
		{
			// ������ ��ġ�� �̵��� ��� ���¸� ������Ʈ�մϴ�.
			CurrentBossLocationIndex = 3;
			mBossState = EEnemyState::Idle;
			BossAnim->BossAnimState = mBossState;
			RibbonDeactivate();
			BlinkCnt = 0;
			isBlink = false;
			
			// ���� ���̾ư���
		}
	}
}

void UBossFSM::BlinkPattern02()
{
	// 1 4 0 2
	BlinkTimer += GetWorld()->DeltaTimeSeconds;
	if (BlinkTimer <= BlinkDuration)
	{

		BlinkAlpha = BlinkTimer / BlinkDuration;
		int32 CurrentIndex = BlinkPattern02Index[BlinkCnt % BlinkPattern02Index.Num()];
		int32 NextIndex = BlinkPattern02Index[(BlinkCnt + 1) % BlinkPattern02Index.Num()];

		FVector StartLocation = BossLocations[CurrentIndex]->GetActorLocation();
		FVector EndLocation = BossLocations[NextIndex]->GetActorLocation();

		FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, BlinkAlpha);

		Boss->SetActorLocation(NewLocation);
	}
	else if (BlinkTimer >= BlinkDuration)
	{
		RibbonDeactivate();
		if (BlinkTimer >= BlinkDuration + 0.5)
		{
			BlinkAlpha = 0;
			BlinkTimer = 0;
			BlinkCnt++;
			if (BlinkCnt <= 2)
			{
				Boss->SpawnBlinkVFX();
				RibbonActive();

			}
		}

		// ��� �̵��� �Ϸ�Ǿ����� Ȯ���մϴ�.
		if (BlinkCnt >= BlinkPattern02Index.Num() - 1)
		{
			// ������ ��ġ�� �̵��� ��� ���¸� ������Ʈ�մϴ�.
			CurrentBossLocationIndex = 2;
			mBossState = EEnemyState::Idle;
			RibbonDeactivate();
			BossAnim->BossAnimState = mBossState;
			BlinkCnt = 0;
			isBlink = false;
			
		}
	}
}

void UBossFSM::BlinkPattern03()
{
	// 3 0 2 0 1
	BlinkTimer += GetWorld()->DeltaTimeSeconds;
	if (BlinkTimer <= BlinkDuration)
	{

		BlinkAlpha = BlinkTimer / BlinkDuration;
		int32 CurrentIndex = BlinkPattern03Index[BlinkCnt % BlinkPattern03Index.Num()];
		int32 NextIndex = BlinkPattern03Index[(BlinkCnt + 1) % BlinkPattern03Index.Num()];

		FVector StartLocation = BossLocations[CurrentIndex]->GetActorLocation();
		FVector EndLocation = BossLocations[NextIndex]->GetActorLocation();

		FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, BlinkAlpha);

		Boss->SetActorLocation(NewLocation);
	}
	else if (BlinkTimer >= BlinkDuration)
	{
		RibbonDeactivate();
		if (BlinkTimer >= BlinkDuration + 0.25)
		{
			BlinkAlpha = 0;
			BlinkTimer = 0;
			BlinkCnt++;
			if (BlinkCnt <= 3)
			{
				Boss->SpawnBlinkVFX();
				RibbonActive();
			}
			
		}

		// ��� �̵��� �Ϸ�Ǿ����� Ȯ���մϴ�.
		if (BlinkCnt >= BlinkPattern03Index.Num() - 1)
		{
			// ������ ��ġ�� �̵��� ��� ���¸� ������Ʈ�մϴ�.
			CurrentBossLocationIndex = 1;
			mBossState = EEnemyState::Idle;
			RibbonDeactivate();
			BossAnim->BossAnimState = EEnemyState::Idle;
			BlinkCnt = 0;
			isBlink = false;
			
		}
	}
}

void UBossFSM::RibbonActive()
{
	Boss->NiagaraRibbonComponent01->Activate();
	Boss->NiagaraRibbonComponent02->Activate();
	Boss->NiagaraRibbonComponent03->Activate();
	Boss->NiagaraRibbonComponent04->Activate();
	Boss->NiagaraRibbonComponent05->Activate();
	if (Boss->BlinkSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Boss->BlinkSound, Boss->GetActorLocation());
	}
}

void UBossFSM::RibbonDeactivate()
{
	Boss->NiagaraRibbonComponent01->Deactivate();
	Boss->NiagaraRibbonComponent02->Deactivate();
	Boss->NiagaraRibbonComponent03->Deactivate();
	Boss->NiagaraRibbonComponent04->Deactivate();
	Boss->NiagaraRibbonComponent05->Deactivate();
}

void UBossFSM::GroggyState()
{

	if (!isGroggy)
	{

		isGroggy = true;

	}
	
	// ����


	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > 3)
	{
		//Ż��
		//mBossState = EEnemyState::Blink;
		currentTime = 0;
		isGroggy = false;
	}
}

void UBossFSM::OnGameFinished(bool isPlayerDead)
{
	//Boss->SetActorTickEnabled(false);
	//SetComponentTickEnabled(false);
}
