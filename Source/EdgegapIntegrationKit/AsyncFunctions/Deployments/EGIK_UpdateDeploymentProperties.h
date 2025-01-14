﻿// Copyright (c) 2024 Betide Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EGIKBlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "EGIK_UpdateDeploymentProperties.generated.h"

USTRUCT(BlueprintType)
struct FEGIK_UpdateDeploymentPropertiesRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Edgegap Integration Kit | Deployment")
	FString RequestId;

	UPROPERTY(BlueprintReadWrite, Category = "Edgegap Integration Kit | Deployment")
	bool bIsJoinableBySession = false;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateDeploymentPropertiesResponse, bool, bIsJoinableBySession, FEGIK_ErrorStruct, Error);
UCLASS()
class EDGEGAPINTEGRATIONKIT_API UEGIK_UpdateDeploymentProperties : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Edgegap Integration Kit | Deployment")
	static UEGIK_UpdateDeploymentProperties* UpdateDeploymentProperties(FEGIK_UpdateDeploymentPropertiesRequest Request);

	void OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg);
	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable, Category = "Edgegap Integration Kit | Deployment")
	FUpdateDeploymentPropertiesResponse OnSuccess;

	UPROPERTY(BlueprintAssignable, Category = "Edgegap Integration Kit | Deployment")
	FUpdateDeploymentPropertiesResponse OnFailure;

private:
	FEGIK_UpdateDeploymentPropertiesRequest Var_Request;
};
