#include "ControlPanelWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GridManager.h"

void UControlPanelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 기본값 설정
    TextBox_Width->SetText(FText::FromString(TEXT("10")));
    TextBox_Height->SetText(FText::FromString(TEXT("10")));

    Button_Apply->OnClicked.AddDynamic(this, &UControlPanelWidget::OnApplyClicked);

    Text_Status->SetText(FText::FromString(TEXT("Enter grid size and click Apply")));
}

void UControlPanelWidget::OnApplyClicked()
{
    if (!GridManager) { UE_LOG(LogTemp, Warning, TEXT("GridManager reference is null")); return; }

    // 텍스트를 정수로 변환 + 범위 클램프
    int32 W = FMath::Clamp(FCString::Atoi(*TextBox_Width->GetText().ToString()), 2, 100);
    int32 H = FMath::Clamp(FCString::Atoi(*TextBox_Height->GetText().ToString()), 2, 100);

    // 범위를 벗어난 값 입력 시 자동 보정해서 다시 표시
    TextBox_Width->SetText(FText::FromString(FString::FromInt(W)));
    TextBox_Height->SetText(FText::FromString(FString::FromInt(H)));

    GridManager->RegenerateGrid(W, H);
    UpdateStatusText(W, H);
}

void UControlPanelWidget::UpdateStatusText(int32 W, int32 H)
{
    FString Msg = FString::Printf(TEXT("Grid generated: %d x %d  (%d tiles)"), W, H, W * H);
    Text_Status->SetText(FText::FromString(Msg));
}