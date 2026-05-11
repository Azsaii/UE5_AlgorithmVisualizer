// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlPanelWidget.generated.h"

class UEditableTextBox;
class UButton;
class AGridManager;
class UImage;
class UTextBlock;

UCLASS()
class ALGORITHMVISUALIZER_API UControlPanelWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // BindWidget = UMG 에디터의 위젯 이름과 반드시 일치해야 함
    UPROPERTY(meta = (BindWidget)) UEditableTextBox* TextBox_Width;
    UPROPERTY(meta = (BindWidget)) UEditableTextBox* TextBox_Height;
    UPROPERTY(meta = (BindWidget)) UButton* Button_Apply;

    UPROPERTY(meta = (BindWidget)) UImage* StatusBackground;

    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Status;

    // GridManager 참조 (BeginPlay에서 주입)
    UPROPERTY(BlueprintReadWrite, Category = "Grid")
    AGridManager* GridManager = nullptr;

    void UpdateStatusText(const FString& Message);
protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION() void OnApplyClicked();

    UPROPERTY(EditDefaultsOnly, Category = "Grid")
    int32 MinGridAmount = 2;

    UPROPERTY(EditDefaultsOnly, Category = "Grid")
    int32 MaxGridAmount = 50;
};
