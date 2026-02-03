#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "DizzyItem.generated.h"

UCLASS()
class SPARTAUNREALCPP_API ADizzyItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ADizzyItem();

protected:
	virtual void ActivateItem(AActor* Activator) override;

protected:
	UPROPERTY(EditDefaultsOnly,Category="Dizzy")
	float DizzyTime;
};
