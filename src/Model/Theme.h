#pragma once
#include <imgui.h>

namespace Model
{
    struct RenderContext
    {
        ImVec4 bgColor          = ImVec4(0.15f, 0.16f, 0.18f, 1.0f);
        ImVec4 bgColor2         = ImVec4(0.20f, 0.22f, 0.25f, 0.8f);
        ImVec4 borderColor      = ImVec4(0.30f, 0.32f, 0.35f, 1.0f);
        ImVec4 textColor        = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
        ImVec4 mutedTextColor   = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
        ImVec4 placeholderColor = ImVec4(0.50f, 0.52f, 0.55f, 1.0f);
        ImVec4 bgColor3         = ImVec4(0.15f, 0.16f, 0.18f, 1.0f);

        bool roundInnerCorners = true;
        bool roundOuterCorners = true;
        bool border = false;
        float innerRounding = 6.0f;
        float outerRounding = 0.0f;
        float borderWidth = 1.0f;
        ImVec2 framePadding = ImVec2(12.0f, 8.0f);
        ImVec2 innerWindowPadding = ImVec2{11.0f, 11.0f};
        ImVec2 outerWindowPadding = ImVec2{0.0f, 0.0f};

        static constexpr size_t countStyleVar = 4;
        static constexpr size_t countStyleColor = 6;
    };

    enum class WidgetType
    {
        NONE,
        SLIDER,
        BUTTON,
        INPUT_TEXT,
        TEXT
    };

    struct WidgetStyle
    {
        WidgetType type = WidgetType::NONE;

        ImVec4 background;
        ImVec4 hovered;
        ImVec4 element;         // e.g. slider grab
        ImVec4 active;          // e.g. grabbing the grab / pressing button
        ImVec4 fontColor;

        float rounding1 = 0.0f; // widget rounding
        float rounding2 = 0.0f; // e.g. slider grab rounding

        unsigned int fontSize = 10;
    };

    struct Theme
    {
        RenderContext globalContext{};
        bool customTextInputField = false;
        RenderContext textInputField;


        // map to allow changing style for specific window. WindowName -> windowStyle
    };
}
