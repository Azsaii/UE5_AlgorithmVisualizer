#include "ControlPanelWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Fonts/FontMeasure.h"
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
    int32 W = FMath::Clamp(FCString::Atoi(*TextBox_Width->GetText().ToString()), MinGridAmount, MaxGridAmount);
    int32 H = FMath::Clamp(FCString::Atoi(*TextBox_Height->GetText().ToString()), MinGridAmount, MaxGridAmount);

    // 범위를 벗어난 값 입력 시 자동 보정해서 다시 표시
    TextBox_Width->SetText(FText::FromString(FString::FromInt(W)));
    TextBox_Height->SetText(FText::FromString(FString::FromInt(H)));

    GridManager->GenerateGrid(W, H);

    FString Msg = FString::Printf(TEXT("Grid generated: %d x %d  (%d tiles)"), W, H, W * H);
    UpdateStatusText(Msg);
}

void UControlPanelWidget::UpdateStatusText(const FString& Message)
{
    // 텍스트 변경
    Text_Status->SetText(FText::FromString(Message));

    // 폰트 정보로 텍스트 너비 직접 계산
    const FSlateFontInfo& FontInfo = Text_Status->GetFont();
    FVector2D TextSize = FVector2D::ZeroVector;

    TSharedRef<FSlateFontMeasure> FontMeasure =
        FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

    TextSize = FontMeasure->Measure(Message, FontInfo);

    float PaddingX = 70.f;
    float PaddingY = 20.f;

    FVector2D NewSize = FVector2D(TextSize.X + PaddingX, TextSize.Y + PaddingY);

    UCanvasPanelSlot* BackgroundSlot = Cast<UCanvasPanelSlot>(StatusBackground->Slot);
    if (BackgroundSlot) BackgroundSlot->SetSize(NewSize);

}