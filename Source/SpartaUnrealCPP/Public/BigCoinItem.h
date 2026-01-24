#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinItem.h"
#include "BigCoinItem.generated.h"

UCLASS()
class SPARTAUNREALCPP_API ABigCoinItem : public ACoinItem
{
	GENERATED_BODY()
	
public:	
	ABigCoinItem();
	
	virtual void ActivateItem(AActor* Activator) override;
};
