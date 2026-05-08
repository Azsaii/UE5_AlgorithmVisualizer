#include "AlgorithmPawn.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AlgorithmPlayerController.h"

AAlgorithmPawn::AAlgorithmPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    RootComponent = CameraComp;

    CameraComp->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
}

void AAlgorithmPawn::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

    if (Subsystem && MappingContext)
        Subsystem->AddMappingContext(MappingContext, 0);
}

void AAlgorithmPawn::SetCameraLocation(FVector NewLocation)
{
    SetActorLocation(NewLocation);
}

void AAlgorithmPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EIC) return;

    EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AAlgorithmPawn::Move);
    EIC->BindAction(IA_Zoom, ETriggerEvent::Triggered, this, &AAlgorithmPawn::Zoom);
    EIC->BindAction(IA_ToggleCamera, ETriggerEvent::Started, this, &AAlgorithmPawn::RequestToggleCamera);
}

void AAlgorithmPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (MoveInput.IsNearlyZero()) return;

    FVector Delta = FVector(MoveInput.X, MoveInput.Y, 0.f) * MoveSpeed * DeltaTime;
    AddActorWorldOffset(Delta);
    MoveInput = FVector2D::ZeroVector;
}

void AAlgorithmPawn::RequestToggleCamera()
{
    UE_LOG(LogTemp, Warning, TEXT("RequestToggleCamera called"));

    AAlgorithmPlayerController* PC = Cast<AAlgorithmPlayerController>(GetController());
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("PC cast failed"));
        return;
    }
    PC->ToggleCameraMode();
}

void AAlgorithmPawn::Move(const FInputActionValue& Value)
{
    MoveInput = Value.Get<FVector2D>();
}

void AAlgorithmPawn::Zoom(const FInputActionValue& Value)
{
    float ScrollValue = Value.Get<float>();

    // 현재 Z 위치에서 스크롤 값만큼 이동
    FVector CurrentLocation = GetActorLocation();
    float NewZ = FMath::Clamp(
        CurrentLocation.Z - ScrollValue * ZoomSpeed,
        ZoomMin,
        ZoomMax
    );

    SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, NewZ));
}