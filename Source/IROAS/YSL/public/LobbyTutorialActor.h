
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IROASEnums.h"
#include "LobbyTutorialActor.generated.h"

class USceneComponent;
class UBoxComponent;
class UWidgetComponent;
class ALobbyTutorialOrb;
class UNiagaraSystem;
class USoundBase;
class ULobbyTutorialWidget;
class ABattlePlayer;
class AMovableFrame;
class AGun;
class AShootableUIActor;
class UBattlePlayerWidget;

UCLASS()
class IROAS_API ALobbyTutorialActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALobbyTutorialActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Editanywhere, Category = MySettings)
	USceneComponent* rootComp;
	UPROPERTY(Editanywhere, Category = MySettings)
	UBoxComponent* boxComp;
	UPROPERTY(Editanywhere, Category = MySettings)
	UWidgetComponent* widgetComp1;
	UPROPERTY(Editanywhere, Category = MySettings)
	UWidgetComponent* widgetComp2;
	UPROPERTY(Editanywhere, Category = MySettings)
	UWidgetComponent* widgetComp3;

	ULobbyTutorialWidget* lobbyWidget1;
	ULobbyTutorialWidget* lobbyWidget2;
	ULobbyTutorialWidget* lobbyWidget3;
	AMovableFrame* movableFrame1;
	AMovableFrame* movableFrame2;
	AGun* gun1;
	AGun* gun2;

	ETutorialStep currentStep;

	ALobbyTutorialOrb* tutorialOrb_Gun;
	UPROPERTY(Editanywhere, Category = MySettings)
	TSubclassOf<ALobbyTutorialOrb> bp_tutorialOrb_Gun;
	TQueue<ALobbyTutorialOrb*> tutorialOrbPool_Gun;

	ALobbyTutorialOrb* tutorialOrb_Sword;
	UPROPERTY(Editanywhere, Category = MySettings)
	TSubclassOf<ALobbyTutorialOrb> bp_tutorialOrb_Sword;
	TQueue<ALobbyTutorialOrb*> tutorialOrbPool_Sword;

	ALobbyTutorialOrb* tutorialOrb_SwordEnergy;
	UPROPERTY(Editanywhere, Category = MySettings)
	TSubclassOf<ALobbyTutorialOrb> bp_tutorialOrb_SwordEnergy;
	TQueue<ALobbyTutorialOrb*> tutorialOrbPool_SwordEnergy;

	AShootableUIActor* startOrb;
	UPROPERTY(Editanywhere, Category = MySettings)
	TSubclassOf<AShootableUIActor> bp_startOrb;

	UPROPERTY(Editanywhere, Category = MySettings)
	USoundBase* clearSound;

	int8 gunSpawnCount = 0;
	int8 swordSpawnCount = 0;

	int8 orbDestroyedCount = 0;
	int8 orbEnergyDestroyedCount = 0;

	bool bThumbRight = false;
	bool bGunReload = false;
	bool bGunSpawn = false;
	bool bSwordSpawn = false;
	bool bSwordSlash = false;

	void ClearCurrentStep();
	void StartNextStep();
	void ThumbRightMove();
	void ThumbLeftMove();
	void ThumbRotate();
	void GunSpawn();
	void GunAttack();
	void SwordSpawn();
	void SwordAttack();
	void SwordGetEnergy();
	void GunReload();
	void SwordSlash();

	void SpawnStartOrb();
	void GetSpawnedGun();

	bool CheckRightRotate();
	bool CheckLeftRotate();


	UFUNCTION(BlueprintCallable)
	void DebuggingWidget();
	UFUNCTION(BlueprintCallable)
	void DebuggingAllOrbs();
	UFUNCTION(BlueprintCallable)
	void DebuggingButton();
	ABattlePlayer* lobbyPlayer;
	UBattlePlayerWidget* playerWidget;
};
