#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GamePlayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
    :ExplosionParticle(nullptr),
    ExplosionSound(nullptr)
{
    ExplosionDelay = 5.0f;
    ExplosionRadius = 300.0f;
    ExplosionDamage = 35;
    ItemType = "Mine";
    bHasExploded = false;

    ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
    ExplosionCollision->InitSphereRadius(ExplosionRadius);
    ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
    if (bHasExploded) return;

    Super::ActivateItem(Activator);
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            ExplosionTimerHandle,
            this,
            &AMineItem::Explode,
            ExplosionDelay,
            false
        );
    }
    bHasExploded = true;
}

void AMineItem::Explode()
{
    UParticleSystemComponent* Particle = nullptr;
    if (ExplosionParticle)
    {
        Particle = UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ExplosionParticle,
            GetActorLocation(),
            GetActorRotation(),
            false
        );
    }

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            ExplosionSound,
            GetActorLocation()
        );
    }

    TArray<AActor*> OverlappingActors;
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Player"))
        {
            UGameplayStatics::ApplyDamage(
                Actor,
                ExplosionDamage,
                nullptr,
                this,
                UDamageType::StaticClass() // 기본 데미지 타입으로 전달
            );
        }
    }

    if (Particle)
    {
        FTimerHandle DestoryParticleTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            DestoryParticleTimerHandle,
            [Particle]()
            {
                Particle->DestroyComponent();
            },
            1.0f,
            false
        );
    }
    DestroyItem();
}
