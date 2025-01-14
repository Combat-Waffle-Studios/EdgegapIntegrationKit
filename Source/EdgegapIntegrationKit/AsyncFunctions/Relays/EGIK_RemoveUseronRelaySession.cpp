﻿// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "EGIK_RemoveUseronRelaySession.h"

UEGIK_RemoveUseronRelaySession* UEGIK_RemoveUseronRelaySession::RemoveUseronRelaySession(FString SessionId,
	FString AuthorizationToken)
{
	UEGIK_RemoveUseronRelaySession* Node = NewObject<UEGIK_RemoveUseronRelaySession>();
	Node->Var_SessionId = SessionId;
	Node->Var_AuthorizationToken = AuthorizationToken;
	return Node;
}

void UEGIK_RemoveUseronRelaySession::Activate()
{
	Super::Activate();
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetVerb("POST");
	Request->SetURL("https://api.edgegap.com/v1/relays/sessions:revoke-user");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", UEGIKBlueprintFunctionLibrary::GetAuthorizationKey());
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("session_id", Var_SessionId);
	JsonObject->SetStringField("authorization_token", Var_AuthorizationToken);
	FString Content;
	TSharedRef<TJsonWriter<TCHAR>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(Content);
	Request->OnProcessRequestComplete().BindUObject(this, &UEGIK_RemoveUseronRelaySession::OnResponseReceived);
	if (!Request->ProcessRequest())
	{
		OnFailure.Broadcast(FEGIK_RelaySessionInfo(), FEGIK_ErrorStruct(0, "Failed to process request"));
		SetReadyToDestroy();
		MarkPendingKill();
	}
}

void UEGIK_RemoveUseronRelaySession::OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
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
		OnFailure.Broadcast(FEGIK_RelaySessionInfo(), FEGIK_ErrorStruct(0, "Failed to process request"));
	}
	SetReadyToDestroy();
	MarkPendingKill();
}
