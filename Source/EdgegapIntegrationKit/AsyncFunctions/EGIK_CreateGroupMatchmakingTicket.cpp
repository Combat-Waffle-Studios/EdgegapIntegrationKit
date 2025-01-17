// Copyright (c) 2025 Combat Waffle Studios. All Rights Reserved.

#include "EGIK_CreateGroupMatchmakingTicket.h"

UEGIK_CreateGroupMatchmakingTicket* UEGIK_CreateGroupMatchmakingTicket::CreateGroupMatchmakingTicket(const FGOT_CreateGroupMatchmakingStruct& MatchmakingStruct)
{
	UEGIK_CreateGroupMatchmakingTicket* BlueprintNode = NewObject<UEGIK_CreateGroupMatchmakingTicket>();
	BlueprintNode->Var_MatchmakingStruct = MatchmakingStruct;
	return BlueprintNode;
}

void UEGIK_CreateGroupMatchmakingTicket::OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
	TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg)
{
	FEGIK_MatchmakingResponse Response;
	if(HttpResponse.IsValid())
	{
		if(EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
			if(FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				Response.TicketId = JsonObject->GetStringField(TEXT("id"));
				Response.GameProfile = JsonObject->GetStringField(TEXT("game_profile"));
				FDateTime::ParseIso8601(*JsonObject->GetStringField(TEXT("created_at")), Response.CreatedAt);
				const TSharedPtr<FJsonObject>* AssignmentObject;
				if (JsonObject->HasTypedField<EJson::Object>(TEXT("assignment")))
				{
					// The assignment field exists and is an object
					if (JsonObject->TryGetObjectField(TEXT("assignment"), AssignmentObject))
					{
						// Deserialize the assignment object
						Response.Assignment = FEGIK_AssignmentStruct(*AssignmentObject);
					}
					else
					{
						// The assignment field is either null or missing, so handle it as null
						Response.Assignment = FEGIK_AssignmentStruct("null");
					}
				}
				else
				{
					// The assignment field is either null or missing, so handle it as null
					Response.Assignment = FEGIK_AssignmentStruct("null");
				}
				OnSuccess.Broadcast(Response, FEGIK_ErrorStruct());
			}
			else
			{
				OnFailure.Broadcast(FEGIK_MatchmakingResponse(), FEGIK_ErrorStruct(0, "Failed to parse JSON"));
			}
		}
		else
		{
			OnFailure.Broadcast(FEGIK_MatchmakingResponse(), FEGIK_ErrorStruct(HttpResponse->GetResponseCode(), HttpResponse->GetContentAsString()));
		}
	}
	else
	{
		OnFailure.Broadcast(FEGIK_MatchmakingResponse(), FEGIK_ErrorStruct(0, "Failed to connect, likely the Matchmaker is down or the URL is incorrect or is not released"));
	}
}

void UEGIK_CreateGroupMatchmakingTicket::Activate()
{
	Super::Activate();
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetVerb("POST");
	Request->SetURL(Var_MatchmakingStruct.MatchmakingURL + "/group-tickets");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", Var_MatchmakingStruct.AuthToken);
	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> PlayerTicketsArray;

	for (FGOT_GroupAttributesStruct Player : Var_MatchmakingStruct.PlayerAttributes)
	{
		// Beacons
		TSharedPtr<FJsonObject> BeaconsJsonObject = MakeShareable(new FJsonObject);
		for (TPair<FString, float>& Kvp : Player.Beacons)
		{
			BeaconsJsonObject->SetNumberField(Kvp.Key, Kvp.Value);
		}
		
		// Dynamic attribute strings
		TSharedPtr<FJsonObject> AttributesJsonObject = MakeShareable(new FJsonObject);
		AttributesJsonObject->SetObjectField(TEXT("beacons"), BeaconsJsonObject);
		for (TPair<FString, FString>& Kvp : Player.Attributes)
		{
			AttributesJsonObject->SetStringField(Kvp.Key, Kvp.Value);
		}
		
		// Required ticket parameters
		TSharedPtr<FJsonObject> PlayerTicket = MakeShareable(new FJsonObject);
		PlayerTicket->SetObjectField(TEXT("attributes"), AttributesJsonObject);
		PlayerTicket->SetStringField("player_ip", Var_MatchmakingStruct.PlayerIp);
		PlayerTicket->SetStringField("profile", Var_MatchmakingStruct.Profile);

		TSharedRef<FJsonValueObject> JsonValue = MakeShareable(new FJsonValueObject(PlayerTicket));
		PlayerTicketsArray.Add(JsonValue);
	}
	
	JsonObject->SetArrayField("player_tickets", PlayerTicketsArray);
	
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(JsonString);
	UE_LOG(LogTemp, Warning, TEXT("Request: %s"), *JsonString);
	Request->OnProcessRequestComplete().BindUObject(this, &UEGIK_CreateGroupMatchmakingTicket::OnResponseReceived);
	if(!Request->ProcessRequest())
	{
		OnFailure.Broadcast(FEGIK_MatchmakingResponse(), FEGIK_ErrorStruct(0, "Failed to process request"));
		SetReadyToDestroy();
		MarkPendingKill();
	}
}
