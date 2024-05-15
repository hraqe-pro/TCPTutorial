#pragma once

#include "CoreMinimal.h"
#include "ExampleStruct.generated.h"

USTRUCT()
struct FItem
{
	GENERATED_BODY()
	
	int32 ItemId;

	int32 Quantity;

	FString Description;
};

inline FArchive& operator<<(FArchive& Ar, FItem& Data)
{
	Ar << Data.ItemId;
	Ar << Data.Quantity;
	Ar << Data.Description;

	return Ar;
}

USTRUCT()
struct FExampleInventory
{
	GENERATED_BODY()

	TMap<int32, FItem> InventorySlots;
};

inline FArchive& operator<<(FArchive& Ar, FExampleInventory& Data)
{
	Ar << Data.InventorySlots;

	return Ar;
}
