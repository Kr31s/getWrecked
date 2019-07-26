#pragma once
#include "CoreMinimal.h"
#include "MyHitBoxComponent.h"
#include "BPLib.generated.h"


USTRUCT(BlueprintType)
struct FColliderInfo
{
	GENERATED_BODY()

public:
	FColliderInfo() = default;

	FColliderInfo(FVector BoxExtent, FVector RelativeLocation, EBoxType BoxType) :
		BoxExtent(BoxExtent),
		RelativeLocation(RelativeLocation),
		BoxType(BoxType)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector BoxExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EBoxType BoxType;


};

UCLASS()
class UBPLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:		
	UFUNCTION(BlueprintCallable)
		static TArray<FColliderInfo> GetCollIDBoxByIndex(UClass *FromClass, int index);

};