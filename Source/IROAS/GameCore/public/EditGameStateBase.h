
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "IROASEnums.h"
#include "IROASStructs.h"
#include "JsonParseLibrary.h"
#include "EditGameStateBase.generated.h"

class AEditableActor;
class AEditableSpawner;
class AEditableProp;
class AMovableFrame;
class UIROASGameInstance;
class AHttpRequestActor;

UCLASS()
class IROAS_API AEditGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AEditGameStateBase();

protected:
	virtual void BeginPlay() override;

public: 
	virtual void Tick( float DeltaSeconds ) override;
public:
	// HTTP RequestActor
	UPROPERTY()
	AHttpRequestActor* reqActor = nullptr;
	// 맵전체정보
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FMapInfoTable mapInfo;

	// 배치된 EditableActor들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AEditableSpawner*> spawnerActors;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AEditableProp*> propActors;



	ETimeZone timezone = ETimeZone::Noon;

	UPROPERTY()
	AEditableActor* playerStart = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEditableActor> bp_playerStart;


	FOnActorSpawned::FDelegate actorSpawnedDelegate;
	FOnActorDestroyed::FDelegate actorDestroyedDelegate;


	FString fullPath;
	FString contentPath = FPaths::ProjectContentDir();

	

private:
	// 서버주소
	FString baseURL = "http://52.79.89.164";
	// 맵 URL
	FString mapURL = "/api/v1/maps";
	

	UIROASGameInstance* gi = nullptr;

public:
	UFUNCTION()
	void HandleActorSpawned(AActor* actor);

	UFUNCTION()
	void HandleActorDestroyed(AActor* actor);

	UFUNCTION()
	void OnPlayerStartUpdate();

	UFUNCTION()
	void OnTimezoneUpdate();

	UFUNCTION()
	void OnSpawnerInfoUpdate(AEditableSpawner* updatedSpawner);

	UFUNCTION()
	void OnPropInfoUpdate(AEditableProp* updatedProp);

	UFUNCTION()
	void OrganizeSpawnerInfo();

	UFUNCTION()
	void OrganizePropInfo();


	UFUNCTION()
	void SaveMapData();

	UFUNCTION() 
	void UploadMapData();

	UFUNCTION()
	void OnDestroyed(AActor* DestroyedActor);

	//실험용변수들
	bool tested = false;
	
};
