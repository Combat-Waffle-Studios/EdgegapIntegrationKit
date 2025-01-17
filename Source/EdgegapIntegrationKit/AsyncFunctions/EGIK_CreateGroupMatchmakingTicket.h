// Copyright (c) 2025 Combat Waffle Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "EGIKBlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "EGIK_CreateGroupMatchmakingTicket.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCreateGroupMatchmakingTicketResponse, const FEGIK_MatchmakingResponse&, Response, const FEGIK_ErrorStruct&, Error);

UCLASS()
class EDGEGAPINTEGRATIONKIT_API UEGIK_CreateGroupMatchmakingTicket : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Edgegap Integration Kit | Matchmaking")
	static UEGIK_CreateGroupMatchmakingTicket* CreateGroupMatchmakingTicket(const FGOT_CreateGroupMatchmakingStruct& MatchmakingStruct);

	void OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg);
	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable, Category = "Edgegap Integration Kit | Matchmaking")
	FCreateGroupMatchmakingTicketResponse OnSuccess;

	UPROPERTY(BlueprintAssignable, Category = "Edgegap Integration Kit | Matchmaking")
	FCreateGroupMatchmakingTicketResponse OnFailure;

private:
	FGOT_CreateGroupMatchmakingStruct Var_MatchmakingStruct;
};
