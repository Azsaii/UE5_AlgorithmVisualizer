#include "GridManager.h"
#include "TileActor.h"
#include "Engine/World.h"
#include "AlgorithmPawn.h"
#include "AlgorithmPlayerController.h"

AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AGridManager::AdjustCamera(int32 Width, int32 Height)
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    float MaxDimension = FMath::Max(Width, Height) * TileSize;
    float TargetZ = MaxDimension * 1.1f;
    float XOffset = 50.f * TargetZ / 1000.f;
    FVector FixedLocation = FVector(XOffset, 0.f, TargetZ);

    // Pawn 위치 직접 설정
    AAlgorithmPawn* Pawn = Cast<AAlgorithmPawn>(PC->GetPawn());
    if (Pawn)
        Pawn->SetCameraLocation(FixedLocation);

    // PlayerController 에 고정 위치 저장
    AAlgorithmPlayerController* AlgorithmPC = Cast<AAlgorithmPlayerController>(PC);
    if (AlgorithmPC)
        AlgorithmPC->SetFixedCameraLocation(FixedLocation);
}

void AGridManager::GenerateGrid(int32 NewWidth, int32 NewHeight)
{
    // 유효성 검사
    if (NewWidth < 2 || NewWidth  > 100) return;
    if (NewHeight < 2 || NewHeight > 100) return;
    if (!TileClass) { UE_LOG(LogTemp, Error, TEXT("TileClass is not set!")); return; }

    CurrentWidth = NewWidth;
    CurrentHeight = NewHeight;

    ReGenerateGrid();
}

void AGridManager::ReGenerateGrid()
{
    // 기존 타일 제거
    for (ATileActor* Tile : GridTiles)
        if (IsValid(Tile)) Tile->Destroy();

    GridTiles.Empty();

    SpawnTiles(CurrentWidth, CurrentHeight);
    CenterGridToOrigin(CurrentWidth, CurrentHeight);

    // 카메라 z위치 조정
    AdjustCamera(CurrentWidth, CurrentHeight);
}

void AGridManager::SpawnTiles(int32 Width, int32 Height)
{
    GridTiles.Reserve(Width * Height);

    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            // 타일 위치 계산 (Y축 반전으로 화면 좌상단이 0,0)
            FVector SpawnLocation(X * TileSize, Y * TileSize, 0.f);
            FActorSpawnParameters Params;
            Params.Owner = this;

            ATileActor* Tile = GetWorld()->SpawnActor<ATileActor>(
                TileClass, SpawnLocation, FRotator::ZeroRotator, Params);

            if (Tile)
            {
                Tile->GridX = X;
                Tile->GridY = Y;
                // 이름 설정 (디버깅용)
                Tile->SetActorLabel(FString::Printf(TEXT("Tile_%d_%d"), X, Y));
                Tile->SetState(ETileState::Unvisited);
                GridTiles.Add(Tile);
            }
        }
    }
}

ATileActor* AGridManager::GetTile(int32 X, int32 Y) const
{
    if (X < 0 || X >= CurrentWidth || Y < 0 || Y >= CurrentHeight)
        return nullptr;

    return GridTiles[X + Y * CurrentWidth];
}

// 그리드 전체를 원점 중앙 정렬
void AGridManager::CenterGridToOrigin(int32 Width, int32 Height)
{
    FVector Offset(
        -(Width * TileSize) / 2.f + TileSize / 2.f,
        -(Height * TileSize) / 2.f + TileSize / 2.f,
        0.f
    );

    for (ATileActor* Tile : GridTiles)
        if (IsValid(Tile))
            Tile->SetActorLocation(Tile->GetActorLocation() + Offset);
}
