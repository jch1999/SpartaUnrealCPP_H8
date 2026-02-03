#include "AnimNotifies/AN_PlayRandomFootStep.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

void UAN_PlayRandomFootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    // Super::Notify(MeshComp, Animation, EventReference);
    if (!MeshComp) return;
    
    UWorld* World = MeshComp->GetWorld();
    if (!World) return;

    if (FootStepSounds.Num() == 0) return;
    const FVector Location = MeshComp->GetComponentLocation();

    int32 Rand = FMath::Rand32() % FootStepSounds.Num();

    UGameplayStatics::PlaySoundAtLocation(
        World,
        FootStepSounds[Rand],
        Location
        // (선택) Attenuation/Concurrency/SoundClass 등은 필요해질 때 추가
    );
}
