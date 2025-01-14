﻿// Copyright (c) 2024 Betide Studio. All Rights Reserved.


#include "EGIK_DeleteDeployment.h"

UEGIK_DeleteDeployment* UEGIK_DeleteDeployment::DeleteDeployment(FString bRequestId, FString ContainerLogStorage)
{
	UEGIK_DeleteDeployment* UEGIK_DeleteDeploymentObject = NewObject<UEGIK_DeleteDeployment>();
	UEGIK_DeleteDeploymentObject->Var_bRequestId = bRequestId;
	UEGIK_DeleteDeploymentObject->Var_ContainerLogStorage = ContainerLogStorage;
	return UEGIK_DeleteDeploymentObject;
}

void UEGIK_DeleteDeployment::OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
                                                TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg)
{
	if (HttpResponse.IsValid())
	{
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
			if(FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				FEGIK_DeleteDeploymentResponse Response;
				Response.Message = JsonObject->GetStringField(TEXT("message"));
				if(JsonObject->HasField(TEXT("deployment_summary")))
				{
					Response.DeploymentStatusAndInfo = JsonObject->GetObjectField(TEXT("deployment_summary"));
				}
				OnSuccess.Broadcast(Response, FEGIK_ErrorStruct());
			}
			else
			{
				OnFailure.Broadcast(FEGIK_DeleteDeploymentResponse(), FEGIK_ErrorStruct(0, "Failed to deserialize response"));
			}
		}
		else
		{
			OnFailure.Broadcast(FEGIK_DeleteDeploymentResponse(), FEGIK_ErrorStruct(HttpResponse->GetResponseCode(), HttpResponse->GetContentAsString()));
		}
	}
	else
	{
		OnFailure.Broadcast(FEGIK_DeleteDeploymentResponse(), FEGIK_ErrorStruct(0, "Failed to deserialize response"));
	}
	SetReadyToDestroy();
	MarkPendingKill();
}

void UEGIK_DeleteDeployment::Activate()
{
	Super::Activate();
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetVerb("DELETE");
	Request->SetURL("https://api.edgegap.com/v1/stop/" + Var_bRequestId);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", UEGIKBlueprintFunctionLibrary::GetAuthorizationKey());
	Request->OnProcessRequestComplete().BindUObject(this, &UEGIK_DeleteDeployment::OnResponseReceived);
	if (!Request->ProcessRequest())
	{
		OnFailure.Broadcast(FEGIK_DeleteDeploymentResponse(), FEGIK_ErrorStruct(0, "Failed to process request"));
		SetReadyToDestroy();
		MarkPendingKill();
	}
}