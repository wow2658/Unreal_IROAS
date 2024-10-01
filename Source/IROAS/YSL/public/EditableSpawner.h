
#pragma once

#include "CoreMinimal.h"
#include "EditableActor.h"
#include "IROASEnums.h"
#include "IROASStructs.h"
#include "EditableSpawner.generated.h"

UCLASS()
class IROAS_API AEditableSpawner : public AEditableActor
{
	GENERATED_BODY()

public:
	AEditableSpawner();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEnemyType enemyType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSpawnerInfoTable spawnerInfo;

	int32 spawnAmount = 0;
	float spawnInterval;
	float spawnedEnemyHP;
	float spawnedEnemyPower;
	float spawnStartDelay;

	virtual void SetPlayerPtr() override;

};
