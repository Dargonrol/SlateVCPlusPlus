#include "TitleBar.h"

#include <imgui.h>

namespace UI::Window
{
    TitleBar::TitleBar()
    {
        customRenderImplementation = true;
        windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
        windowTitle_ = "Title Bar";
        autoResizeHorizontal = true;
        floating = false;
    }

    void TitleBar::PreRender(const Model::Theme& theme, const Model::VirtualViewport& viewport)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));
    }

    void TitleBar::RenderContent(const Model::Theme& theme, const Model::VirtualViewport& viewport)
    {
        if (resizable)
            MakeWindowResizable({1000, 1000}, {100, 100}, 3);
        if (draggable)
            MakeWindowDraggable();

        ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   ImVec4(0.85f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(0.95f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text,            ImVec4(0.9f, 0.9f, 0.9f, 1.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,    ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign,  ImVec2{0.5f, 0.5f});

        const float buttonWidth = size_.y;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - buttonWidth);
        if (ImGui::Button("X##CloseBtn", ImVec2(buttonWidth, buttonWidth)))
        {
            glfwSetWindowShouldClose(Core::Application::Get().GetWindow(), GLFW_TRUE);
        }

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);
    }

    void TitleBar::PostRender(const Model::Theme& theme, const Model::VirtualViewport& viewport)
    {
        ImGui::PopStyleVar(1);
    }

    static bool isDraggingWindowDrag;
    static double startingMouseXDrag;
    static double startingMouseYDrag;
    void TitleBar::MakeWindowDraggable()
    {
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (!ImGui::IsAnyItemActive() && !ImGui::IsItemHovered())
            {
                isDraggingWindowDrag = true;
                GLFWwindow* window = Core::Application::Get().GetWindow();
                glfwGetCursorPos(window, &startingMouseXDrag, &startingMouseYDrag);
                resizable = false;
            }

        }

        if (isDraggingWindowDrag)
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                GLFWwindow* window = Core::Application::Get().GetWindow();

                double cursorX, cursorY;
                glfwGetCursorPos(window, &cursorX, &cursorY);

                const double deltaMouseX = startingMouseXDrag - cursorX;
                const double deltaMouseY = startingMouseYDrag - cursorY;

                int windowPosX, windowPosY;
                glfwGetWindowPos(window, &windowPosX, &windowPosY);

                glfwSetWindowPos(window,  static_cast<int>(windowPosX - deltaMouseX), static_cast<int>(windowPosY - deltaMouseY));
            }
            else
            {
                isDraggingWindowDrag = false;
                resizable = true;
            }
        }
    }

    namespace
    {
        GLFWcursor* GetCursor(const TitleBar::ResizeMode mode)
        {
            static GLFWcursor* nsCursor   = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
            static GLFWcursor* ewCursor   = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
            static GLFWcursor* nwseCursor = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
            static GLFWcursor* neswCursor = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);

            switch (mode)
            {
            case TitleBar::ResizeMode::N:
            case TitleBar::ResizeMode::S:   return nsCursor;
            case TitleBar::ResizeMode::E:
            case TitleBar::ResizeMode::W:   return ewCursor;
            case TitleBar::ResizeMode::NW:
            case TitleBar::ResizeMode::SE:  return nwseCursor;
            case TitleBar::ResizeMode::NE:
            case TitleBar::ResizeMode::SW:  return neswCursor;
            default:                        return nullptr;
            }
        }
    }

    void TitleBar::MakeWindowResizable(const ImVec2& maxSize, const ImVec2& minSize, const int detectionMargin)
    {
        GLFWwindow* window = Core::Application::Get().GetWindow();

        int windowPosX, windowPosY;
        int windowSizeX, windowSizeY;
        double relCursorPosX, relCursorPosY;

        glfwGetWindowPos(window, &windowPosX, &windowPosY);
        glfwGetWindowSize(window, &windowSizeX, &windowSizeY);
        glfwGetCursorPos(window, &relCursorPosX, &relCursorPosY);

        const ImVec2 windowPos = { static_cast<float>(windowPosX), static_cast<float>(windowPosY) };
        const ImVec2 windowSize = { static_cast<float>(windowSizeX), static_cast<float>(windowSizeY) };
        const ImVec2 cursorPos = { static_cast<float>(windowPosX + relCursorPosX), static_cast<float>(windowPosY + relCursorPosY) };

        UpdateResizeState(window, cursorPos, windowPos, windowSize, detectionMargin);

        if (resizeMode != ResizeMode::None)
        {
            draggable = false;
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                const ImVec2 deltaMouse = { startMouseResizePos.x - cursorPos.x, startMouseResizePos.y - cursorPos.y };
                prevWindowSize = windowSize;

                ImVec2 newSize = startWindowSize;
                ImVec2 newPos = windowPos;

                switch (resizeMode)
                {
                case ResizeMode::E:
                    newSize.x = startWindowSize.x - deltaMouse.x;
                    break;
                case ResizeMode::S:
                    newSize.y = startWindowSize.y - deltaMouse.y;
                    break;
                case ResizeMode::SE:
                    newSize.x = startWindowSize.x - deltaMouse.x;
                    newSize.y = startWindowSize.y - deltaMouse.y;
                    break;
                case ResizeMode::W:
                    newSize.x = startWindowSize.x + deltaMouse.x;
                    newPos.x = startWindowPos.x - deltaMouse.x;
                    break;
                case ResizeMode::N:
                    newSize.y = startWindowSize.y + deltaMouse.y;
                    newPos.y = startWindowPos.y - deltaMouse.y;
                    break;
                case ResizeMode::NW:
                    newSize.x = startWindowSize.x + deltaMouse.x;
                    newSize.y = startWindowSize.y + deltaMouse.y;
                    newPos.x = startWindowPos.x - deltaMouse.x;
                    newPos.y = startWindowPos.y - deltaMouse.y;
                    break;
                case ResizeMode::NE:
                    newSize.x = startWindowSize.x - deltaMouse.x;
                    newSize.y = startWindowSize.y + deltaMouse.y;
                    newPos.y = startWindowPos.y - deltaMouse.y;
                    break;
                case ResizeMode::SW:
                    newSize.x = startWindowSize.x + deltaMouse.x;
                    newSize.y = startWindowSize.y - deltaMouse.y;
                    newPos.x = startWindowPos.x - deltaMouse.x;
                    break;
                default:
                    break;
                }

                constrainCheck(windowSize, windowPos, newSize, newPos, maxSize, minSize);

                glfwSetWindowSize(window, static_cast<int>(newSize.x), static_cast<int>(newSize.y));

                if (resizeMode == ResizeMode::W || resizeMode == ResizeMode::N || resizeMode == ResizeMode::NW || resizeMode == ResizeMode::NE || resizeMode == ResizeMode::SW)
                {
                    glfwSetWindowPos(window, static_cast<int>(newPos.x), static_cast<int>(newPos.y));
                }
            }
            else
            {
                resizeMode = ResizeMode::None;
            }
        }
        else
            draggable = true;
    }

    void TitleBar::UpdateResizeState(GLFWwindow* window, const ImVec2& cursorPos, const ImVec2& windowPos, const ImVec2& windowSize, const int detectionMargin)
    {
        if (resizeMode != ResizeMode::None)
            return;

        const bool isMouseDown = IsMouseLeftDown(window);
        ResizeMode detectedMode = ResizeMode::None;

        const bool onLeft   = cursorPos.x <= windowPos.x + detectionMargin;
        const bool onRight  = cursorPos.x >= windowPos.x + windowSize.x - detectionMargin;
        const bool onTop    = cursorPos.y <= windowPos.y + detectionMargin;
        const bool onBottom = cursorPos.y >= windowPos.y + windowSize.y - detectionMargin;

        // corners
        if      (onTop && onLeft)     detectedMode = ResizeMode::NW;
        else if (onTop && onRight)    detectedMode = ResizeMode::NE;
        else if (onBottom && onLeft)  detectedMode = ResizeMode::SW;
        else if (onBottom && onRight) detectedMode = ResizeMode::SE;
        // edges
        else if (onLeft)              detectedMode = ResizeMode::W;
        else if (onRight)             detectedMode = ResizeMode::E;
        else if (onTop)               detectedMode = ResizeMode::N;
        else if (onBottom)            detectedMode = ResizeMode::S;

        if (detectedMode != ResizeMode::None)
        {
            glfwSetCursor(window, GetCursor(detectedMode));
            if (isMouseDown)
            {
                resizeMode = detectedMode;
                startMouseResizePos = cursorPos;
                startWindowSize = windowSize;
                startWindowPos = windowPos;
            }
        }
        else
        {
            glfwSetCursor(window, nullptr);
        }
    }

    void TitleBar::constrainCheck(const ImVec2& windowSize, const ImVec2& windowPos, ImVec2& newSize, ImVec2& newPos, const ImVec2& maxSize, const ImVec2& minSize)
    {
        if (newSize.x > maxSize.x || newSize.x < minSize.x)
        {
            newSize = { windowSize.x, newSize.y };
            newPos = { windowPos.x, newPos.y };
        }
        if (newSize.y > maxSize.y || newSize.y < minSize.y)
        {
            newSize = { newSize.x, windowSize.y };
            newPos = { newSize.x, windowPos.y };
        }
    }

    bool TitleBar::IsMouseLeftDown(GLFWwindow* window)
    {
        return ImGui::IsMouseDown(ImGuiMouseButton_Left);
    }
}
