#pragma once

#include "CoreMinimal.h"
#include "Common/TcpListener.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TCPController.generated.h"

UENUM()
enum class EInstruction
{
	REPLICATE_INVENTORY
};

UCLASS()
class TCPTUTORIAL_API ATCPController : public AActor
{
	GENERATED_BODY()
	
public:	
	ATCPController();

protected:
	virtual void BeginPlay() override;

	bool ClientConnected(FSocket* Socket, const FIPv4Endpoint& FiPv4Endpoint);
	
public:

	template<typename T>void SendBytes(T& Data, const EInstruction Instruction)
	{
		TArray<uint8> Bytes;
		FMemoryWriter Writer(Bytes);

		uint32 NumBytes = 16;
		Writer << NumBytes;
		Writer << Instruction;
		Writer << Data;

		Writer.Seek(0);

		NumBytes = Bytes.Num();

		Writer << NumBytes;

		int32 BytesSent = 0;

		if(!ClientRemoteSocket->Send(Bytes.GetData(), Bytes.Num(), BytesSent))
		{
			UKismetSystemLibrary::PrintString(GetWorld(), "Unable to send data");
		}
	}

	FSocket* ClientRemoteSocket;
	
	UFUNCTION(BlueprintCallable)
	void Connect(FString ServerIp);
	virtual void Tick(float DeltaTime) override;
	
	void ProcessBytes(const TArray<uint8> &Array);
	
	FSocket* ClientSocket;

	TUniquePtr<FTcpListener> ServerListner;
	
	const uint32 BufferSize = 4096;

	TArray<uint8> BytesSum;
};
