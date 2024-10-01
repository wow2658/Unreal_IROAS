

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPlayerStatus : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Dead UMETA(DisplayName = "Dead"),
	Stun UMETA(DisplayName = "Stun"),
	Blind UMETA(DisplayName = "Blind")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None UMETA(DisplayName = "None"),
	Heal UMETA(DisplayName = "Heal"),
	Energy UMETA(DisplayName = "Energy"),
	IceElement UMETA(DisplayName = "IceElement")
};

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Melee UMETA(DisplayName = "Melee"),
	Ranged_Ground UMETA(DisplayName = "Ranged_Ground"),
	Ranged_Air UMETA(DisplayName = "Ranged_Air"),
	Elite UMETA(DisplayName = "Elite"),
	None UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Dead UMETA(DisplayName = "Dead"),
	Move UMETA(DisplayName = "Move"),
	Attack UMETA(DisplayName = "Attack"),
	Damage UMETA(DisplayName = "Damage"),

	Backstep UMETA(DisplayName = "Backstep"), //none
	Sidestep UMETA(DisplayName = "Sidestep"), //none
	JumpAttack UMETA(DisplayName = "JumpAttack"), //none

	Blink UMETA(DisplayName = "Blink"), //boss
	Groggy UMETA(DisplayName = "Groggy"), //boss

	ThrowRock UMETA(DisplayName = "ThrowRock"), //bossSK1
	BossSlash UMETA(DisplayName = "BossSlash"), //bossSK2
	Rour UMETA(DisplayName = "Rour"), //bossSK3
	TeleKinesiss UMETA(DisplayName = "TeleKinesiss"), //bossSK4

	Rage UMETA(DisplayName = "Rage"),

	BossIntro UMETA(DisplayName = "BossIntro"), 
	BossDead UMETA(DisplayName = "BossDead"),
	PlayerDie UMETA(DisplayName = "PlayerDie"),

};

UENUM(BlueprintType)
enum class EPlayerHandType : uint8
{
	Left,
	Right,
};

UENUM(BlueprintType)
enum class EGrabType : uint8
{
	SnapObjToHand,
	SnapHandToObj,
	Custom,
};

UENUM(BlueprintType)
enum class EGrabHandType : uint8
{
	Left,
	Right,
	Both
};

UENUM(BlueprintType)
enum class EHandSnapTypeOnOverlap : uint8
{
	SnapHandToObjAndSim,
	JustSimFingers,
	NoSnapNoSim,
};

UENUM(BlueprintType)
enum class EHandState : uint8 
{
	Default,
	PointFinger,
	UnfoldPalm,
};

UENUM(BlueprintType)
enum class ETimeZone : uint8
{
	None UMETA(DisplayName = "None"),
	Morning UMETA(DisplayName = "Morning"),
	Noon UMETA(DisplayName = "Noon"),
	Evening UMETA(DisplayName = "Evening")
};

UENUM(BlueprintType)
enum class EStaticMeshType : uint8
{
	M_MainMenu,
	M_PropCategory,
	M_EnemyProperty,
	In_Spawner,
	In_Building,
	In_Construction,
	In_Prop,
	In_Foliage,
	In_TimeZone,
	In_Passthrough
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None UMETA(DisplayName = "None"),
	Gun UMETA(DisplayName = "Gun"),
	Sword UMETA(DisplayName = "Sword")
};

UENUM(BlueprintType)
enum class EIndicatorType : uint8
{
	HealthHeal,
	EnergyHeal,
	Damage,
	LeftMove,
	RightMove,
	Default
};

UENUM(BlueprintType)
enum class EGameModeType : uint8
{
	None UMETA(DisplayName = "None"),
	StoryMode UMETA(DisplayName = "StoryMode"),
	EditorMode UMETA(DisplayName = "EditorMode"),
	LobbyMode UMETA(DisplayName = "LobbyMode"),
	CustomMode UMETA(DisplayName = "CustomMode")
};

UENUM(BlueprintType)
enum class ESpawnerState : uint8
{
	Inactive UMETA(DisplayName = "Inactive"),
	WithShield UMETA(DisplayName = "WithShield"),
	NoShield UMETA(DisplayName = "NoShield"),
	Destroyed UMETA(DisplayName = "Destroyed")
};


UENUM(BlueprintType)
enum class ETutorialStep : uint8
{
	S1_RigtMove,
	S2_MiddleMove,
	S3_Rotate,
	S4_GunSpawn,
	S5_GunAttack,
	S6_SwordSpawn,
	S7_SwordAttack,
	S8_SwordGetEnergy,
	S9_GunReload,
	S10_SwordSlash,
	S11_Default
};


class IROAS_API IROASEnums
{
public:
	IROASEnums();
	~IROASEnums();
};
