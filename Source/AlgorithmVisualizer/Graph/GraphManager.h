// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ControlPanelWidget.h"
#include "GraphManager.generated.h"

class ANodeActor;
class AEdgeActor;

UCLASS()
class ALGORITHMVISUALIZER_API AGraphManager : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AGraphManager();

	// 그래프 생성 시 호출
	void GenerateGraph(int32 NodeAmount);

	// 그래프 재생성 시 호출
	void ReGenerateGraph();

	UControlPanelWidget* ControlPanel = nullptr;
};
