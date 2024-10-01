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

	ai = Cast<AAIController>(Boss->GetController()); // 사용하지는 않음

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

	// FSM 목차와 요약
	switch (mBossState)
	{
	case EEnemyState::Idle:
		IdleState(); 
		//Attack(ThrowRock)		Rock.cpp에서 Rock이 파괴됐을 경우(EndPlay에서) IdleState로 전환하는 로직있음
		//Attack(BossSlash)		BossAnim.cpp의 AnimNotify_End_Slash()에서 IdleState로 전환하는 로직있음
		//Damage				BossFSM의 DamageState()에서 넘어오는 로직있음
		//Blink					BossFSM의 Blink 기능을 담당하는 함수 BlinkPattern01~BlinkPattern03로 부터 넘어오는 로직있음
		break;
		
	case EEnemyState::Dead:
		DeadState();
		//		BossAnim.cpp의 AnimNotify_Die()에서 넘어와 확정짓는 로직있음
		//		Boss.cpp의 인터페이스 구독 내용에서 넘어오는 로직있음
		break;
	case EEnemyState::Move:
		MoveState();
		//		BossFSM의 IdleState()에서 넘어오는 로직있음
		break;
	case EEnemyState::Attack:
		AttackState();
		//		BossFSM의 IdleState()에서 넘어오는 로직있음
		//		여태까지 각각의 스킬 State를 따로 만들어두진않았음, 나중을 대비해서 Enum에는 미리 만들어뒀음.
		break;
	case EEnemyState::Damage:
		DamageState();
		//		Boss.cpp의 인터페이스 구독 내용에서 넘어오는 로직있음
		break;
	case EEnemyState::Blink:  // Attack도중에 사망하면 Blink하는 버그 발생 -> Rock Destroy됐을때 전환하는 로직때문에 생긴 버그로 파악 및 해결
		BlinkState();
		//		BossFSM의 IdleState()에서 넘어오는 로직있음
		//		BossFSM의 DamageState()에서 넘어오는 로직있음
		break;
	case EEnemyState::Groggy:
		GroggyState();
		break;		
	}
}
/************************************************************************************************

IdleState : 보스가 방어태세를 취하며 idleDelayTime후에 스킬로 공격할 준비를 한다.

*************************************************************************************************/

void UBossFSM::IdleState()
{
	if (!isIdle && BossAnim != nullptr)
	{
		BossAnim->BossAnimState = mBossState;
		currentTime = 0;
		isIdle = true; // 플래그 문닫는 역할
		isAttack = false; // 다른 플래그들 한번더 문닫는 역할
		isMove = false;

		//randomMovecnt = FMath::RandRange(0, 4);
		randomMovecnt = (randomMovecnt + 1)%6;
		//randomMovecnt = 3; 테스트를 위해 하드코딩
		BossAnim->brandomMovecnt = randomMovecnt; // 랜덤 야바위 결과로 다음에 갈 State가 정해지고 그것을 애니메이션에 동기화
	}

	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > idleDelayTime && BossAnim != nullptr) // 현재 1초
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

	// 블링크가 끝나면 Idle로 돌아온다. 여기에서 무빙을 칠수도 있고 또한번 Blink를 할수도있고, Attack을 시작할수도 있는것이다.
	if (currentTime > idleDelayTime && BossAnim != nullptr)
	{
		// idleDelayTime만큼 시간이 흘렀으면 AttackState로 넘어간다.
		mBossState = EEnemyState::Attack;
		isIdle = false; // 다음에 Idle상태로 돌아왔을때 최초1회 영역 들어가기 위해 초기화
		BossAnim->BossAnimState = mBossState; // Attack 상태로 넘어가면서 애니메이션도 동기화

		// 다음 State에서도 currentTime를 활용할 수 있도록 0으로 초기화
		currentTime = 0;
	}
}

void UBossFSM::DeadState()
{
	if (!isDead && BossAnim != nullptr)
	{
		BossAnim->BossAnimState = EEnemyState::Dead; // 버그 파악하느라 한번더 찍어본 함수
		RibbonDeactivate();
		//시네마틱 플레이 (셋뷰타겟)
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			PlayerController->SetViewTargetWithBlend(Boss, 1.f, EViewTargetBlendFunction::VTBlend_EaseInOut,2.0f);
		}
		//콜리전 끄기
		Boss->SetActorEnableCollision(false);
		//플레이어 그만 쳐다보기 -> 이건 tick에다가 조건추가
		isDead = true;
		currentTime=0;

	}

	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > 2.5f  && isDead)// 죽인거 확인 후 1초뒤에
	{
		Boss->bIsCameraBlending = true;

		
		//isDead = false; 다른상태로 딱히 안갈테니까 우선 비활성화
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
			//FMath::RandRange(0, 2),,, 시연용으로 순차적으로 보여주기 위해서 PatternCnt= 0 1 0 1 반복, 나중에 스킬이 늘어나면 ArrPatternCnt으로 패턴 컨트롤 유도
			if (PatternCnt % 2 == 0)
			{
				Boss->ThrowRock(); // 이게 소환한 Rock.cpp에서 Idle로 넘어가는 로직이 있음
				PatternCnt = PatternCnt % 2;
			}
			else if (PatternCnt % 2 == 1)
			{
				Boss->BossSlash(); // 이게 발동시키는 AnimNotify_End_Slash()에서 Idle로 넘어가는 로직이 있음
				PatternCnt = PatternCnt % 2;
			}

			// 애니메이션도 발동하도록 동기화 시켜주기
			BossAnim->bPatternCnt = PatternCnt % 2;
			// 다음 콤보가 발동하도록 PatternCnt높여주기
			PatternCnt++;

			//탈출할때 기본세팅
			currentTime = 0;

			// 아직 ThrowRock과 BossSlash에 대해 따로 FSM을 파놓고 컨트롤 하는것이 아니라
			// 모두 Attack State 상태이면서 애니메이션만 State를 inAction과 inAction_Anim을 이용해서 눈속임

			BossAnim->BossAnimState = mBossState;
			Boss->inAction = true; // Projectiles에서 true로 활용
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
	// 피격 애니메이션 재생시간
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
		// 블링크 패턴을 결정합니다
		PreviousBossLocationIndex = CurrentBossLocationIndex;
		//NextBossLocationIndex = FMath::RandRange(0, 2);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("BlinkOn")));
		//시작 나이아가라
		Boss->SpawnBlinkVFX();
		RibbonActive();
		isBlink = true;
	}

	// 진짜 움직이는 로직
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
	//	//탈출
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
			// 모든 액터에 대한 반복자를 생성합니다. AStaticMeshActor 타입에 대해서만 반복합니다.
			for (TActorIterator<AStaticMeshActor> It(World); It; ++It)
			{
				AStaticMeshActor* MeshActor = *It;

				// 'BossLocation' 태그를 가지고 있는지 확인합니다.
				if (MeshActor && MeshActor->ActorHasTag(FName("BossLocation")))
				{
					// 조건을 만족하는 액터를 TArray에 추가합니다.
					BossLocations.Add(MeshActor);
					if (BossLocations.Num()>0 && MeshActor!=nullptr)
					{
						Algo::Sort(BossLocations, [](const AStaticMeshActor* A, const AStaticMeshActor* B) {
							// 첫 번째 태그를 기준으로 정렬
							if (A->Tags.Num() > 0 && B->Tags.Num() > 0)
							{
								return A->Tags[1].Compare(B->Tags[1]) < 0;
							}
							return false; // 태그가 없는 경우
							});
						/*Algo::Sort(BossLocations, [](const AStaticMeshActor* A, const AStaticMeshActor* B) {
							return A->GetActorLabel() < B->GetActorLabel();
							});*/
					}
					
				}

			}

			/*for (AStaticMeshActor* BossLocation : BossLocations) 꼬깔콘 확인용
			{
				if (BossLocation)
				{
					FString LocationString = BossLocation->GetActorLocation().ToString();
					UE_LOG(LogTemp, Warning, TEXT("Boss Location: %s"), *LocationString);
				}
			}*/
		}
	}

	if (BossLocations.Num() > 0 && BossLocations[0] != nullptr) // 배열에 요소가 있고, 첫 번째 요소가 null이 아닌 경우
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
			// 재시작 나이아가라
			if (BlinkCnt <= 2)
			{
				Boss->SpawnBlinkVFX();
				RibbonActive();

			}
			
		}

		// 모든 이동이 완료되었는지 확인합니다.
		if (BlinkCnt >= BlinkPattern01Index.Num() - 1)
		{
			// 마지막 위치로 이동한 경우 상태를 업데이트합니다.
			CurrentBossLocationIndex = 3;
			mBossState = EEnemyState::Idle;
			BossAnim->BossAnimState = mBossState;
			RibbonDeactivate();
			BlinkCnt = 0;
			isBlink = false;
			
			// 도착 나이아가라
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

		// 모든 이동이 완료되었는지 확인합니다.
		if (BlinkCnt >= BlinkPattern02Index.Num() - 1)
		{
			// 마지막 위치로 이동한 경우 상태를 업데이트합니다.
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

		// 모든 이동이 완료되었는지 확인합니다.
		if (BlinkCnt >= BlinkPattern03Index.Num() - 1)
		{
			// 마지막 위치로 이동한 경우 상태를 업데이트합니다.
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
	
	// 로직


	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > 3)
	{
		//탈출
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
