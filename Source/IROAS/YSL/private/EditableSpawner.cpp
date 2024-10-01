

#include "EditableSpawner.h"

AEditableSpawner::AEditableSpawner()
{
	isProp = false;
}

void AEditableSpawner::BeginPlay()
{
	Super::BeginPlay();

}

void AEditableSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEditableSpawner::SetPlayerPtr()
{
	editor->SetEditableSpawner(this);
}
