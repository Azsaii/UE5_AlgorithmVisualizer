#include "AlgorithmPlayerController.h"
#include "AlgorithmPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TileActor.h"
#include "ControlPanelWidget.h"
#include "GridManager.h"
#include "GraphManager.h"
#include "BFSAlgorithm.h"
#include "DFSAlgorithm.h"
#include "AStarAlgorithm.h"
#include "JPSAlgorithm.h"
#include "DijkstraAlgorithm.h"
#include "BellmanFordAlgorithm.h"
#include "KruskalAlgorithm.h"
#include "PrimAlgorithm.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/NavigationConfig.h"

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

    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            CurrentAlgorithm = MakeUnique<FBFSAlgorithm>(GridManager);

            // 기본으로 BFS 선택
            ControlPanel->SelectAlgorithmBtn(ControlPanel->Btn_SwitchBFS);
        }, 0.1f, false);
    
    // UI가 Space 키 안먹게 하기
    TSharedRef<FNavigationConfig> NavConfig = MakeShared<FNavigationConfig>();

    // 기본적으로 Space가 Accept로 등록되어 있음
    NavConfig->KeyActionRules.Remove(EKeys::SpaceBar);

    FSlateApplication::Get().SetNavigationConfig(NavConfig);
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
    EIC->BindAction(IA_StepOnce, ETriggerEvent::Triggered, this, &AAlgorithmPlayerController::Input_StepOnce);
    EIC->BindAction(IA_StepAll, ETriggerEvent::Started, this, &AAlgorithmPlayerController::Input_StepAll);
}

ATileActor* AAlgorithmPlayerController::GetTileUnderCursor()
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
    return Cast<ATileActor>(HitResult.GetActor());
}

void AAlgorithmPlayerController::Input_SetStart()
{
    ControlPanel->UpdateStatusText(TEXT("Click to set start point"));
    CurrentEditMode = EEditMode::SetStart;
}

void AAlgorithmPlayerController::Input_SetEnd()
{
    ControlPanel->UpdateStatusText(TEXT("Click to set end point"));
    CurrentEditMode = EEditMode::SetEnd;
}

void AAlgorithmPlayerController::Input_ClickStarted()
{
    ATileActor* ClickedTile = GetTileUnderCursor();
    if (!ClickedTile) return;

    FString Msg;
    bIsMousePressed = true;

    // 기존 경로탐색 제거
    CurrentAlgorithm->ClearPath();

    switch (CurrentEditMode) {
    case EEditMode::SetStart: {
        if (GridManager->StartTile) GridManager->StartTile->SetStateAndColor(ETileState::Unvisited);
        GridManager->StartTile = ClickedTile;
        GridManager->StartTile->SetStateAndColor(ETileState::Start);

        Msg = FString::Printf(TEXT("Start point set at (%d, %d)"),
            ClickedTile->GridX, ClickedTile->GridY);

        // 시작지점으로 선택한 지점이 원래 목표점이었으면 기존 목표점은 제거
        if (ClickedTile == GridManager->EndTile) GridManager->EndTile = nullptr;

        break;
    }
    case EEditMode::SetEnd: {
        if (GridManager->EndTile) GridManager->EndTile->SetStateAndColor(ETileState::Unvisited);
        GridManager->EndTile = ClickedTile;
        GridManager->EndTile->SetStateAndColor(ETileState::Goal);

        Msg = FString::Printf(TEXT("End point set at (%d, %d)"),
            ClickedTile->GridX, ClickedTile->GridY);

        // 목표지점으로 선택한 지점이 원래 시작점이었으면 기존 시작점은 제거
        if (ClickedTile == GridManager->StartTile) GridManager->StartTile = nullptr;

        break;
    }
    case EEditMode::None: {
        // 시작/종료 지점은 장애물로 덮어쓰지 않음
        if (ClickedTile == GridManager->StartTile || ClickedTile == GridManager->EndTile) return;

        CurrentEditMode = EEditMode::SetObstacle;

        ETileState& state = ClickedTile->CurrentState;

        // 장애물 지정
        if (state == ETileState::Unvisited) {
            bRemovingObstacle = false;
            ClickedTile->SetStateAndColor(ETileState::Obstacle);
            Msg = TEXT("Placing obstacles...");
        }
        // 장애물 해제
        else if (state == ETileState::Obstacle) {
            bRemovingObstacle = true;
            ClickedTile->SetStateAndColor(ETileState::Unvisited);
            Msg = TEXT("Removing obstacles...");
        }
        break;
    }
    }

    ControlPanel->UpdateStatusText(Msg);
}

void AAlgorithmPlayerController::Input_ClickTriggered()
{
    if (CurrentEditMode != EEditMode::SetObstacle) return;
    if (!bIsMousePressed) return;

    ATileActor* HoveredTile = GetTileUnderCursor();
    if (!HoveredTile) return;

    // 시작/종료 지점은 장애물로 덮어쓰지 않음
    if (HoveredTile == GridManager->StartTile || HoveredTile == GridManager->EndTile) return;

    ETileState TargetState = bRemovingObstacle
        ? ETileState::Unvisited
        : ETileState::Obstacle;

    if (HoveredTile->CurrentState != TargetState)
        HoveredTile->SetStateAndColor(TargetState);
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
    CurrentAlgorithm->ClearPath();

    FString Msg;

    bool bSuccess = GridManager->SwapStartEnd();

    if (!bSuccess) Msg = TEXT("Cannot swap: start or end point not set");
    else Msg = TEXT("Start and end points swapped");

    ControlPanel->UpdateStatusText(Msg);
}

void AAlgorithmPlayerController::Input_ClearScreen() 
{
    Input_ClearPath();

    GridManager->ReGenerateGrid();
    GraphManager->ReGenerateGraph();
    ControlPanel->UpdateStatusText(TEXT("Screen cleared"));
}

void AAlgorithmPlayerController::Input_ClearPath() 
{
    CurrentAlgorithm->ClearPath();
    ControlPanel->UpdateStatusText(TEXT("Path cleared"));
}

void AAlgorithmPlayerController::Input_StepOnce() 
{
    float CurrentTime = GetWorld()->GetTimeSeconds();

    if (CurrentTime - LastCallTime >= FuncCallCoolTime) {
        LastCallTime = CurrentTime;
        CurrentAlgorithm->StepOnce();
    }
}

void AAlgorithmPlayerController::Input_StepAll() 
{
    CurrentAlgorithm->ClearPath();
    CurrentAlgorithm->StepAll();
}

void AAlgorithmPlayerController::SwitchAlgorithm(EAlgorithmType Type)
{
    Input_ClearPath();

    switch (Type)
    {
    case EAlgorithmType::BFS: {
        CurrentAlgorithm = MakeUnique<FBFSAlgorithm>(GridManager);
        break;
    }
    case EAlgorithmType::DFS: {
        CurrentAlgorithm = MakeUnique<FDFSAlgorithm>(GridManager);
        break;
    }
    case EAlgorithmType::ASTAR: {
        CurrentAlgorithm = MakeUnique<FAStarAlgorithm>(GridManager);
        break;
    }
    case EAlgorithmType::JPS: {
        CurrentAlgorithm = MakeUnique<FJPSAlgorithm>(GridManager);
        break;
    }

    case EAlgorithmType::DIJKSTRA: {

        break;
    }
    case EAlgorithmType::BELLMAN_FORD: {

        break;
    }
    case EAlgorithmType::PRIM: {

        break;
    }
    case EAlgorithmType::KRUSKAL: {

        break;
    }
    }
}