// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParentDirection.h"
#include "OpenNode.h"
#include "TileActor.generated.h"

class UWidgetComponent;

enum class ETileState : uint8
{
	Unvisited,
	Obstacle,
	Start,
	Goal,

	Open,    // 탐색 후보
	Closed,  // 탐색 완료
	Path,     // 최종 경로

	Test,
};

UCLASS()
class ALGORITHMVISUALIZER_API ATileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATileActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* LabelRoot;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* LabelWidget;

	ETileState CurrentState = ETileState::Unvisited;

	// 그리드 상 좌표
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridX = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridY = 0;
	
	void SetStateAndColor(ETileState NewState);

	// 호버 상태 변경
	void SetHovered(bool bHovered);

	// 경로 상에 있을 때 머티리얼 밝기 값 변경
	void SetPath(bool bPath);

	// 경로 상 이전 타일
	UPROPERTY()
	ATileActor* PathParent = nullptr;

	// 경로 상 부모로 이어진 방향
	EParentDirection PathParentDirection;

	// 상태별 색상
	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Unvisited;

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Obstacle;

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Start;

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Goal;

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Open;

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Closed;

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Path;

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Test;

	UPROPERTY(EditDefaultsOnly, Category = "Label|Colors")
	FLinearColor Color_LabelStart;

	UPROPERTY(EditDefaultsOnly, Category = "Label|Colors")
	FLinearColor Color_LavelGoal;

	UPROPERTY(EditDefaultsOnly, Category = "Label")
	UFont* LabelFont;

	// A*, JPS 에서 사용
	FOpenNode OpenNode;

	// 라벨 표시/숨김
	void ShowLabel(const FString& Text, FLinearColor Color);
	void HideLabel();

protected:
	virtual void PostInitializeComponents() override;
private:

	// 타일 상태 변경
	FLinearColor StateToColor(ETileState State);

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Materials")
	UMaterialInterface* TileMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;
};
