﻿// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "EGIK_GetLocationBeacons.h"

UEGIK_GetLocationBeacons* UEGIK_GetLocationBeacons::GetLocationBeacons(FString MatchmakingUrl, FString AuthToken)
{
	UEGIK_GetLocationBeacons* BlueprintNode = NewObject<UEGIK_GetLocationBeacons>();
	BlueprintNode->Var_MatchmakingUrl = MatchmakingUrl;
	BlueprintNode->Var_AuthToken = AuthToken;
	return BlueprintNode;
}

void UEGIK_GetLocationBeacons::OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,	TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg)
{
	FEGIK_LocationBeaconResponse Response;
	if(HttpResponse.IsValid())
	{
		if(EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
			if(FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				Response.Count = JsonObject->GetIntegerField(TEXT("count"));
				TArray<TSharedPtr<FJsonValue>> BeaconsArray = JsonObject->GetArrayField(TEXT("beacons"));
				for (TSharedPtr<FJsonValue> BeaconValue : BeaconsArray)
				{
					TSharedPtr<FJsonObject> BeaconObject = BeaconValue->AsObject();
					FEGIK_LocationBeaconStruct Beacon;
					Beacon.FQDN = BeaconObject->GetStringField(TEXT("fqdn"));
					Beacon.PublicIP = BeaconObject->GetStringField(TEXT("public_ip"));
					Beacon.TCP_Port = BeaconObject->GetIntegerField(TEXT("tcp_port"));
					Beacon.UDP_Port = BeaconObject->GetIntegerField(TEXT("udp_port"));
					TSharedPtr<FJsonObject> LocationObject = BeaconObject->GetObjectField(TEXT("location"));
					Beacon.Location.City = LocationObject->GetStringField(TEXT("city"));
					Beacon.Location.Country = LocationObject->GetStringField(TEXT("country"));
					Beacon.Location.Continent = LocationObject->GetStringField(TEXT("continent"));
					Beacon.Location.AdministrativeDivision = LocationObject->GetStringField(TEXT("administrative_division"));
					Beacon.Location.Timezone = LocationObject->GetStringField(TEXT("timezone"));
					Response.Beacons.Add(Beacon);
				}
				OnSuccess.Broadcast(Response, FEGIK_ErrorStruct());
			}
			else
			{
				OnFailure.Broadcast(FEGIK_LocationBeaconResponse(), FEGIK_ErrorStruct(0, "Failed to parse JSON"));
			}
		}
		else
		{
			OnFailure.Broadcast(FEGIK_LocationBeaconResponse(), FEGIK_ErrorStruct(HttpResponse->GetResponseCode(), HttpResponse->GetContentAsString()));
		}
	}
	else
	{
		OnFailure.Broadcast(FEGIK_LocationBeaconResponse(), FEGIK_ErrorStruct(0, "Failed to process request"));
	}
	SetReadyToDestroy();
	MarkPendingKill();
}

void UEGIK_GetLocationBeacons::Activate()
{
	Super::Activate();
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetVerb("GET");
	Request->SetURL(Var_MatchmakingUrl + "/locations/beacons");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", Var_AuthToken);
	Request->OnProcessRequestComplete().BindUObject(this, &UEGIK_GetLocationBeacons::OnResponseReceived);
	if(!Request->ProcessRequest())
	{
		OnFailure.Broadcast(FEGIK_LocationBeaconResponse(), FEGIK_ErrorStruct(0, "Failed to process request"));
		SetReadyToDestroy();
		MarkPendingKill();
	}
}
