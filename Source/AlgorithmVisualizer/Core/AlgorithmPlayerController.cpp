#include "AlgorithmPlayerController.h"
#include "AlgorithmPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TileActor.h"
#include "ControlPanelWidget.h"
#include "GridManager.h"

AAlgorithmPlayerController::AAlgorithmPlayerController()
{
    bShowMouseCursor = true;
}

void AAlgorithmPlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
}

void AAlgorithmPlayerController::Tick(float DeltaTime)
{
    UpdateHoveredTile();  // 매 틱 호버 타일 갱신
}

void AAlgorithmPlayerController::UpdateHoveredTile()
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

    ATileActor* NewHoveredTile = Cast<ATileActor>(HitResult.GetActor());

    // 이전 호버 타일과 다를 때만 처리
    if (NewHoveredTile == CurrentHoveredTile) return;

    // 머티리얼 복구
    if (CurrentHoveredTile) CurrentHoveredTile->SetHovered(false);

    // 새 타일 호버 효과 적용
    if (NewHoveredTile) NewHoveredTile->SetHovered(true);

    CurrentHoveredTile = NewHoveredTile;
}


void AAlgorithmPlayerController::SetFixedCameraLocation(FVector NewLocation)
{
    FixedCameraLocation = NewLocation;
    APawn* ControlledPawn = GetPawn();
    if (ControlledPawn) ControlledPawn->SetActorLocation(FixedCameraLocation);
}

void AAlgorithmPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
    if (!EIC) return;

    EIC->BindAction(IA_SetStart, ETriggerEvent::Started, this, &AAlgorithmPlayerController::Input_SetStart);
    EIC->BindAction(IA_SetEnd, ETriggerEvent::Started, this, &AAlgorithmPlayerController::Input_SetEnd);
    EIC->BindAction(IA_Click, ETriggerEvent::Started, this, &AAlgorithmPlayerController::Input_ClickStarted);
    EIC->BindAction(IA_Click, ETriggerEvent::Triggered, this, &AAlgorithmPlayerController::Input_ClickTriggered);
    EIC->BindAction(IA_Click, ETriggerEvent::Completed, this, &AAlgorithmPlayerController::Input_ClickCompleted);

    EIC->BindAction(IA_SwapStartEnd, ETriggerEvent::Started, this, &AAlgorithmPlayerController::Input_SwapStartEnd);
    EIC->BindAction(IA_ClearScreen, ETriggerEvent::Started, this, &AAlgorithmPlayerController::Input_ClearScreen);
    EIC->BindAction(IA_ClearPath, ETriggerEvent::Started, this, &AAlgorithmPlayerController::Input_ClearPath);
    EIC->BindAction(IA_ProgressOnce, ETriggerEvent::Started, this, &AAlgorithmPlayerController::Input_ProgressOnce);
    EIC->BindAction(IA_ProgressAll, ETriggerEvent::Started, this, &AAlgorithmPlayerController::Input_ProgressAll);
}

ATileActor* AAlgorithmPlayerController::GetTileUnderCursor()
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
    return Cast<ATileActor>(HitResult.GetActor());
}

void AAlgorithmPlayerController::Input_SetStart()
{
    if (ControlPanel) ControlPanel->UpdateStatusText(TEXT("Click to set start point"));
    CurrentEditMode = EEditMode::SetStart;
}

void AAlgorithmPlayerController::Input_SetEnd()
{
    if (ControlPanel) ControlPanel->UpdateStatusText(TEXT("Click to set end point"));
    CurrentEditMode = EEditMode::SetEnd;
}

void AAlgorithmPlayerController::Input_ClickStarted()
{
    UE_LOG(LogTemp, Error, TEXT("Click start!!"));

    ATileActor* ClickedTile = GetTileUnderCursor();
    if (!ClickedTile) return;

    FString Msg;

    bIsMousePressed = true;

    switch (CurrentEditMode) {
    case EEditMode::SetStart: {
        if (StartTile) StartTile->SetState(ETileState::Unvisited);
        StartTile = ClickedTile;
        StartTile->SetState(ETileState::Start);

        Msg = FString::Printf(TEXT("Start point set at (%d, %d)"),
            ClickedTile->GridX, ClickedTile->GridY);

        // 시작지점으로 선택한 지점이 원래 목표점이었으면 기존 목표점은 제거
        if (ClickedTile == EndTile) EndTile = nullptr;

        break;
    }
    case EEditMode::SetEnd: {
        if (EndTile) EndTile->SetState(ETileState::Unvisited);
        EndTile = ClickedTile;
        EndTile->SetState(ETileState::Goal);

        Msg = FString::Printf(TEXT("End point set at (%d, %d)"),
            ClickedTile->GridX, ClickedTile->GridY);

        // 목표지점으로 선택한 지점이 원래 시작점이었으면 기존 시작점은 제거
        if (ClickedTile == StartTile) StartTile = nullptr;

        break;
    }
    case EEditMode::None: {
        // 시작/종료 지점은 장애물로 덮어쓰지 않음
        if (ClickedTile == StartTile || ClickedTile == EndTile) return;

        CurrentEditMode = EEditMode::SetObstacle;

        ETileState& state = ClickedTile->CurrentState;

        // 장애물 지정
        if (state == ETileState::Unvisited) {
            bRemovingObstacle = false;
            ClickedTile->SetState(ETileState::Obstacle);
            Msg = TEXT("Placing obstacles...");
        }
        // 장애물 해제
        else if (state == ETileState::Obstacle) {
            bRemovingObstacle = true;
            ClickedTile->SetState(ETileState::Unvisited);
            Msg = TEXT("Removing obstacles...");
        }
        break;
    }
    }

    if (ControlPanel) ControlPanel->UpdateStatusText(Msg);
}

void AAlgorithmPlayerController::Input_ClickTriggered()
{
    if (CurrentEditMode != EEditMode::SetObstacle) return;
    if (!bIsMousePressed) return;

    ATileActor* HoveredTile = GetTileUnderCursor();
    if (!HoveredTile) return;

    // 시작/종료 지점은 장애물로 덮어쓰지 않음
    if (HoveredTile == StartTile || HoveredTile == EndTile) return;

    ETileState TargetState = bRemovingObstacle
        ? ETileState::Unvisited
        : ETileState::Obstacle;

    if (HoveredTile->CurrentState != TargetState)
        HoveredTile->SetState(TargetState);
}

void AAlgorithmPlayerController::Input_ClickCompleted()
{
    if (CurrentEditMode == EEditMode::SetObstacle)
    {
        if (ControlPanel)
        {
            FString Msg = bRemovingObstacle
                ? TEXT("Obstacle removal completed")
                : TEXT("Obstacle placement completed");
            ControlPanel->UpdateStatusText(Msg);
        }
    }

    CurrentEditMode = EEditMode::None;
    bIsMousePressed = false;
}

void AAlgorithmPlayerController::Input_SwapStartEnd() 
{
    FString Msg;

    // 스왑 전 조건 체크
    if (!StartTile || !EndTile)
    {
        Msg = TEXT("Cannot swap: start or end point not set");
    }
    else
    {
        ATileActor* tmp = StartTile;
        StartTile = EndTile;
        EndTile = tmp;

        StartTile->SetState(ETileState::Start);
        EndTile->SetState(ETileState::Goal);
        Msg = TEXT("Start and end points swapped");
    }

    if (ControlPanel) ControlPanel->UpdateStatusText(Msg);
}

void AAlgorithmPlayerController::Input_ClearScreen() 
{
    GridManager->ReGenerateGrid();
    if (ControlPanel) ControlPanel->UpdateStatusText(TEXT("Grid cleared"));
}

void AAlgorithmPlayerController::Input_ClearPath() 
{
    if (ControlPanel) ControlPanel->UpdateStatusText(TEXT("Path cleared"));
}

void AAlgorithmPlayerController::Input_ProgressOnce() 
{
    if (ControlPanel) ControlPanel->UpdateStatusText(FString::Printf(TEXT("Searching... (step %d)"), 1));
}

void AAlgorithmPlayerController::Input_ProgressAll() 
{
    if (ControlPanel) ControlPanel->UpdateStatusText(FString::Printf(TEXT("Path found! (step %d)"), 1));
}
