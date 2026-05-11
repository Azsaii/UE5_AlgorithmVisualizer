// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileActor.generated.h"

UENUM(BlueprintType)
enum class ETileState : uint8
{
	Unvisited,
	Obstacle,
	Start,
	Goal
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

	// 그리드 상 좌표
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridX = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridY = 0;

	UFUNCTION(BlueprintPure, Category = "Tile")
	ETileState GetState() const { return CurrentState; }

	void SetState(ETileState NewState);

	// 호버 상태 변경
	void SetHovered(bool bHovered);

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

private:

	ETileState CurrentState = ETileState::Unvisited;

	// 타일 상태 변경
	FLinearColor StateToColor(ETileState State);

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Materials")
	UMaterialInterface* TileMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	// 상태별 색상
	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Unvisited = FLinearColor(0.2f, 0.2f, 0.2f);

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Obstacle = FLinearColor(0.05f, 0.05f, 0.05f);

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Start = FLinearColor(0.f, 1.f, 0.f);

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Colors")
	FLinearColor Color_Goal = FLinearColor(1.f, 0.f, 0.f);

};
