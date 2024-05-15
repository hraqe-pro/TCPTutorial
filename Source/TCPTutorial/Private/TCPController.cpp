// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPController.h"

#include "ExampleStruct.h"
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

	ClientRemoteSocket = Socket;
	
	TArray<uint8> Bytes;
	FMemoryWriter MemoryWriter(Bytes);

	FExampleInventory Inventory;

	for(int32 i = 0; i < 400; ++i)
	{
		Inventory.InventorySlots.Add(i, FItem());
		Inventory.InventorySlots[i].Description = "ABC_DDD_" + FString::FromInt(i);
	}
	
	MemoryWriter << Inventory;

	SendBytes(Inventory, EInstruction::REPLICATE_INVENTORY);
	SendBytes(Inventory, EInstruction::REPLICATE_INVENTORY);
	
//	FMemoryReader MemoryReader(Bytes);

//	FExampleInventory Inventory2;
//	MemoryReader << Inventory2;
	
//	UE_LOG(LogTemp, Warning, TEXT("Inventory: %s"), *Inventory2.InventorySlots[1].Description);
	
//	int32 BytesSent = 0;
//	if(!Socket->Send(Bytes.GetData(), Bytes.Num(), BytesSent))
//	{
//		UKismetSystemLibrary::PrintString(GetWorld(), "Unable to send data");
//	}
	
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
						BytesSum.Append(Bytes);
						
						uint32 AwaitingBytesNum = 0;
						bool ShouldContinue = true;
						
						while(ShouldContinue)
						{
							FMemoryReader MemoryReader(BytesSum);
							MemoryReader << AwaitingBytesNum;

							if(AwaitingBytesNum <= static_cast<uint32>(BytesSum.Num()) && BytesSum.Num() != 0)
							{
								BytesSum.RemoveAt(0, 4);
								ProcessBytes(BytesSum);
								BytesSum.RemoveAt(0, AwaitingBytesNum - 4);
							}
							else
							{
								ShouldContinue = false;
							}
						}
					}
					else
					{
						UKismetSystemLibrary::PrintString(GetWorld(), "Unable to receive data");
					}
				}
			}
		}
	}
}

void ATCPController::ProcessBytes(const TArray<uint8>& Array)
{
	FMemoryReader MemoryReader(Array);

	EInstruction Instruction;

	MemoryReader << Instruction;

	switch (Instruction)
	{
		case EInstruction::REPLICATE_INVENTORY:
		{
			FExampleInventory Inventory;
			MemoryReader << Inventory;
			UE_LOG(LogTemp, Warning, TEXT("Inventory: %s"), *Inventory.InventorySlots[1].Description);
		}
		break;
	}
}

