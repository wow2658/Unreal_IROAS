
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EditableActor.generated.h"

UCLASS()
class IROAS_API AEditableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AEditableActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	class USphereComponent* limitedRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	class USceneComponent* transformComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	class UStaticMeshComponent* spawnableStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	class USkeletalMeshComponent* spawnableSkeletalMesh;

	void CheckSpawn();
	void ShowRedBoundary(bool spawn);
	virtual void SetPlayerPtr() {}

	void CalculateRotationAngle();

	bool cantSpawn = false;
	float rotationAngle = 0.0f;
	class AEditorPlayer* editor;
	float stencilValue = 1.0f;

// DVNÃß°¡
public:
	bool isProp = false;
};
