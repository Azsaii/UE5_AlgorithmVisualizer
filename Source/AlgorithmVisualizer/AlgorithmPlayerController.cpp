#include "AlgorithmPlayerController.h"
#include "AlgorithmPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AAlgorithmPlayerController::AAlgorithmPlayerController()
{
    bShowMouseCursor = true;
}

void AAlgorithmPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 한 프레임 뒤에 실행해서 엔진 초기화 이후에 적용
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            ApplyCameraMode(ECameraMode::Fixed);
        }, 0.1f, false);
}

void AAlgorithmPlayerController::SetFixedCameraLocation(FVector NewLocation)
{
    FixedCameraLocation = NewLocation;

    // Fixed 모드 중에 타일이 재생성되면 즉시 위치 반영
    if (CurrentCameraMode == ECameraMode::Fixed)
    {
        APawn* ControlledPawn = GetPawn();
        if (ControlledPawn) ControlledPawn->SetActorLocation(FixedCameraLocation);
    }
}

void AAlgorithmPlayerController::ToggleCameraMode()
{
    ECameraMode NewMode = (CurrentCameraMode == ECameraMode::Fixed)
        ? ECameraMode::Free
        : ECameraMode::Fixed;

    ApplyCameraMode(NewMode);
}

void AAlgorithmPlayerController::ApplyCameraMode(ECameraMode NewMode)
{
    CurrentCameraMode = NewMode;
    APawn* ControlledPawn = GetPawn();

    switch (NewMode)
    {
    case ECameraMode::Fixed:
        // 카메라를 고정 위치로 복귀
        if (ControlledPawn)
        {
            ControlledPawn->SetActorLocation(FixedCameraLocation);
            SetControlRotation(FRotator(-90.f, 0.f, 0.f)); // 정면 아래 방향
        }
        bShowMouseCursor = true;
        {
            // 뷰포트 클릭해도 커서 안사라지게
            FInputModeGameAndUI InputMode;
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            InputMode.SetHideCursorDuringCapture(false); // 캡쳐 중에도 커서 유지
            SetInputMode(InputMode);
        }
        break;

    case ECameraMode::Free:
        bShowMouseCursor = false;
        SetInputMode(FInputModeGameOnly());
        break;
    }
}