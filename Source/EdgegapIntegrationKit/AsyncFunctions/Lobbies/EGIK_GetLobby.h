﻿// Copyright (c) 2024 Betide Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EGIKBlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "EGIK_GetLobby.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEGIK_GetLobbyResponse, FEGIK_LobbyInfo, Result, FEGIK_ErrorStruct, Error);
UCLASS()
class EDGEGAPINTEGRATIONKIT_API UEGIK_GetLobby : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Edgegap Integration Kit | Lobbies")
	static UEGIK_GetLobby* GetLobby(FString LobbyName);

	virtual void Activate() override;
	void OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg);

	UPROPERTY(BlueprintAssignable, Category = "Edgegap Integration Kit | Lobbies")
	FEGIK_GetLobbyResponse OnSuccess;

	UPROPERTY(BlueprintAssignable, Category = "Edgegap Integration Kit | Lobbies")
	FEGIK_GetLobbyResponse OnFailure;
	
private:
	FString Var_LobbyName;
};
