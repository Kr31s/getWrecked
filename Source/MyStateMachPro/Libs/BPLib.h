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

	FColliderInfo(FVector BoxExtent, FVector RelativeLocation, FRotator RelativRotation, EBoxType BoxType) :
		BoxExtent(BoxExtent),
		RelativeLocation(RelativeLocation),
		RelativRotation(RelativRotation),
		BoxType(BoxType)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector BoxExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator RelativRotation;

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