#pragma once
#include "IWindow.h"

#include <imgui.h>

namespace UI::Window
{
    class ChatWindow : public IWindow
    {
    protected:
        void RenderContent(const Model::Theme& theme, const Model::VirtualViewport& viewport) override
        {
            ImGui::Text("Chat Window");

            float value = 0.0f;

            ImGui::PushStyleColor(ImGuiCol_FrameBg,          ImVec4(1.15f, 0.15f, 0.18f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,   ImVec4(0.25f, 0.25f, 0.30f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_SliderGrab,       ImVec4(0.20f, 1.60f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.10f, 0.45f, 0.90f, 1.0f));

            ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 4.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

            ImGui::SliderFloat("##slider", &value, 0.0f, 100.0f, "Value: %.1f");

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(4);
        }
    };
}
