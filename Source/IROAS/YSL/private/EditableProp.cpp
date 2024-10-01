

#include "EditableProp.h"
#include "EditableActor.h"
#include "EditorPlayer.h"

AEditableProp::AEditableProp()
{
	rotateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotation Mesh"));
	rotateMesh->SetupAttachment(transformComp);
	rotateMesh->SetVisibility(false);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Game/YSL/UI/EditorPlayer/3DButton/rotate"));
	if (tempMesh.Succeeded()) {
		rotateMesh->SetStaticMesh(tempMesh.Object);
	}
	isProp = true;
}

void AEditableProp::BeginPlay()
{
	Super::BeginPlay();

	

}

void AEditableProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEditableProp::SetPlayerPtr()
{
	editor->SetEditableProp(this);
}

void AEditableProp::ShowRotateUI(bool canShow)
{
	rotateMesh->SetVisibility(canShow);
}
