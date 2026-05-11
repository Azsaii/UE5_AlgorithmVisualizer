#include "AlgorithmGameMode.h"
#include "GridManager.h"
#include "ControlPanelWidget.h"
#include "AlgorithmPlayerController.h"

void AAlgorithmGameMode::BeginPlay()
{
    Super::BeginPlay();

    // GridManager 스폰
    GridManager = GetWorld()->SpawnActor<AGridManager>(
        GridManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        // 위젯 생성, GridManager 주입
        ControlPanel = CreateWidget<UControlPanelWidget>(PC, ControlPanelClass);
        if (ControlPanel)
        {
            ControlPanel->GridManager = GridManager;
            ControlPanel->AddToViewport();
        }

        AAlgorithmPlayerController* ALPC = Cast<AAlgorithmPlayerController>(PC);
        ALPC->ControlPanel = ControlPanel;
        ALPC->GridManager = GridManager;
    }

    // Pawn 이 완전히 초기화된 이후에 AdjustCamera 가 호출되게
    // 타이머로 한 프레임 뒤에 기본 그리드 생성
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            if (GridManager)
                GridManager->RegenerateGrid(10, 10);
        }, 0.1f, false);
}