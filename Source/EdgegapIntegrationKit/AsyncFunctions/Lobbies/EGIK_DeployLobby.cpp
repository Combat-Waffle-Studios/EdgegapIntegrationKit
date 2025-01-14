﻿// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "EGIK_DeployLobby.h"

UEGIK_DeployLobby* UEGIK_DeployLobby::DeployLobby(FString LobbyName)
{
	UEGIK_DeployLobby* Node = NewObject<UEGIK_DeployLobby>();
	Node->Var_LobbyName = LobbyName;
	return Node;
}

void UEGIK_DeployLobby::Activate()
{
	Super::Activate();
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetVerb("POST");
	Request->SetURL("https://api.edgegap.com/v1/lobbies:deploy");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", UEGIKBlueprintFunctionLibrary::GetAuthorizationKey());
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("name", Var_LobbyName);
	FString Content;
	TSharedRef<TJsonWriter<TCHAR>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(Content);
	Request->OnProcessRequestComplete().BindUObject(this, &UEGIK_DeployLobby::OnResponseReceived);
	if (!Request->ProcessRequest())
	{
		OnFailure.Broadcast(FEGIK_LobbyInfo(), FEGIK_ErrorStruct(0, "Failed to process request"));
		SetReadyToDestroy();
		MarkPendingKill();
	}
}

void UEGIK_DeployLobby::OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse,
	bool bArg)
{
	if (HttpResponse.IsValid())
	{
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(HttpResponse->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				FEGIK_LobbyInfo Lobby;
				Lobby.Name = JsonObject->GetStringField(TEXT("name"));
				Lobby.Url = JsonObject->GetStringField(TEXT("url"));
				Lobby.Status = JsonObject->GetStringField(TEXT("status"));
				OnSuccess.Broadcast(Lobby, FEGIK_ErrorStruct());
			}
			else
			{
				OnFailure.Broadcast(FEGIK_LobbyInfo(), FEGIK_ErrorStruct(0, "Failed to deserialize response"));
			}
		}
		else
		{
			OnFailure.Broadcast(FEGIK_LobbyInfo(), FEGIK_ErrorStruct(HttpResponse->GetResponseCode(), HttpResponse->GetContentAsString()));
		}
	}
	else
	{
		OnFailure.Broadcast(FEGIK_LobbyInfo(), FEGIK_ErrorStruct(0, "Failed to deserialize response"));
	}
	SetReadyToDestroy();
	MarkPendingKill();
}
