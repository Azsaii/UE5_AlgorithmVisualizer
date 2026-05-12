#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class ATileActor;

UCLASS()
class ALGORITHMVISUALIZER_API AGridManager : public AActor
{
	GENERATED_BODY()
	
public:	

	AGridManager();

    // 블루프린트에서 BP_TileActor를 할당
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid")
    TSubclassOf<ATileActor> TileClass;

    // 타일 하나의 월드 크기 (간격 포함)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid")
    float TileSize = 110.f; // 큐브 100 + 간격 10

    // 스폰된 타일 전체 (GridX + GridY * Width = Index)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    TArray<ATileActor*> GridTiles;

    // 현재 그리드 정보
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    int32 CurrentWidth = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    int32 CurrentHeight = 0;

    // 그리드 생성 시 호출
    void GenerateGrid(int32 NewWidth, int32 NewHeight);

    // 그리드 재생성 시 호출
    void ReGenerateGrid();

    // 인덱스로 타일 접근
    ATileActor* GetTile(int32 X, int32 Y) const;

private:
    void AdjustCamera(int32 Width, int32 Height);

    void SpawnTiles(int32 Width, int32 Height);
    void CenterGridToOrigin(int32 Width, int32 Height);
};
