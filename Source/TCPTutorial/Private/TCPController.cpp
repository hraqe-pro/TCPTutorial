// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPController.h"

#include "Kismet/KismetSystemLibrary.h"

ATCPController::ATCPController()
{
 	
	PrimaryActorTick.bCanEverTick = true;

}

void ATCPController::BeginPlay()
{
	Super::BeginPlay();

	if(GetNetMode() == NM_DedicatedServer)
	{
		FIPv4Address IpAddress;
		FIPv4Address::Parse(FString("0.0.0.0"), IpAddress);

		FIPv4Endpoint Endpoint(IpAddress, (uint16)2213);

		ServerListner = MakeUnique<FTcpListener>(Endpoint);

		ServerListner->OnConnectionAccepted().BindUObject(this, &ATCPController::ClientConnected);
	}
}

bool ATCPController::ClientConnected(FSocket* Socket, const FIPv4Endpoint& FiPv4Endpoint)
{
	UKismetSystemLibrary::PrintString(GetWorld(), "CLIENT CONNECTED!");

	TArray<uint8> Bytes;
	FMemoryWriter MemoryWriter(Bytes);

	FString Message = "Industrial Black Metal is great for studying";
	MemoryWriter << Message;

	int32 BytesSent = 0;
	if(!Socket->Send(Bytes.GetData(), Bytes.Num(), BytesSent))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Unable to send data");
	}
	
	return true;
}

void ATCPController::Connect(FString ServerIp)
{
	FIPv4Address Address;
	FIPv4Address::Parse(ServerIp, Address);
	FIPv4Endpoint Endpoint(Address, (uint16)2213);

	ClientSocket = FTcpSocketBuilder(TEXT("Client Socket")).AsReusable().WithReceiveBufferSize(BufferSize).AsNonBlocking();
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	
	TSharedPtr<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	Addr->SetIp(Endpoint.Address.Value);
	Addr->SetPort(Endpoint.Port);

	ClientSocket->Connect(*Addr);
}


void ATCPController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(GetNetMode() == NM_Client)
	{
		TArray<uint8> Bytes;

		if(ClientSocket)
		{
			if(ClientSocket->GetConnectionState() == SCS_Connected)
			{
				uint32 PendingData = 0;
				if(ClientSocket->HasPendingData(PendingData))
				{
					Bytes.SetNum(PendingData);

					int32 BytesRead = 0;
					if(ClientSocket->Recv(Bytes.GetData(), PendingData, BytesRead))
					{
						FString Message = "";
						FMemoryReader MemoryReader(Bytes);

						MemoryReader << Message;

						UKismetSystemLibrary::PrintString(GetWorld(), Message, true, true, FLinearColor::Red, 50.f);
					}
				}
			}
		}
	}
}

