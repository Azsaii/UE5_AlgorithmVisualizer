// Fill out your copyright notice in the Description page of Project Settings.


#include "TileActor.h"

ATileActor::ATileActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

}

void ATileActor::SetState(ETileState NewState)
{
    CurrentState = NewState;

    // 상태에 따라 머티리얼 교체
    UMaterialInterface* TargetMat = nullptr;
    switch (NewState)
    {
    case ETileState::Unvisited: TargetMat = Mat_Unvisited; break;
    case ETileState::Obstacle:  TargetMat = Mat_Obstacle;  break;
    case ETileState::Start:     TargetMat = Mat_Start;     break;
    case ETileState::Goal:      TargetMat = Mat_Goal;      break;
    }

    if (TargetMat)
        MeshComp->SetMaterial(0, TargetMat);
}

