﻿// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "EGIK_GetRelaySession.h"

UEGIK_GetRelaySession* UEGIK_GetRelaySession::GetRelaySession(FString SessionId)
{
	UEGIK_GetRelaySession* Node = NewObject<UEGIK_GetRelaySession>();
	Node->Var_SessionId = SessionId;
	return Node;
}

void UEGIK_GetRelaySession::Activate()
{
	Super::Activate();
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetVerb("GET");
	Request->SetURL("https://api.edgegap.com/v1/relays/sessions/" + Var_SessionId);
	Request->SetHeader("Authorization", UEGIKBlueprintFunctionLibrary::GetAuthorizationKey());
	Request->SetHeader("Content-Type", "application/json");
	Request->OnProcessRequestComplete().BindUObject(this, &UEGIK_GetRelaySession::OnResponseReceived);
	if (!Request->ProcessRequest())
	{
		OnFailure.Broadcast(FEGIK_RelaySessionInfo(), FEGIK_ErrorStruct(0, "Failed to process request"));
		SetReadyToDestroy();
		MarkPendingKill();
	}
}

void UEGIK_GetRelaySession::OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
	TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg)
{
	if (HttpResponse.IsValid())
	{
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(HttpResponse->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				OnSuccess.Broadcast(JsonObject, FEGIK_ErrorStruct());
			}
			else
			{
				OnFailure.Broadcast(FEGIK_RelaySessionInfo(), FEGIK_ErrorStruct(0, "Failed to deserialize response"));
			}
		}
		else
		{
			OnFailure.Broadcast(FEGIK_RelaySessionInfo(), FEGIK_ErrorStruct(HttpResponse->GetResponseCode(), HttpResponse->GetContentAsString()));
		}
	}
	else
	{
		OnFailure.Broadcast(FEGIK_RelaySessionInfo(), FEGIK_ErrorStruct(0, "Failed to deserialize response"));
	}
	SetReadyToDestroy();
	MarkPendingKill();
}