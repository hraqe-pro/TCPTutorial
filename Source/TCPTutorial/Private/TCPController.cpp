// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPController.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATCPController::ATCPController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATCPController::BeginPlay()
{
	Super::BeginPlay();

	if(GetNetMode() == NM_DedicatedServer)
	{
		FIPv4Address IpAdresses;
		FIPv4Address::Parse(FString("0.0.0.0"), IpAdresses);
		FIPv4Endpoint Endpoint(IpAdresses, (uint16)2213);
		
		ServerListener = MakeUnique<FTcpListener>(Endpoint); //(TUniquePtr<FTcpListener> TcpListener should be a a member variable)
		ServerListener->OnConnectionAccepted().BindUObject(this, &ATCPController::ClientConnected);
	}
}

// Called every frame
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

bool ATCPController::ClientConnected(FSocket* Socket, const FIPv4Endpoint& FiPv4Endpoint)
{
	UKismetSystemLibrary::PrintString(GetWorld(), "CLIENT CONNECTED!");

	TArray<uint8> Bytes;
	FMemoryWriter MemoryWriter(Bytes);

	FString Message = "Industrial Black Metal is great for studying!";
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
	FIPv4Address IpAddress;
	FIPv4Address::Parse(ServerIp, IpAddress);
	FIPv4Endpoint Endpoint(IpAddress, (uint16)2213);

	ClientSocket = FTcpSocketBuilder(TEXT("Client Socket")).AsReusable().WithReceiveBufferSize(BufferSize).AsNonBlocking();
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	TSharedPtr<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	Addr->SetIp(Endpoint.Address.Value);
	Addr->SetPort(Endpoint.Port);

	ClientSocket->Connect(*Addr);
	ClientSocket->Bind(*Addr);
	int32 NewSize = 0;
	ClientSocket->SetReceiveBufferSize(BufferSize, NewSize);
}
