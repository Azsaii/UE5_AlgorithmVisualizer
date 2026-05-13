#include "TileActor.h"

ATileActor::ATileActor()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

}

void ATileActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (!TileMaterial)
    {
        UE_LOG(LogTemp, Error, TEXT("TileMaterial is null"));
        return;
    }

    MeshComp->SetMaterial(0, TileMaterial);
    DynamicMaterial = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
}

void ATileActor::SetState(ETileState NewState, bool bColorChange)
{
    CurrentState = NewState;

    if (bColorChange == true) {
        FLinearColor Color = StateToColor(NewState);
        DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);
    }
}

FLinearColor ATileActor::StateToColor(ETileState State)
{
    switch (State)
    {
    case ETileState::Unvisited: return Color_Unvisited;
    case ETileState::Obstacle:  return Color_Obstacle;
    case ETileState::Start:     return Color_Start;
    case ETileState::Goal:      return Color_Goal;
    case ETileState::Open:      return Color_Open;
    case ETileState::Closed:    return Color_Closed;
    case ETileState::Path:      return Color_Path;
    default:                    return FLinearColor::White;
    }
}

void ATileActor::SetHovered(bool bHovered)
{
    if (!DynamicMaterial) return;

    DynamicMaterial->SetScalarParameterValue(TEXT("EmissiveStrength"), bHovered ? 2.f : 0.f);
}