#include "TileActor.h"
#include "Components/WidgetComponent.h"
#include "TileLabelWidget.h"

ATileActor::ATileActor()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);

    LabelRoot = CreateDefaultSubobject<USceneComponent>(TEXT("LabelRoot"));
    LabelRoot->SetupAttachment(RootComponent);
    LabelRoot->SetRelativeLocation(FVector(0, 0, 10));

    LabelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LabelWidget"));
    LabelWidget->SetupAttachment(LabelRoot);
    LabelWidget->SetPivot(FVector2D(0.5f, 0.5f));
    LabelWidget->SetRelativeLocation(FVector(20, -20, 0));
    LabelWidget->SetRelativeRotation(FRotator(90, -180, 0));
    LabelWidget->SetRelativeScale3D(FVector(0.9));

    // 화면 기준 UI
    LabelWidget->SetWidgetSpace(EWidgetSpace::World);
    LabelWidget->SetWorldScale3D(FVector(1.f));
    LabelWidget->SetDrawSize(FVector2D(100, 100));
    LabelWidget->SetVisibility(false);
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

void ATileActor::SetStateAndColor(ETileState NewState)
{
    CurrentState = NewState;
    if (!DynamicMaterial) return;

    FLinearColor Color = StateToColor(NewState);
    DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);

    switch (NewState)
    {
    case ETileState::Start: {
        ShowLabel(TEXT("S"), Color_LabelStart);
        break;
    }

    case ETileState::Goal: {
        ShowLabel(TEXT("E"), Color_LavelGoal);
        break;
    }
    default: {
        HideLabel();
        break;
    }
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
    DynamicMaterial->SetScalarParameterValue(TEXT("EmissiveStrength_Hover"), bHovered ? 2.f : 0.f);
}

void ATileActor::SetPath(bool bPath)
{
    if (!DynamicMaterial) return;
    DynamicMaterial->SetScalarParameterValue(TEXT("EmissiveStrength_Path"), bPath ? 2.f : 0.f);
}

void ATileActor::ShowLabel(const FString& Text, FLinearColor Color)
{
    if (UTileLabelWidget* Widget = Cast<UTileLabelWidget>(LabelWidget->GetUserWidgetObject())) {
        Widget->SetLabelTextAndColor(Text, Color);
    }

    LabelWidget->SetVisibility(true);
}

void ATileActor::HideLabel()
{
    LabelWidget->SetVisibility(false);
}