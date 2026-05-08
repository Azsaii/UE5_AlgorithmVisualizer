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

	// 상태 변경 (머티리얼 자동 업데이트)
	UFUNCTION(BlueprintCallable, Category = "Tile")
	void SetState(ETileState NewState);

	UFUNCTION(BlueprintPure, Category = "Tile")
	ETileState GetState() const { return CurrentState; }

private:

	ETileState CurrentState = ETileState::Unvisited;

	// 상태별 머티리얼 (블루프린트에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Tile|Materials")
	UMaterialInterface* Mat_Unvisited;

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Materials")
	UMaterialInterface* Mat_Obstacle;

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Materials")
	UMaterialInterface* Mat_Start;

	UPROPERTY(EditDefaultsOnly, Category = "Tile|Materials")
	UMaterialInterface* Mat_Goal;

};
