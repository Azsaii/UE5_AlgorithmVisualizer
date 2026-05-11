// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class TileActor;

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

    // 현재 그리드 정보
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    int32 CurrentWidth = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    int32 CurrentHeight = 0;

    // 스폰된 타일 전체 (GridX + GridY * Width = Index)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    TArray<ATileActor*> GridTiles;

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void AdjustCamera(int32 Width, int32 Height);

    // 위젯에서 그리드 생성 시 호출
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void RegenerateGrid(int32 NewWidth, int32 NewHeight);

    // 컨트롤러에서 그리드 초기화용으로 사용
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ResetGrid();

    // 인덱스로 타일 접근
    UFUNCTION(BlueprintPure, Category = "Grid")
    ATileActor* GetTile(int32 X, int32 Y) const;

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearGrid();

private:
    void SpawnTiles(int32 Width, int32 Height);
    void CenterGridToOrigin(int32 Width, int32 Height);
};
