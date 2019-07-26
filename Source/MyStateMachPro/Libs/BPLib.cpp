#include "BPLib.h"
#include "PlayerComponents/ScsNodeSearchTamplates.h"
#include "DemoFrameActor.h"

#include "PlayerComponents/HitBoxIDComp.h"

TArray<FColliderInfo> UBPLib::GetCollIDBoxByIndex(UClass *FromClass, int index)
{
	auto Nodes{ GetScsDataNodesForType<UHitBoxIDComp>(FromClass) };
	if (Nodes.Num() == 0)
	{
		return {};
	}

	TArray<FColliderInfo> Out{};
	for (auto&& ChildNode : Nodes[index]->ChildNodes)
	{
		auto* pAsComp{ Cast<UMyHitBoxComponent>(ChildNode->ComponentTemplate) };
		checkf(pAsComp, TEXT("Child component of UHitBoxIDComp not of time UMyHitBoxComponent"));

		Out.Add({ pAsComp->GetScaledBoxExtent(), pAsComp->RelativeLocation,  pAsComp->Etype });

	}
	
	return Out;


}
