// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GraphManager.generated.h"

class ANodeActor;
class AEdgeActor;

UCLASS()
class ALGORITHMVISUALIZER_API AGraphManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGraphManager();
};
