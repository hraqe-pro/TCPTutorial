#pragma once

#include "CoreMinimal.h"
#include "Common/TcpListener.h"
#include "GameFramework/Actor.h"
#include "TCPController.generated.h"

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

	UFUNCTION(BlueprintCallable)
	void Connect(FString ServerIp);
	
	virtual void Tick(float DeltaTime) override;

	FSocket* ClientSocket;

	TUniquePtr<FTcpListener> ServerListner;
	
	const uint32 BufferSize = 4096;
};
