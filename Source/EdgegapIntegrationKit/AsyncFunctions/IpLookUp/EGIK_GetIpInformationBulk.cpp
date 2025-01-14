﻿// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "EGIK_GetIpInformationBulk.h"

UEGIK_GetIpInformationBulk* UEGIK_GetIpInformationBulk::GetIpInformationBulk(TArray<FString> IpAddresses)
{
	UEGIK_GetIpInformationBulk* Node = NewObject<UEGIK_GetIpInformationBulk>();
	Node->Var_IpAddresses = IpAddresses;
	return Node;
}

void UEGIK_GetIpInformationBulk::OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg)
{
	if (HttpResponse.IsValid())
	{
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			TArray<FEGIK_IpLookUpAddress> IpAddresses;
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				TArray<TSharedPtr<FJsonValue>> JsonArray = JsonObject->GetArrayField(TEXT("addresses"));
				for (TSharedPtr<FJsonValue> JsonValue : JsonArray)
				{
					TSharedPtr<FJsonObject> JsonObj = JsonValue->AsObject();
					FEGIK_IpLookUpAddress IpAddress;
					IpAddress.IP = JsonObj->GetStringField(TEXT("ip_address"));
					IpAddress.Type = JsonObj->GetStringField(TEXT("type"));
					IpAddress.Location.Continent.Code = JsonObj->GetObjectField(TEXT("location"))->GetObjectField(TEXT("continent"))->GetStringField(TEXT("code"));
					IpAddress.Location.Continent.Name = JsonObj->GetObjectField(TEXT("location"))->GetObjectField(TEXT("continent"))->GetStringField(TEXT("name"));
					IpAddress.Location.Country.Code = JsonObj->GetObjectField(TEXT("location"))->GetObjectField(TEXT("country"))->GetStringField(TEXT("code"));
					IpAddress.Location.Country.Name = JsonObj->GetObjectField(TEXT("location"))->GetObjectField(TEXT("country"))->GetStringField(TEXT("name"));
					IpAddress.Location.LatitudeLongitude.Latitude = JsonObj->GetObjectField(TEXT("location"))->GetNumberField(TEXT("latitude"));
					IpAddress.Location.LatitudeLongitude.Longitude = JsonObj->GetObjectField(TEXT("location"))->GetNumberField(TEXT("longitude"));
					IpAddresses.Add(IpAddress);
				}
				OnSuccess.Broadcast(IpAddresses, FEGIK_ErrorStruct());
			}
			else
			{
				OnFailure.Broadcast(TArray<FEGIK_IpLookUpAddress>(), FEGIK_ErrorStruct(0, "Failed to deserialize response"));
			}
		}
		else
		{
			OnFailure.Broadcast(TArray<FEGIK_IpLookUpAddress>(), FEGIK_ErrorStruct(HttpResponse->GetResponseCode(), "Failed to get ip information"));
		}
	}
	else
	{
		OnFailure.Broadcast(TArray<FEGIK_IpLookUpAddress>(), FEGIK_ErrorStruct(0, "Failed to deserialize response"));
	}
	SetReadyToDestroy();
	MarkPendingKill();
}

void UEGIK_GetIpInformationBulk::Activate()
{
	Super::Activate();
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetVerb("POST");
	Request->SetURL("https://api.edgegap.com/v1/ips/lookup");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", UEGIKBlueprintFunctionLibrary::GetAuthorizationKey());
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> IpAddressesArray;
	for (auto val : Var_IpAddresses)
	{
		IpAddressesArray.Add(MakeShareable(new FJsonValueString(val)));
	}
	JsonObject->SetArrayField("addresses", IpAddressesArray);
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(OutputString);
	Request->OnProcessRequestComplete().BindUObject(this, &UEGIK_GetIpInformationBulk::OnResponseReceived);
	if (!Request->ProcessRequest())
	{
		OnFailure.Broadcast(TArray<FEGIK_IpLookUpAddress>(), FEGIK_ErrorStruct(0, "Failed to process request"));
		SetReadyToDestroy();
		MarkPendingKill();
	}
}