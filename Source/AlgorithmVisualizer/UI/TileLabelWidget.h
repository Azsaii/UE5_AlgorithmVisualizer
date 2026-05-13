// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TileLabelWidget.generated.h"

class UTextBlock;

UCLASS()
class ALGORITHMVISUALIZER_API UTileLabelWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetLabelTextAndColor(const FString& Text, FLinearColor Color);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LabelText;
};
