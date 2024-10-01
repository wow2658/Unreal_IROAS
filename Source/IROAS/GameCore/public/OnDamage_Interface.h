// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OnDamage_Interface.generated.h"

UINTERFACE(MinimalAPI)
class UOnDamage_Interface : public UInterface
{
	GENERATED_BODY()
};

class IROAS_API IOnDamage_Interface
{
	GENERATED_BODY()

public:

    virtual void OnDamage_Interface(float DamageAmount) = 0;
};
