﻿// Copyright (c) 2024 Betide Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "EGIKBlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "EGIK_GetLocationBeacons.generated.h"

USTRUCT(BlueprintType)
struct FEGIK_LocationBeaconStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Edgegap Integration Kit | Location")
	FString FQDN;

	UPROPERTY(BlueprintReadWrite, Category = "Edgegap Integration Kit | Location")
	FString PublicIP;

	UPROPERTY(BlueprintReadWrite, Category = "Edgegap Integration Kit | Location")
	int32 TCP_Port = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Edgegap Integration Kit | Location")
	int32 UDP_Port = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Edgegap Integration Kit | Location")
	FEGIK_LocationStruct Location;
};

USTRUCT(BlueprintType)
struct FEGIK_LocationBeaconResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Edgegap Integration Kit | Location")
	int32 Count = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Edgegap Integration Kit | Location")
	TArray<FEGIK_LocationBeaconStruct> Beacons;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGetLocationBeaconsResponse, const FEGIK_LocationBeaconResponse&, Response, const FEGIK_ErrorStruct&, Error);

UCLASS()
class EDGEGAPINTEGRATIONKIT_API UEGIK_GetLocationBeacons : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Edgegap Integration Kit | Location")
	static UEGIK_GetLocationBeacons* GetLocationBeacons(FString MatchmakingUrl, FString AuthToken);

	void OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg);
	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable, Category = "Edgegap Integration Kit | Location")
	FGetLocationBeaconsResponse OnSuccess;

	UPROPERTY(BlueprintAssignable, Category = "Edgegap Integration Kit | Location")
	FGetLocationBeaconsResponse OnFailure;

private:
	FString Var_MatchmakingUrl;
	FString Var_AuthToken;
};
