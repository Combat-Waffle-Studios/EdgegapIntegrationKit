﻿// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "EGIK_UpdateDeploymentProperties.h"

UEGIK_UpdateDeploymentProperties* UEGIK_UpdateDeploymentProperties::UpdateDeploymentProperties(
	FEGIK_UpdateDeploymentPropertiesRequest Request)
{
	UEGIK_UpdateDeploymentProperties* Node = NewObject<UEGIK_UpdateDeploymentProperties>();
	Node->Var_Request = Request;
	return Node;
}

void UEGIK_UpdateDeploymentProperties::OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
	TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg)
{
	if (HttpResponse.IsValid())
	{
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			OnSuccess.Broadcast(Var_Request.bIsJoinableBySession, FEGIK_ErrorStruct());
		}
		else
		{
			OnFailure.Broadcast(Var_Request.bIsJoinableBySession, FEGIK_ErrorStruct(HttpResponse->GetResponseCode(), "Failed to update deployment properties"));
		}
	}
	else
	{
		OnFailure.Broadcast(Var_Request.bIsJoinableBySession, FEGIK_ErrorStruct(0, "Failed to deserialize response"));
	}
	SetReadyToDestroy();
	MarkPendingKill();
}

void UEGIK_UpdateDeploymentProperties::Activate()
{
	Super::Activate();
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetVerb("PATCH");
	Request->SetURL("https://api.edgegap.com/v1/deployments/" + Var_Request.RequestId);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", UEGIKBlueprintFunctionLibrary::GetAuthorizationKey());
	Request->OnProcessRequestComplete().BindUObject(this, &UEGIK_UpdateDeploymentProperties::OnResponseReceived);
	if (!Request->ProcessRequest())
	{
		OnFailure.Broadcast(false, FEGIK_ErrorStruct(0, "Failed to process request"));
		SetReadyToDestroy();
		MarkPendingKill();
	}
}
