// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
	ATCPController();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool ClientConnected(FSocket* Socket, const FIPv4Endpoint& FiPv4Endpoint);

public:	

	UFUNCTION(BlueprintCallable)
	void Connect(FString ServerIp);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	FSocket* ClientSocket;
	TUniquePtr<FTcpListener> ServerListener;
	const uint32 BufferSize = 4096;
	
};
