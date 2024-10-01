
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PlayerHand.h"
#include "IROASEnums.h"
#include "GrabComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IROAS_API UGrabComp : public USceneComponent
{
	GENERATED_BODY()

public:	
	UGrabComp();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	APlayerHand* currentGrabbedHand = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float grabAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	EGrabType grabType = EGrabType::Custom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	EHandSnapTypeOnOverlap snapTypeOnOverlap = EHandSnapTypeOnOverlap::NoSnapNoSim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	EGrabHandType grabHandType = EGrabHandType::Both;

	bool grabbed = false;
	bool simulateOnDrop = false;
	APlayerHand* vrhandRef = nullptr;
	USceneComponent* grabSocket = nullptr;

public:
	UFUNCTION(BlueprintCallable)
	bool CanGrab(APlayerHand* vrhand, USceneComponent* _grabSocket);
	UFUNCTION(BlueprintCallable)
	bool TryGrab(APlayerHand* vrhand, USceneComponent* _grabSocket);
	UFUNCTION(BlueprintCallable)
	bool TryRelease();

	void SetGrabType(EGrabType _grabType) { grabType = _grabType; }
	void SetGrabHandType(EGrabHandType _grabHandType) { grabHandType = _grabHandType; }
	void SetGrabAngle(float angle) { grabAngle = angle; }

	EGrabType GetGrabType() { return grabType; }
	EGrabHandType GetGrabHandType() { return grabHandType; }
	EHandSnapTypeOnOverlap GetHandSnapTypeOnOverlap() { return snapTypeOnOverlap; }
	bool IsGrabbed() { return grabbed; }
	USceneComponent* GetGrabSocket() { return grabSocket; }
	APlayerHand* GetGrabbedHand() { return vrhandRef; }

private:
	void SetShouldSimulateOnDrop();
	void SetPrimitiveCompPhysics(bool simulate);
	bool AttachParentToMotionController();
	void DetachParentFromMotionController();
	void OrientHeldActor();



};
