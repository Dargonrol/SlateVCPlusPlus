#pragma once
#include "IWindow.h"

#include "Core/Application.h"

namespace UI::Window
{
    class TitleBar :public IWindow
    {
    public:
        enum class ResizeMode { None, N, S, E, W, NW, NE, SW, SE};

        TitleBar();

    protected:
        void PreRender(const Model::Theme& theme, const Model::VirtualViewport& viewport) override;

        void RenderContent(const Model::Theme& theme, const Model::VirtualViewport& viewport) override;

        void PostRender(const Model::Theme& theme, const Model::VirtualViewport& viewport) override;

    private:
        void MakeWindowDraggable();
        void MakeWindowResizable(const ImVec2& maxSize, const ImVec2& minSize, int detectionMargin = 5);

        bool IsMouseLeftDown(GLFWwindow* window);
        void UpdateResizeState(GLFWwindow* window, const ImVec2& cursorPos, const ImVec2& windowPos, const ImVec2& windowSize, int detectionMargin);
        void constrainCheck(const ImVec2& windowSize, const ImVec2& windowPos, ImVec2& newSize, ImVec2& newPos, const ImVec2& maxSize, const ImVec2& minSize);

    private:
        bool draggable = true;
        bool resizable = true;

        ImVec2 startMouseResizePos = {0, 0};
        ImVec2 startWindowSize = {0 , 0};
        ImVec2 startWindowPos = { 0, 0};
        ImVec2 prevWindowSize = { 0, 0};
        ResizeMode resizeMode = ResizeMode::None;
    };
}