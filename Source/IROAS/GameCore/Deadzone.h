
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Deadzone.generated.h"

class UBoxComponent;

UCLASS()
class IROAS_API ADeadzone : public AActor
{
	GENERATED_BODY()
	
public:	
	ADeadzone();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* boxComp;

public:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
