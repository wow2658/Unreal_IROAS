// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyTutorialOrb.h"
#include <Components/BoxComponent.h>
#include <Components/AudioComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Curves/CurveVector.h>
#include "EnergyBullet.h"
#include "Sword.h"
#include "LobbyTutorialActor.h"
#include "Components/WidgetComponent.h"
#include "SlashProjectile.h"
#include "BattlePlayer.h"
#include "BattlePlayerWidget.h"
#include <UMG/Public/Components/ProgressBar.h>
#include <UMG/Public/Components/TextBlock.h>

ALobbyTutorialOrb::ALobbyTutorialOrb()
{
	PrimaryActorTick.bCanEverTick = true;
	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(rootComp);
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Button"));
	boxComp->SetupAttachment(RootComponent);
	boxComp->SetBoxExtent(FVector(130));
	boxComp->SetCollisionProfileName(FName("EnemyBody"));
	niagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraFX"));
	niagaraComp->SetupAttachment(boxComp);
	//niagaraComp->bAutoActivate = false;
	niagaraComp->SetVisibility(false);
	niagaraComp->SetWorldScale3D(FVector(0));
	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	audioComp->SetupAttachment(boxComp);
	audioComp->Stop();

	floatingCurve1 = nullptr;
	floatingTimeLineLength = 3.0f;
}

void ALobbyTutorialOrb::BeginPlay()
{
	Super::BeginPlay();
	
	player = Cast<ABattlePlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	CurveSetting();

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ALobbyTutorialOrb::OnOrbOverlap);
}

void ALobbyTutorialOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	curtime += DeltaTime;
	if (curtime >= floatingStartTime)
		floatingTimeLine.TickTimeline(DeltaTime);

	GEngine->AddOnScreenDebugMessage(4, 3.0f, FColor::Red, FString::Printf(TEXT("%d"), Tags.Num()));
}

void ALobbyTutorialOrb::OnOrbBegin()
{
	niagaraComp->SetVisibility(true);
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	boxComp->SetCollisionObjectType(ECC_GameTraceChannel3);
	boxComp->bHiddenInGame = true;

	if (appearSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), appearSound, GetActorLocation());
	}

	audioComp->Activate(true);
	audioComp->FadeIn(0.8f);
}

void ALobbyTutorialOrb::SetOrbScale(float scale)
{
	/*if (niagaraComp->IsVisible()) {
		scaleTime = FMath::Clamp(scaleTime, 0.f, 0.5f);
		float interpScale = FMath::Lerp(0.f, scale, scaleTime / 0.5f);
		niagaraComp->SetWorldScale3D(FVector(interpScale));
	}*/
	//niagaraComp->SetWorldScale3D(FVector(scale));
	boxComp->SetWorldScale3D(FVector(scale));
}

void ALobbyTutorialOrb::OnOrbDestroyed()
{
	niagaraComp->SetVisibility(false);
	//niagaraComp->SetWorldScale3D(FVector(0));
	if (destroyFX != nullptr) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), destroyFX, GetActorLocation());
	}
	if (destroySound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), destroySound, GetActorLocation());
	}
	boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	audioComp->Stop();
	SetActorLocation(FVector(0, -10000, -1000));

	if (ActorHasTag("GunOrbs"))
		widgetActor->tutorialOrbPool_Gun.Enqueue(this);
	else if (ActorHasTag("SwordOrbs"))
		widgetActor->tutorialOrbPool_Sword.Enqueue(this);
	else if (ActorHasTag("SwordEnergyOrbs")) {
		widgetActor->tutorialOrbPool_SwordEnergy.Enqueue(this);
		newPercent += 10.f;
		if (player != nullptr)
			player->GainEnergy(newPercent);
	}
	else if (ActorHasTag("SwordSlashOrbs")) {
		UGameplayStatics::PlaySound2D(GetWorld(), appearSound);
		widgetActor->widgetComp1->SetVisibility(false);
		widgetActor->widgetComp2->SetVisibility(false);
		widgetActor->widgetComp3->SetVisibility(false);
		widgetActor->SpawnStartOrb();
	}

	if (widgetActor->orbEnergyDestroyedCount == 2 && ActorHasTag("SwordEnergyOrbs")) {
		widgetActor->ClearCurrentStep();
		widgetActor->orbEnergyDestroyedCount = 0;
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("sword : %d"), widgetActor->orbDestroyedCount));
	} 

	if (widgetActor->orbDestroyedCount == 3) {
		if (ActorHasTag("GunOrbs") || ActorHasTag("SwordOrbs")) {
			widgetActor->ClearCurrentStep(); 
			widgetActor->orbDestroyedCount = 0;
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("gun : %d"), widgetActor->orbDestroyedCount));
		}
	}
}

void ALobbyTutorialOrb::OnOrbOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorHasTag("GunOrbs")) {
		AEnergyBullet* bullet = Cast<AEnergyBullet>(OtherActor);
		if (bullet != nullptr) {
			widgetActor->orbDestroyedCount++;
			OnOrbDestroyed();
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("bulletoverlap")));
		}
	}
	else {
		AEnergyBullet* bullet = Cast<AEnergyBullet>(OtherActor);
		if (bullet != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), bulletClashSound, SweepResult.ImpactPoint);
			FVector inversedDir = bullet->GetActorForwardVector() * (-1);
			bullet->SetActorRotation(inversedDir.Rotation());
			return;
		}
		else {
			ASword* sword = Cast<ASword>(OtherActor);
			if (sword != nullptr) {
				if (ActorHasTag("SwordOrbs")) {
					widgetActor->orbDestroyedCount++;
					OnOrbDestroyed();
				}
				else if (ActorHasTag("SwordEnergyOrbs")) {
					widgetActor->orbEnergyDestroyedCount++;
					OnOrbDestroyed();
				}
			}
			else {
				if (!ActorHasTag("SwordSlashOrbs")) return;

				ASlashProjectile* swordSlash = Cast<ASlashProjectile>(OtherActor);
				if (swordSlash != nullptr) {
					widgetActor->tutorialOrb_Sword = this;
					OnOrbDestroyed();
				}
			}
		}
	}	
}


void ALobbyTutorialOrb::CurveSetting()
{
	if (floatingCurve1 != nullptr) {
		// VectorCurve의 Z축에 대한 키 값 랜덤 변경
		/*floatingCurve1->FloatCurves[2].Keys[1].Value = FMath::RandRange(0.5f, 0.9f);
		floatingCurve1->FloatCurves[2].Keys[2].Value = FMath::RandRange(-0.9f, -0.5f);*/

		FOnTimelineVector onProgressFloating;
		onProgressFloating.BindUFunction(this, FName("FloatingCurveStart"));
		FOnTimelineEvent onFinishedFloating;
		onFinishedFloating.BindUFunction(this, FName("FloatingCurveEnd"));


		floatingTimeLine.AddInterpVector(floatingCurve1, onProgressFloating);
		floatingTimeLine.SetTimelineFinishedFunc(onFinishedFloating);
		floatingTimeLine.SetTimelineLength(floatingTimeLineLength);
		floatingTimeLine.SetLooping(true);
	}
	floatingTimeLine.PlayFromStart();
}

void ALobbyTutorialOrb::FloatingCurveStart(FVector value)
{
	boxComp->AddLocalOffset(value);
}

void ALobbyTutorialOrb::FloatingCurveEnd()
{
	bInFloating = false;
}

