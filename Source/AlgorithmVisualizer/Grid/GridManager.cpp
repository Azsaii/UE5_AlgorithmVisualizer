#include "GridManager.h"
#include "TileActor.h"
#include "Engine/World.h"
#include "AlgorithmPawn.h"
#include "AlgorithmPlayerController.h"
#include "ProceduralMeshComponent.h"

AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    PathMeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PathMesh"));
    PathMeshComp->SetupAttachment(RootComponent);
}

void AGridManager::BeginPlay()
{
    Super::BeginPlay();
    PathMeshComp->SetMaterial(0, PathMaterial);
    PathMeshComp->SetCastShadow(false);
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
    StartTile = nullptr;
    EndTile = nullptr;

    // 기존 타일 제거
    for (ATileActor* Tile : GridTiles)
        if (IsValid(Tile)) Tile->Destroy();

    GridTiles.Empty();

    SpawnTiles(CurrentWidth, CurrentHeight);
    CenterGridToOrigin(CurrentWidth, CurrentHeight);

    // 카메라 z위치 조정
    AdjustCamera(CurrentWidth, CurrentHeight);
}

void AGridManager::ResetTile()
{
    for (ATileActor* Tile : GridTiles)
    {
        if(Tile->CurrentState != ETileState::Obstacle)
            Tile->SetStateAndColor(ETileState::Unvisited);

        Tile->OpenNode.ResetOpenNode();

        Tile->PathParent = nullptr;
    }

    if(StartTile) StartTile->SetStateAndColor(ETileState::Start);
    if(EndTile) EndTile->SetStateAndColor(ETileState::Goal);
}

void AGridManager::UpdateAllTileColor()
{
    for (ATileActor* Tile : GridTiles)
        Tile->SetStateAndColor(Tile->CurrentState);

    if (StartTile) StartTile->SetStateAndColor(ETileState::Start);
    if (EndTile) EndTile->SetStateAndColor(ETileState::Goal);
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
                Tile->SetStateAndColor(ETileState::Unvisited);
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

bool AGridManager::SwapStartEnd()
{
    // 스왑 전 조건 체크
    if (!StartTile || !EndTile) return false;
    
    ATileActor* tmp = StartTile;
    StartTile = EndTile;
    EndTile = tmp;

    StartTile->SetStateAndColor(ETileState::Start);
    EndTile->SetStateAndColor(ETileState::Goal);
    return true;
}

// start는 정방향, end는 역방향
FVector AGridManager::GetStartEndDrawPathLoc(EParentDirection dir, bool bStart)
{
    if (bStart == false) {
        int8 t = static_cast<int8>(dir);
        t = (t + 4) % 8;
        dir = static_cast<EParentDirection>(t);
    }
    float half = TileSize / 2 + 5;
    switch (dir) {
    case EParentDirection::UU: {
        return FVector(half, 0.f, 2.f);
    }
    case EParentDirection::UR: {
        return FVector(half, half, 2.f);
    }
    case EParentDirection::RR: {
        return FVector(0.f, half, 2.f);
    }
    case EParentDirection::DR: {
        return FVector(half * -1, half, 2.f);
    }
    case EParentDirection::DD: {
        return FVector(half * -1, 0.f, 2.f);
    }
    case EParentDirection::DL: {
        return FVector(half * -1, half * -1, 2.f);
    }
    case EParentDirection::LL: {
        return FVector(0.f, half * -1, 2.f);
    }
    case EParentDirection::UL: {
        return FVector(half, half * -1, 2.f);
    }
    }

    return { 0.f, 0.f, 2.f };
}

void AGridManager::DrawPath(ATileActor* CurrentTile, bool bDraw)
{
    TArray<FVector> PathPoints;
    ATileActor* Tile = CurrentTile;
    EParentDirection prevDirection = EParentDirection::UU;

    while (Tile) {
        FVector offset = { 0.f, 0.f, 2.f };
        if (Tile == StartTile) offset = GetStartEndDrawPathLoc(prevDirection, true);
        else if(Tile == EndTile) offset = GetStartEndDrawPathLoc(Tile->PathParentDirection, false);
        
        PathPoints.Add(Tile->GetActorLocation() + offset);
        prevDirection = Tile->PathParentDirection;

        Tile->SetPath(bDraw); // 경로 여부에 따라 밝기 머티리얼 값 조정
        Tile = Tile->PathParent;
        if (bDraw && Tile) UE_LOG(LogTemp, Error, TEXT("next %d, %d"), Tile->GridX, Tile->GridY);
    }

    // 이전 메시 초기화
    PathMeshComp->ClearAllMeshSections();
    if (!bDraw || PathPoints.Num() < 2) return;

    TArray<FVector>   Vertices;
    TArray<int32>     Triangles;
    TArray<FVector>   Normals;
    TArray<FVector2D> UVs;

    // 선 너비
    float HalfW = PathLineWidth * 0.5f;

    // 각 포인트의 Left/Right 꼭짓점을 미리 계산
    TArray<FVector> LeftPts;
    TArray<FVector> RightPts;

    int32 Last = PathPoints.Num() - 1;

    for (int32 i = 0; i <= Last; i++)
    {
        FVector MiterRight;

        if (i == 0)
        {
            // 시작점: 첫 번째 구간 방향만 사용
            FVector Dir = (PathPoints[1] - PathPoints[0]).GetSafeNormal();
            if (Dir.IsNearlyZero()) continue;
            MiterRight = FVector::CrossProduct(FVector::UpVector, Dir).GetSafeNormal();
            if (MiterRight.IsNearlyZero()) MiterRight = FVector::RightVector;
        }
        else if (i == Last)
        {
            // 끝점: 마지막 구간 방향만 사용
            FVector Dir = (PathPoints[Last] - PathPoints[Last - 1]).GetSafeNormal();
            if (Dir.IsNearlyZero()) continue;
            MiterRight = FVector::CrossProduct(FVector::UpVector, Dir).GetSafeNormal();
            if (MiterRight.IsNearlyZero()) MiterRight = FVector::RightVector;
        }
        else
        {
            // 중간 꺾임 지점: Miter 벡터 계산
            FVector DirA = (PathPoints[i] - PathPoints[i - 1]).GetSafeNormal();
            FVector DirB = (PathPoints[i + 1] - PathPoints[i]).GetSafeNormal();

            FVector RightA = FVector::CrossProduct(FVector::UpVector, DirA).GetSafeNormal();
            FVector RightB = FVector::CrossProduct(FVector::UpVector, DirB).GetSafeNormal();

            FVector Miter = (RightA + RightB).GetSafeNormal();
            if (Miter.IsNearlyZero()) {
                // 180도 꺾임 → 이전 방향 그대로 사용
                MiterRight = RightA;
            }

            // 꺾임 각도에 따라 너비 보정
            float Dot = FVector::DotProduct(RightA, Miter);
            float Scale = FMath::IsNearlyZero(Dot) ? 1.f : 1.f / Dot;
            Scale = FMath::Clamp(Scale, 0.f, 3.f); // 날카로운 꺾임 제한

            MiterRight = Miter * Scale;
        }

        LeftPts.Add(PathPoints[i] - MiterRight * HalfW);
        RightPts.Add(PathPoints[i] + MiterRight * HalfW);
    }

    // 전체 경로 길이 먼저 계산 - 색상 변경에 사용
    float TotalLength = 0.f;
    for (int32 i = 0; i < Last; i++)
    {
        TotalLength += FVector::Dist(PathPoints[i], PathPoints[i + 1]);
    }

    // UV 설정 시 전체 길이로 나눠서 0~1 정규화
    float AccumLength = 0.f;
    for (int32 i = 0; i < Last; i++)
    {
        int32 Base = Vertices.Num();

        Vertices.Add(LeftPts[i]);       // v0
        Vertices.Add(RightPts[i]);      // v1
        Vertices.Add(LeftPts[i + 1]);     // v2
        Vertices.Add(RightPts[i + 1]);    // v3

        Normals.Add(FVector::UpVector);
        Normals.Add(FVector::UpVector);
        Normals.Add(FVector::UpVector);
        Normals.Add(FVector::UpVector);

        float SegLen = FVector::Dist(PathPoints[i], PathPoints[i + 1]);
        float UVStart = AccumLength / TotalLength;
        float UVEnd = (AccumLength + SegLen) / TotalLength;
        AccumLength += SegLen;

        UVs.Add(FVector2D(0.f, UVStart)); // v0
        UVs.Add(FVector2D(1.f, UVStart)); // v1
        UVs.Add(FVector2D(0.f, UVEnd));   // v2
        UVs.Add(FVector2D(1.f, UVEnd));   // v3

        Triangles.Add(Base + 0);
        Triangles.Add(Base + 1);
        Triangles.Add(Base + 2);

        Triangles.Add(Base + 1);
        Triangles.Add(Base + 3);
        Triangles.Add(Base + 2);
    }

    PathMeshComp->CreateMeshSection(
        0, Vertices, Triangles, Normals, UVs,
        TArray<FColor>(), TArray<FProcMeshTangent>(), false
    );
}