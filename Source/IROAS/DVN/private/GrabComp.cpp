
#include "GrabComp.h"
#include <Kismet/KismetMathLibrary.h>

UGrabComp::UGrabComp()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UGrabComp::BeginPlay()
{
	Super::BeginPlay();


}


void UGrabComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UGrabComp::CanGrab(APlayerHand* vrhand, USceneComponent* _grabSocket)
{
	// ����ڵ�
	if (vrhand == nullptr || _grabSocket == nullptr) return false;

	// ��/���� Ȯ��
	bool rightHand = true;
	if (vrhand->GetIsRight() == false) rightHand = false;

	// �޼ո� �������ִ� �����ε� ���������� �������°��� Ȯ��
	if (grabHandType == EGrabHandType::Left && rightHand) return false;
	// ���� �ݴ�
	if (grabHandType == EGrabHandType::Right && !rightHand) return false;


	// grabAngle: ������ �ִ� �������� Ȯ��. --> ���� �ʿ�����Ű��� ���.
	/*FVector v1 = _grabSocket->GetUpVector();
	FVector v2 = GetUpVector();

	double angle = UKismetMathLibrary::RadiansToDegrees(UKismetMathLibrary::Acos(v1.Dot(v2)));

	if (angle > grabAngle) {
		return false;
	}*/

	return true;
}

bool UGrabComp::TryGrab(APlayerHand* vrhand, USceneComponent* _grabSocket)
{
	if (CanGrab(vrhand, _grabSocket) == false) return false;

	vrhandRef = vrhand;
	grabSocket = _grabSocket;

	/*if (AVRTechGrabBase* owner = Cast<AVRTechGrabBase>(GetOwner()))
		owner->OnPreGrabbed_Internal(this);*/

	switch (grabType)
	{
	case EGrabType::SnapObjToHand:
		SetPrimitiveCompPhysics(false);
		AttachParentToMotionController();
		OrientHeldActor();
		break;
	case EGrabType::SnapHandToObj:
		SetPrimitiveCompPhysics(false);
		break;
	case EGrabType::Custom:
		break;
	}

	grabbed = true;

	if (grabbed == false)
	{
		vrhandRef = nullptr;
		grabSocket = nullptr;
		vrhandRef->SetAttachedActor(nullptr);
	}
	else
	{
		/*if (AVRTechGrabBase* owner = Cast<AVRTechGrabBase>(GetOwner()))
		{
			owner->OnGrabbed(this);
			_vrhandRef->SetAttachedActor(owner);

		}*/
	}

	// PlayHapticEffect

	return grabbed;
}

bool UGrabComp::TryRelease()
{
	grabbed = false;

	if (grabType != EGrabType::Custom)
	{
		if (simulateOnDrop)
			SetPrimitiveCompPhysics(true);
	}

	switch (grabType)
	{
	case EGrabType::SnapObjToHand:
		DetachParentFromMotionController();
		break;
	case EGrabType::SnapHandToObj:
		//DetachParentFromMotionController();
		break;
	case EGrabType::Custom:
		break;
	}

	vrhandRef = nullptr;
	grabSocket = nullptr;

	return grabbed == false;
}

void UGrabComp::SetShouldSimulateOnDrop()
{
	UPrimitiveComponent* component = Cast<UPrimitiveComponent>(GetAttachParent());
	if (component && component->IsAnySimulatingPhysics())
		simulateOnDrop = true;
}

void UGrabComp::SetPrimitiveCompPhysics(bool simulate)
{
	UPrimitiveComponent* component = Cast<UPrimitiveComponent>(GetAttachParent());
	if (component)
		component->SetSimulatePhysics(simulate);
}

bool UGrabComp::AttachParentToMotionController()
{
	if (grabSocket == nullptr)
		return false;

	FAttachmentTransformRules rules(EAttachmentRule::KeepWorld, true);
	return GetAttachParent()->AttachToComponent(grabSocket, rules);
}

void UGrabComp::DetachParentFromMotionController()
{
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, false);
	GetAttachParent()->DetachFromComponent(rules);
}

void UGrabComp::OrientHeldActor()
{
	if (grabSocket == nullptr)
		return;

	FRotator newRotation = GetRelativeRotation().GetInverse();
	GetAttachParent()->SetRelativeRotation(newRotation, false, nullptr, ETeleportType::TeleportPhysics);

	// Comp -> Parent
	FVector compToParentDirection = GetAttachParent()->GetComponentLocation() - GetComponentLocation();
	FVector newLocation = grabSocket->GetComponentLocation() + compToParentDirection;
	GetAttachParent()->SetWorldLocation(newLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

