

#include "BattleGameModeBase.h"
#include <Engine/StreamableManager.h>
#include "IROASGameInstance.h"
#include <Engine/Engine.h>
#include "Kismet/GameplayStatics.h"
#include "BaseEnemy.h"
#include "Boss.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "EnemySpawner.h"
#include "../GTL/public/BossAnim.h"



ABattleGameModeBase::ABattleGameModeBase()
{

}

void ABattleGameModeBase::BeginPlay()
{
	Super::BeginPlay();

    UIROASGameInstance* tempGI = Cast<UIROASGameInstance>(GetGameInstance());
    if (tempGI != nullptr)  {
        gi = tempGI;
        //mapInfo = gi->customMapInfo;
    }
    
    /* ���⼭ �ʿ� ���� ��� �ּ� ��� �߰�
    FStreamableManager StreamableManager;
   // StreamableManager
    TArray<FSoftObjectPath> AssetReferences;

    AssetReferences.Add(FSoftObjectPath(TEXT("/Game/YSL/Blueprints/BP_Melee1Spawner")));
    AssetReferences.Add(FSoftObjectPath(TEXT("/Game/YSL/Blueprints/BP_RangedGroundSpawner")));
    
    StreamableManager.RequestAsyncLoad(AssetReferences, FStreamableDelegate::CreateUObject(this, &ABattleGameModeBase::OnLoadComplete));
    */

    //UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), Enemys);
    //EnemyCnt= Enemys.Num();

    // ���� ���� �� ���ʹ� ���� üũ
    TArray<AActor*> FoundEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), FoundEnemies);
    
    /////////
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), FoundSpawners);
    for (AActor* Actor : FoundSpawners)
    {
        AEnemySpawner* Spawner = Cast<AEnemySpawner>(Actor);
        if (Spawner)
        {
            TotalEnemyCount += Spawner->spawnMaxNum;
            spawners.Add(Spawner);
        }
    }
    // maxnum���� ����
    CurrentEnemyCount = TotalEnemyCount;
    // 1�ʸ��� ���ʹ� �� üũ
    //GetWorld()->GetTimerManager().SetTimer(CheckEnemyTimerHandle, this, &ABattleGameModeBase::SpawnBoss, 1.0f, true);
   


    // ���� ���� �ó׸�ƽ �÷���
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelSequenceActor::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        //MyLevelSequenceActor = Cast<ALevelSequenceActor>(FoundActors[0]);
        for (auto Actor : FoundActors)
        {
            if (Actor && Actor->Tags.Contains(FName("BossCine")))
            {
                MyLevelSequenceActor = Cast<ALevelSequenceActor>(Actor);
            }
        }
    }


}

void ABattleGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void ABattleGameModeBase::OnLoadComplete()
{
    
}

void ABattleGameModeBase::SpawnBoss()
{
    //GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("TotalEnemyCount = %d CurrentEnemyCount = %d "), TotalEnemyCount, CurrentEnemyCount));

    // ���� ���� �ڵ� (��ġ, �����̼�, ���� �Ķ���� ���� �����ؾ� �մϴ�.)
   // if (CurrentEnemyCount <= TotalEnemyCount/2)
    //GetWorld()->GetTimerManager().ClearTimer(CheckEnemyTimerHandle);

    // ������ �÷��̽�Ű��
    if (MyLevelSequenceActor)
    {
        ULevelSequencePlayer* SequencePlayer = MyLevelSequenceActor->GetSequencePlayer();
        if (SequencePlayer)
        {
            SequencePlayer->Play();
            SequencePlayer->OnFinished.AddDynamic(this, &ABattleGameModeBase::OnLevelSequenceFinished);
        }
    }

  
}

void ABattleGameModeBase::OnLevelSequenceFinished()
{
    //���� �������� ������ 
    UWorld* World = GetWorld();
    if (World && BP_BossClass)
    {
        // ��ġ �� ȸ�� ���� (X=1971.553983,Y=40069.846061,Z=0.000000)
        FVector SpawnLocation = FVector(1971.553983f, 40069.846061f, -60.f);
        FRotator SpawnRotation = FRotator(0.0f, -92.528514f, 0.0f);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        // Boss ����
        ABoss* SpawnedBoss = Cast<ABoss>(World->SpawnActor<AActor>(BP_BossClass, SpawnLocation, SpawnRotation, SpawnParams));
        if (SpawnedBoss != nullptr) {
            bossRef = SpawnedBoss;
            bossRef->FindTarget();
            bossRef->BossFSM->mBossState = EEnemyState::Attack;
            bossRef->BossAnim->BossAnimState = EEnemyState::Attack;
            onBossSpawned.Broadcast();
        }
    }

}
