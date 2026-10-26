#include "MainView.h"

#include <imgui.h>
#include <algorithm>

#include "Model/Theme.h"

namespace UI::View
{
    void MainView::RecalculateWorkspace(const ImVec2 size)
    {
        if (size.x == 0.0f && size.y == 0.0f)
        {
            contentArea_.WorkSize = viewport_->WorkSize;
            contentArea_.WorkPos = viewport_->WorkPos;
        } else
        {
            contentArea_.WorkSize = size;
            contentArea_.WorkPos = viewport_->WorkPos;
        }

        contentArea_.WorkPos.y += titleBar_.GetWindowSize().y + themeController_.GetCurrentTeme().globalContext.outerWindowPadding.y * 1.0f;
        contentArea_.WorkSize.y -= titleBar_.GetWindowSize().y + themeController_.GetCurrentTeme().globalContext.outerWindowPadding.y * 1.0f;
    }

    void MainView::OnViewportResize(const ImVec2& oldSize, const ImVec2& newSize)
    {
        RecalculateWorkspace(newSize);

        const ImVec2 minSize{contentArea_.WorkSize.x - themeController_.GetCurrentTeme().globalContext.outerWindowPadding.x * 2.0f, FLT_MIN};
        const ImVec2 maxSize{contentArea_.WorkSize.x - themeController_.GetCurrentTeme().globalContext.outerWindowPadding.x * 2.0f, FLT_MAX};
        titleBar_.SetCustomConstrains(minSize, maxSize);

        titleBar_.OnViewportResize(oldSize, newSize);
        navBar_.OnViewportResize(oldSize, newSize);
        serverAndStatus_.OnViewportResize(oldSize, newSize);
        contextList_.OnViewportResize(oldSize, newSize);
        chatWindow_.OnViewportResize(oldSize, newSize);
    }

    void MainView::Init()
    {
        viewport_ = ImGui::GetMainViewport();

        const Model::Theme& theme = themeController_.GetCurrentTeme();

        RecalculateWorkspace();

        // INIT titlebar
        {
            const ImVec2 titleBarSize = {viewport_->WorkSize.x - theme.globalContext.outerWindowPadding.x * 2.0f, 20.0f};
            titleBar_.SetWindowSize(titleBarSize);
            titleBar_.SetWindowPos({theme.globalContext.outerWindowPadding.x, theme.globalContext.outerWindowPadding.y});
            const ImVec2 minSize{contentArea_.WorkSize.x - theme.globalContext.outerWindowPadding.x * 2.0f, FLT_MIN};
            const ImVec2 maxSize{contentArea_.WorkSize.x - theme.globalContext.outerWindowPadding.x * 2.0f, FLT_MAX};
            titleBar_.SetCustomConstrains(minSize, maxSize);
        }

        RecalculateWorkspace();

        // INIT navbar window
        {
            navBar_.SetWindowSize({60.0f, contentArea_.WorkSize.y});
            constexpr ImVec2 minSize{60.0f, FLT_MIN};
            constexpr ImVec2 maxSize{120.0f, FLT_MAX};
            navBar_.SetCustomConstrains(minSize, maxSize);
            navBar_.autoResizeVertical = true;
            //navBar_.showDebugChildArea = true;
            //navBar_.showDebugResizeArea = true;
            navBar_.resizableFlags = Resizable::ResizableRight;
        }

        // INIT server and status window
        {
            serverAndStatus_.SetWindowSize({200.0f, contentArea_.WorkSize.y});
            constexpr ImVec2 minSize{200.0f, FLT_MIN};
            constexpr ImVec2 maxSize{340.0f, FLT_MAX};
            serverAndStatus_.SetCustomConstrains(minSize, maxSize);
            serverAndStatus_.autoResizeVertical = true;
            //serverAndStatus_.showDebugChildArea = true;
            //serverAndStatus_.showDebugResizeArea = true;
            serverAndStatus_.resizableFlags = Resizable::ResizableRight;
        }

        // INIT context list window
        {
            contextList_.SetWindowSize({240.0f, contentArea_.WorkSize.y});
            constexpr ImVec2 minSize{240.0f, FLT_MIN};
            constexpr ImVec2 maxSize{300.0f, FLT_MAX};
            contextList_.SetCustomConstrains(minSize, maxSize);
            contextList_.autoResizeVertical = true;
            //contextList_.showDebugChildArea = true;
            //contextList_.showDebugResizeArea = true;
            contextList_.resizableFlags = Resizable::ResizableLeft;
        }

        // INIT chat window
        {
            constexpr ImVec2 minSize{10.0f, FLT_MIN};
            constexpr ImVec2 maxSize{FLT_MAX, FLT_MAX};
            chatWindow_.SetCustomConstrains(minSize, maxSize);
            chatWindow_.autoResizeVertical = true;
            //chatWindow_.showDebugChildArea = true;
            chatWindow_.floating = false;
        }
    }

    void MainView::Update()
    {
        viewport_ = ImGui::GetMainViewport();

        if (viewport_->WorkSize.x <= 0 || viewport_->WorkSize.y <= 0)
            return;

        RecalculateWorkspace();
    }

    MainView::MainView(NavCallback navCallback, const Service::ThemeController& themeController) : navCallback_(std::move(navCallback)), themeController_(themeController) {}

    void MainView::Render()
    {
        const Model::Theme& theme = themeController_.GetCurrentTeme();

        DrawTitleBar(theme);
        DrawNavBar(theme);
        DrawServerAndStatusBar(theme);
        DrawContextList(theme);
        DrawChatWindow(theme);
    }

    void MainView::DrawTitleBar(const Model::Theme& theme)
    {
        titleBar_.Render(theme, contentArea_);
    }

    void MainView::DrawNavBar(const Model::Theme& theme)
    {
        const ImVec2 windowPosition{contentArea_.WorkPos.x + theme.globalContext.outerWindowPadding.x, contentArea_.WorkPos.y + theme.globalContext.outerWindowPadding.y};
        navBar_.SetWindowPos(windowPosition);
        navBar_.Render(theme, contentArea_);
    }

    void MainView::DrawServerAndStatusBar(const Model::Theme& theme)
    {
        const ImVec2 windowPosition{contentArea_.WorkPos.x + navBar_.GetWindowSize().x + theme.globalContext.outerWindowPadding.x * 2.0f, contentArea_.WorkPos.y + theme.globalContext.outerWindowPadding.y};
        serverAndStatus_.SetWindowPos(windowPosition);

        serverAndStatus_.Render(theme, contentArea_);
    }

    void MainView::DrawContextList(const Model::Theme& theme)
    {
        const ImVec2 windowPosition{contentArea_.WorkPos.x + contentArea_.WorkSize.x - contextList_.GetWindowSize().x - theme.globalContext.outerWindowPadding.x, contentArea_.WorkPos.y + theme.globalContext.outerWindowPadding.y};
        contextList_.SetWindowPos(windowPosition);

        contextList_.Render(theme, contentArea_);
    }

    void MainView::DrawChatWindow(const Model::Theme& theme)
    {
        const float calculatedWidth = contentArea_.WorkSize.x
                                    - navBar_.GetWindowSize().x
                                    - serverAndStatus_.GetWindowSize().x
                                    - contextList_.GetWindowSize().x
                                    - theme.globalContext.outerWindowPadding.x * 5.0f;

        const ImVec2 windowSize{calculatedWidth, contentArea_.WorkSize.y - theme.globalContext.outerWindowPadding.y * 2.0f};
        const ImVec2 windowPosition{
            viewport_->WorkPos.x + navBar_.GetWindowSize().x + serverAndStatus_.GetWindowSize().x + theme.globalContext.outerWindowPadding.x * 3.0f,
            contentArea_.WorkPos.y + theme.globalContext.outerWindowPadding.y
        };

        chatWindow_.SetWindowSize(windowSize);
        chatWindow_.SetWindowPos(windowPosition);

        chatWindow_.Render(theme, contentArea_);
    }
}