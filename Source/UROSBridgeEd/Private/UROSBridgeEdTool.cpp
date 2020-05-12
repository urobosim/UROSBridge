// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#include "UROSBridgeEdTool.h"
#include "UObject/ConstructorHelpers.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

UROSBridgeEdTool::UROSBridgeEdTool(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}


void UROSBridgeEdTool::Connect()
{
  UE_LOG(LogTemp, Error, TEXT("Connect to websocked"));
        if(!RosHandler.IsValid())
	{
		RosHandler = MakeShareable<FROSBridgeHandler>(
			new FROSBridgeHandler(ServerAdress, ServerPort));
	}

	// Register Services, Publisher and Subcriber
	for (auto Pub : PublisherList)
	{
          AlreadyRegistered.Add(Pub);
          UE_LOG(LogTemp, Error, TEXT("Before Register"));
          Pub->Register(Namespace);
          UE_LOG(LogTemp, Error, TEXT("After Register"));
          // Register SrvServers
          for (auto SrvServer : Pub->ServicesToPublish)
            {
              RosHandler->AddServiceServer(SrvServer);
            }

          // Register Publisher
          for (auto Publisher : Pub->PublisherToPublish)
            {
              RosHandler->AddPublisher(Publisher);
            }

          // Register Subscriber
          for (auto Subscriber : Pub->SubscriberToPublish)
            {
              RosHandler->AddSubscriber(Subscriber);
            }

		// This Avoids registering the same Servers Multiple times
		// if (Pub && AlreadyRegistered.Find(Pub) == INDEX_NONE)
		// {
		// 	AlreadyRegistered.Add(Pub);

		// 	auto BaseClass = Pub->GetDefaultObject<UROSCallbackRegisterBase>();

		// 	// Make sure Outer is something GetWorld() can be called on.
                //         UE_LOG(LogTemp, Error, TEXT("BaseName %s"), *Pub->GetClass()->GetName());
		// 	BaseClass->Rename(*BaseClass->GetName(), this);
		// 	BaseClass->Register(Namespace);

		// 	// Register SrvServers
		// 	for (auto SrvServer : BaseClass->ServicesToPublish)
		// 	// for (auto SrvServer : Pub->ServicesToPublish)
		// 	{
		// 		RosHandler->AddServiceServer(SrvServer);
		// 	}

		// 	// Register Publisher
		// 	for (auto Publisher : BaseClass->PublisherToPublish)
		// 	{
		// 		RosHandler->AddPublisher(Publisher);
		// 	}

		// 	// Register Subscriber
		// 	for (auto Subscriber : BaseClass->SubscriberToPublish)
		// 	{
		// 		RosHandler->AddSubscriber(Subscriber);
		// 	}

		// }
	}

	// bool bAdressChanged = (RosHandler->GetHost().Compare(ServerAdress) != 0) || (RosHandler->GetPort() != ServerPort);
	// // Set up Serveradress and callbacks
	// RosHandler->AddConnectedCallback(this, &UROSBridgeEdTool::ConnectedCallback);
	// RosHandler->AddErrorCallback(this, &UROSBridgeEdTool::ConnectionErrorCallback);

	// // Connect
	// if (!RosHandler->IsConnected() || bAdressChanged)
	// {
	// 	RosHandler->SetHost(ServerAdress);
	// 	RosHandler->SetPort(ServerPort);
	// 	RosHandler->Connect();
	// }

		RosHandler->Connect();
}

void UROSBridgeEdTool::Disconnect()
{
        RosHandler->Disconnect();
	AlreadyRegistered.Empty();
}


void UROSBridgeEdTool::ConnectionErrorCallback()
{
	AlreadyRegistered.Empty();
	ConnectionStatus = TEXT("Not connected.");
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Connection to RosBridge lost.")));
}

void UROSBridgeEdTool::ConnectedCallback()
{
	ConnectionStatus = TEXT("Connected to Rosbridge.");
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("You are now connected to RosBridge.")));
}

UWorld* UROSBridgeEdTool::GetWorld() const
{
	if (GEditor)
		return GEditor->GetEditorWorldContext().World();
	return nullptr;

}
