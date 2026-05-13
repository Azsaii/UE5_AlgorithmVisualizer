// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AlgorithmGameMode.generated.h"

class AGridManager;
class AGraphManager;
class UControlPanelWidget;

UCLASS()
class ALGORITHMVISUALIZER_API AAlgorithmGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditDefaultsOnly, Category = "Grid")
    TSubclassOf<AGridManager> GridManagerClass;

    UPROPERTY(EditDefaultsOnly, Category = "Graph")
    TSubclassOf<AGraphManager> GraphManagerClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UControlPanelWidget> ControlPanelClass;

protected:
    virtual void BeginPlay() override;

private:

    AGridManager* GridManager = nullptr;
    AGraphManager* GraphManager = nullptr;
    UControlPanelWidget* ControlPanel = nullptr;
};
