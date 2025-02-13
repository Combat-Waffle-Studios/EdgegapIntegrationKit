﻿// Copyright (c) 2024 Betide Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EGIKBlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "EGIK_DeleteRelaySession.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeleteRelaySession, FEGIK_ErrorStruct, Error);
UCLASS()
class EDGEGAPINTEGRATIONKIT_API UEGIK_DeleteRelaySession : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Edgegap Integration Kit | Relay Sessions", meta = (BlueprintInternalUseOnly = "true"))
	static UEGIK_DeleteRelaySession* DeleteRelaySession(FString SessionId);

	virtual void Activate() override;
	void OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg);

	UPROPERTY(BlueprintAssignable, Category = "Edgegap Integration Kit | Relay Sessions")
	FOnDeleteRelaySession OnSuccess;

	UPROPERTY(BlueprintAssignable, Category = "Edgegap Integration Kit | Relay Sessions")
	FOnDeleteRelaySession OnFailure;

private:
	FString Var_SessionId;
};
