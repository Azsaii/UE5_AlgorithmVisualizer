#include "TileLabelWidget.h"
#include "Components/TextBlock.h"

void UTileLabelWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UTileLabelWidget::SetLabelTextAndColor(const FString& Text, FLinearColor Color)
{
    if (LabelText)
    {
        LabelText->SetText(FText::FromString(Text));
        LabelText->SetColorAndOpacity(FSlateColor(Color));
    }
}