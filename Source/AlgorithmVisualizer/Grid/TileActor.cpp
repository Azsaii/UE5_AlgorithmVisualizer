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

    if (!DynamicMaterial) {
        UE_LOG(LogTemp, Error, TEXT("DynamicMaterial creation failed"));
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("DynamicMaterial creation success"));
}

void ATileActor::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("BeginPlay - DynamicMaterial: %s"),
        DynamicMaterial ? TEXT("Valid") : TEXT("Null"));

    SetState(ETileState::Unvisited);
}

void ATileActor::SetState(ETileState NewState)
{
    CurrentState = NewState;

    if (!DynamicMaterial)
    {
        UE_LOG(LogTemp, Error, TEXT("SetState - DynamicMaterial is null"));
        return;
    }

    FLinearColor Color = StateToColor(NewState);
    UE_LOG(LogTemp, Warning, TEXT("SetState - Color: R=%f G=%f B=%f"),
        Color.R, Color.G, Color.B);

    DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);

    UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(MeshComp->GetMaterial(0)));
}

FLinearColor ATileActor::StateToColor(ETileState State)
{
    switch (State)
    {
    case ETileState::Unvisited: return Color_Unvisited;
    case ETileState::Obstacle:  return Color_Obstacle;
    case ETileState::Start:     return Color_Start;
    case ETileState::Goal:      return Color_Goal;
    default:                    return FLinearColor::White;
    }
}

void ATileActor::SetHovered(bool bHovered)
{
    if (!DynamicMaterial) return;

    DynamicMaterial->SetScalarParameterValue(TEXT("EmissiveStrength"), bHovered ? 2.f : 0.f);
}