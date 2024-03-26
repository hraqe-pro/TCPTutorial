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

}

bool ATCPController::ClientConnected(FSocket* Socket, const FIPv4Endpoint& FiPv4Endpoint)
{
	AsyncTask(ENamedThreads::GameThread, [&]()
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "CLIENT CONNECTED!");
	});

	return true;
}

void ATCPController::Connect(FString ServerIp)
{
	FIPv4Address IpAddress;
	FIPv4Address::Parse(ServerIp, IpAddress);
	FIPv4Endpoint Endpoint(IpAddress, 2213);

	ClientSocket = FTcpSocketBuilder(TEXT("Client Socket")).AsReusable().WithReceiveBufferSize(BufferSize).AsNonBlocking();
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	TSharedPtr<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	Addr->SetIp(Endpoint.Address.Value);
	Addr->SetPort(Endpoint.Port);

	ClientSocket->Connect(*Addr);
}
