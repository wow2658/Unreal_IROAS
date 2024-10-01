
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
	// ����ü����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FMapInfoTable mapInfo;

	// ��ġ�� EditableActor��
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
	// �����ּ�
	FString baseURL = "http://52.79.89.164";
	// �� URL
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

	//����뺯����
	bool tested = false;
	
};
