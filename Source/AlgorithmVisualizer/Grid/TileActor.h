// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileActor.generated.h"

UENUM()
enum class ETileState : uint8
{
	Unvisited,
	Obstacle,
	Start,
	Goal,

	Open,    // 탐색 후보
	Closed,  // 탐색 완료
	Path     // 최종 경로
};

UCLASS()
class ALGORITHMVISUALIZER_API ATileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp;

	ETileState CurrentState = ETileState::Unvisited;

	// 그리드 상 좌표
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridX = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridY = 0;

	void SetState(ETileState NewState, bool bColorChange = true);

	// 호버 상태 변경
	void SetHovered(bool bHovered);

	ATileActor* PathParent = nullptr;

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
