#include "DizzyItem.h"
#include "SpartaCharacter.h"

ADizzyItem::ADizzyItem()
{
}

void ADizzyItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);
    if (ASpartaCharacter* Character = Cast<ASpartaCharacter>(Activator))
    {
        Character->StartDizzy(DizzyTime);
        DestroyItem();
    }
}