#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PlayRandomFootStep.generated.h"

class USoundBase;
UCLASS()
class SPARTAUNREALCPP_API UAN_PlayRandomFootStep : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	UPROPERTY(EditDefaultsOnly, Category="Sound")
	TArray<TObjectPtr<USoundBase>> FootStepSounds;
};
