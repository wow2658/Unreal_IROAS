// Fill out your copyright notice in the Description page of Project Settings.


#include "RageTriggerBox.h"
#include "MeleeEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequence/Public/LevelSequenceActor.h"



ARageTriggerBox::ARageTriggerBox()
{
    OnActorBeginOverlap.AddDynamic(this, &ARageTriggerBox::OnOverlap);
}

void ARageTriggerBox::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> foundCineActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelSequenceActor::StaticClass(), foundCineActors);

    for (AActor* Actor : foundCineActors)
    {
        if (Actor && Actor->Tags.Contains(startTag))
        {
            startCineActor = Cast<ALevelSequenceActor>(Actor);
        }
        else if (Actor && Actor->Tags.Contains(bossTag))
        {
            bossCineActor = Cast<ALevelSequenceActor>(Actor);
        }
    }

    if (startCineActor && bossCineActor)
    {
        ULevelSequencePlayer* startCinePlayer = startCineActor->GetSequencePlayer();
        ULevelSequencePlayer* bossCinePlayer = bossCineActor->GetSequencePlayer();
        if (startCinePlayer && bossCinePlayer)
        {
            //startCinePlayer->Play();
            startCinePlayer->OnFinished.AddDynamic(this, &ARageTriggerBox::OnstartCineFinished);
            bossCinePlayer->OnFinished.AddDynamic(this, &ARageTriggerBox::OnbossCineFinished);

            CineAutoPlay(startCineActor, startCineAutoPlay);
            CineAutoPlay(bossCineActor, bossCineAutoPlay);
        }
    }



}

void ARageTriggerBox::OnOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
    
    if (OtherActor && OtherActor != this)
    {
        AMeleeEnemy* MeleeEnemy = Cast<AMeleeEnemy>(OtherActor);
        if (MeleeEnemy)
        {
            MeleeEnemy->Tags.AddUnique("Rage");
        }
    }
    
}

void ARageTriggerBox::OnstartCineFinished()
{
    
}

void ARageTriggerBox::OnbossCineFinished()
{
    
}

void ARageTriggerBox::CineAutoPlay(ALevelSequenceActor* LevelSequenceActor, bool bAutoPlay)
{
    if (LevelSequenceActor)
    {
        ULevelSequencePlayer* Player = LevelSequenceActor->GetSequencePlayer();
        if (Player)
        {
            if (bAutoPlay)
            {
                Player->Play();  // 자동 재생이 활성화되면 재생 시작
            }
            else
            {
                Player->Stop();  // 자동 재생이 비활성화되면 재생 중지
            }
        }
    }
}