#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ABaseItem::ABaseItem()
	:PickupParticle(nullptr),
	PickupSound(nullptr)
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Collision);

	// 이벤트 바인딩
	// 런타임에 함수를 동적으로 바인딩 해준다.
	// 컴파일 시점에서는 어느 객체가 생성되어서 충돌이 날 지 알 수 없기 때문
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnItemEndOverlap);
}

void ABaseItem::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Overlapped!!")));
		}
		ActivateItem(OtherActor);
	}
}

void ABaseItem::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void ABaseItem::ActivateItem(AActor* Activator)
{
	UParticleSystemComponent* Particle = nullptr;
	if (PickupParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PickupParticle,
			GetActorLocation(),
			GetActorRotation(),
			true
		);
	}

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation()
		);
	}

	if (Particle)
	{
		FTimerHandle DestoryParticleTimerHandle;
		TWeakObjectPtr<UParticleSystemComponent> ParticlePtr(Particle);
		TWeakObjectPtr<UWorld> WorldPtr(GetWorld());
		GetWorld()->GetTimerManager().SetTimer(
			DestoryParticleTimerHandle,
			[WorldPtr, ParticlePtr]()
			{
				if (WorldPtr.IsValid() && ParticlePtr.IsValid())
				{
					ParticlePtr->DestroyComponent();
				}
			},
			1.0f,
			false
		);
	}
}

FName ABaseItem::GetItemType() const
{
	return ItemType;
}

void ABaseItem::DestroyItem()
{
	Destroy();
}
