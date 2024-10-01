

#include "IROASGameInstance.h"
#include "Engine/Engine.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemSteam.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"


UIROASGameInstance::UIROASGameInstance(const FObjectInitializer& ObjectInitializer)
{
	
}


void UIROASGameInstance::Init()
{
    Super::Init();
    /*
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    if (OnlineSub)
    {
        IOnlineIdentityPtr IdentityInterface = OnlineSub->GetIdentityInterface();
        IOnlineUserPtr UserInterface = OnlineSub->GetUserInterface();

        if (!IdentityInterface.IsValid()) {
            GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("No IdentityInterface")));
        }

        if (UserInterface.IsValid())
        {
            UserId = IdentityInterface->GetUniquePlayerId(0);
            TSharedPtr<FOnlineUser> User = UserInterface->GetUserInfo(0, *UserId);
            if (UserId.IsValid())
            {
                FString MyID = UserId->ToString();
                if (User.IsValid())
                {
                    // ½ºÆÀ ´Ð³×ÀÓÀ» °¡Á®¿È
                     userNickName = User->GetDisplayName();
                     GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Steam Nickname: %s"), *userNickName));
                }
                else GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("No User")));
            }
            else GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("No UserId")));
        }
        else GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("No UserInterface")));
    }
    else  GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("No OnlineSub")));
    UE_LOG(LogTemp, Warning, TEXT("GameInstance Init"));
    */
    /*STEAM_SUBSYSTEM*/
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    if (OnlineSub)
    {
        IOnlineIdentityPtr IdentityInterface = OnlineSub->GetIdentityInterface();
        IOnlineUserPtr UserInterface = OnlineSub->GetUserInterface();

        if (IdentityInterface.IsValid())
        {
            UserId = IdentityInterface->GetUniquePlayerId(0);
            if (UserId.IsValid())
            {
                MyID = UserId->ToString();
                //userNickName = MyID;
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Steam ID: %s"), *userNickName));
                }
            }
        }
    }
    else {
        bIsSteamId = false;
        OnlineSub = IOnlineSubsystem::Get();
        if (OnlineSub)
        {
            IOnlineIdentityPtr IdentityInterface = OnlineSub->GetIdentityInterface();
            IOnlineUserPtr UserInterface = OnlineSub->GetUserInterface();

            if (IdentityInterface.IsValid())
            {
                UserId = IdentityInterface->GetUniquePlayerId(0);
                if (UserId.IsValid())
                {
                    MyID = UserId->ToString();
                    //userNickName = MyID;
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Steam ID: %s"), *MyID));
                    }
                }
            }
        }
    }
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Init"));
    
}


void UIROASGameInstance::Join(const FString& Address)
{
	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

