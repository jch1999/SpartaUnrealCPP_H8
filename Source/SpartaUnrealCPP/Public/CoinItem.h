#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"

UCLASS()
class SPARTAUNREALCPP_API ACoinItem : public ABaseItem
{
	GENERATED_BODY()
	
public:	
	ACoinItem();

protected:
	virtual void ActivateItem(AActor* Activator) override;

	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	int32 PointValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	float RotateSpeed;
};
