#pragma once
#include "imgui.h"
#include "imgui_internal.h"

//###############学习#################
// 阅 - 1遍
//####################################

struct LayoutParams {
    // 窗口相关
    ImVec2 WindowPadding;
    ImVec2 WindowMinSize;
    float WindowRounding;
    float WindowBorderSize;
    float WindowTitleAlign;

    // 子窗口相关
    float ChildRounding;
    float ChildBorderSize;

    // 弹窗相关
    float PopupRounding;
    float PopupBorderSize;

    // 框架相关
    ImVec2 FramePadding;
    float FrameRounding;
    float FrameBorderSize;

    // 控件间距
    ImVec2 ItemSpacing;
    ImVec2 ItemInnerSpacing;
    ImVec2 TouchExtraPadding;
    float IndentSpacing;

    // 滚动条
    float ScrollbarSize;
    float ScrollbarRounding;

    // 滑块
    float GrabMinSize;
    float GrabRounding;  // 滑块圆角

    // 标签页
    float TabRounding;
    float TabBorderSize;
    // TabMinWidthForCloseButton 已移除（旧版本ImGui没有这个成员）

    // 其他
    ImVec2 DisplayWindowPadding;
    ImVec2 DisplaySafeAreaPadding;
    float MouseCursorScale;
    ImVec2 CellPadding;
    ImVec2 ButtonTextAlign;
    ImVec2 SelectableTextAlign;
};

class ThemeManager {
public:
    static ThemeManager& Get() {
        static ThemeManager instance;
        return instance;
    }

    // 保存当前布局参数（包括所有形态样式）
    LayoutParams SaveLayoutParams(const ImGuiStyle& style) {
        LayoutParams params;
        // 窗口相关
        params.WindowPadding = style.WindowPadding;
        params.WindowMinSize = style.WindowMinSize;
        params.WindowRounding = style.WindowRounding;
        params.WindowBorderSize = style.WindowBorderSize;
        params.WindowTitleAlign = style.WindowTitleAlign.x;

        // 子窗口相关
        params.ChildRounding = style.ChildRounding;
        params.ChildBorderSize = style.ChildBorderSize;

        // 弹窗相关
        params.PopupRounding = style.PopupRounding;
        params.PopupBorderSize = style.PopupBorderSize;

        // 框架相关
        params.FramePadding = style.FramePadding;
        params.FrameRounding = style.FrameRounding;
        params.FrameBorderSize = style.FrameBorderSize;

        // 控件间距
        params.ItemSpacing = style.ItemSpacing;
        params.ItemInnerSpacing = style.ItemInnerSpacing;
        params.TouchExtraPadding = style.TouchExtraPadding;
        params.IndentSpacing = style.IndentSpacing;

        // 滚动条
        params.ScrollbarSize = style.ScrollbarSize;
        params.ScrollbarRounding = style.ScrollbarRounding;

        // 滑块
        params.GrabMinSize = style.GrabMinSize;
        params.GrabRounding = style.GrabRounding;

        // 标签页
        params.TabRounding = style.TabRounding;
        params.TabBorderSize = style.TabBorderSize;
        // TabMinWidthForCloseButton 已移除

        // 其他
        params.DisplayWindowPadding = style.DisplayWindowPadding;
        params.DisplaySafeAreaPadding = style.DisplaySafeAreaPadding;
        params.MouseCursorScale = style.MouseCursorScale;
        params.CellPadding = style.CellPadding;
        params.ButtonTextAlign = style.ButtonTextAlign;
        params.SelectableTextAlign = style.SelectableTextAlign;

        return params;
    }

    // 恢复布局参数（恢复所有形态样式）
    void RestoreLayoutParams(ImGuiStyle& style, const LayoutParams& params) {
        // 窗口相关
        style.WindowPadding = params.WindowPadding;
        style.WindowMinSize = params.WindowMinSize;
        style.WindowRounding = params.WindowRounding;
        style.WindowBorderSize = params.WindowBorderSize;
        style.WindowTitleAlign.x = params.WindowTitleAlign;

        // 子窗口相关
        style.ChildRounding = params.ChildRounding;
        style.ChildBorderSize = params.ChildBorderSize;

        // 弹窗相关
        style.PopupRounding = params.PopupRounding;
        style.PopupBorderSize = params.PopupBorderSize;

        // 框架相关
        style.FramePadding = params.FramePadding;
        style.FrameRounding = params.FrameRounding;
        style.FrameBorderSize = params.FrameBorderSize;

        // 控件间距
        style.ItemSpacing = params.ItemSpacing;
        style.ItemInnerSpacing = params.ItemInnerSpacing;
        style.TouchExtraPadding = params.TouchExtraPadding;
        style.IndentSpacing = params.IndentSpacing;

        // 滚动条
        style.ScrollbarSize = params.ScrollbarSize;
        style.ScrollbarRounding = params.ScrollbarRounding;

        // 滑块
        style.GrabMinSize = params.GrabMinSize;
        style.GrabRounding = params.GrabRounding;

        // 标签页
        style.TabRounding = params.TabRounding;
        style.TabBorderSize = params.TabBorderSize;
        // TabMinWidthForCloseButton 已移除

        // 其他
        style.DisplayWindowPadding = params.DisplayWindowPadding;
        style.DisplaySafeAreaPadding = params.DisplaySafeAreaPadding;
        style.MouseCursorScale = params.MouseCursorScale;
        style.CellPadding = params.CellPadding;
        style.ButtonTextAlign = params.ButtonTextAlign;
        style.SelectableTextAlign = params.SelectableTextAlign;
    }

    // 应用主题（根据ID）
    void ApplyTheme(int themeID, bool keepLayout = true) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiIO& io = ImGui::GetIO();

        LayoutParams currentLayout;
        if (keepLayout) {
            currentLayout = SaveLayoutParams(style);
        }

        // 保存原始透明度
        float originalAlpha = style.Alpha;

        // 清空样式但保留形态参数
        ImGuiStyle newStyle;

        switch (themeID) {
        case 0: // 深色主题
            newStyle = ImGuiStyle();
            ImGui::StyleColorsDark(&newStyle);
            break;

        case 1: // 浅色主题
            newStyle = ImGuiStyle();
            ImGui::StyleColorsLight(&newStyle);
            break;

        case 2: // 经典主题
            newStyle = ImGuiStyle();
            ImGui::StyleColorsClassic(&newStyle);
            break;

        case 3: // 正式主题
            newStyle = ImGuiStyle();
            ImGui::StyleColorsLight(&newStyle);

            newStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.96f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.40f, 0.60f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.50f, 0.75f, 1.0f);
            newStyle.Colors[ImGuiCol_Button] = ImVec4(0.30f, 0.55f, 0.75f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.60f, 0.85f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.45f, 0.65f, 1.0f);
            newStyle.Colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            newStyle.Colors[ImGuiCol_CheckMark] = ImVec4(0.25f, 0.55f, 0.80f, 1.0f);
            newStyle.Colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.55f, 0.75f, 1.0f);
            newStyle.Colors[ImGuiCol_Text] = ImVec4(0.10f, 0.15f, 0.20f, 1.0f);

            // Docking 分支特定颜色
            newStyle.Colors[ImGuiCol_DockingPreview] = ImVec4(0.25f, 0.55f, 0.80f, 0.7f);
            newStyle.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.94f, 0.94f, 0.96f, 1.0f);
            newStyle.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.25f, 0.55f, 0.80f, 1.0f);
            newStyle.Colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.15f, 0.45f, 0.70f, 1.0f);
            break;

        case 4: // 学术主题
            newStyle = ImGuiStyle();
            ImGui::StyleColorsLight(&newStyle);

            newStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.98f, 0.98f, 0.99f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.20f, 0.25f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.35f, 0.60f, 1.0f);
            newStyle.Colors[ImGuiCol_Button] = ImVec4(0.85f, 0.87f, 0.90f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.75f, 0.80f, 0.85f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonActive] = ImVec4(0.65f, 0.70f, 0.75f, 1.0f);
            newStyle.Colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            newStyle.Colors[ImGuiCol_CheckMark] = ImVec4(0.10f, 0.35f, 0.60f, 1.0f);
            newStyle.Colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.45f, 0.50f, 1.0f);
            newStyle.Colors[ImGuiCol_Text] = ImVec4(0.05f, 0.05f, 0.08f, 1.0f);

            newStyle.Colors[ImGuiCol_DockingPreview] = ImVec4(0.10f, 0.35f, 0.60f, 0.7f);
            newStyle.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.98f, 0.98f, 0.99f, 1.0f);
            newStyle.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.10f, 0.35f, 0.60f, 1.0f);
            break;

        case 5: // 护眼主题
            newStyle = ImGuiStyle();
            ImGui::StyleColorsDark(&newStyle);

            newStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.96f, 0.94f, 0.88f, 1.0f);
            newStyle.Colors[ImGuiCol_ChildBg] = ImVec4(0.98f, 0.96f, 0.92f, 1.0f);
            newStyle.Colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.96f, 0.92f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.55f, 0.65f, 0.55f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.45f, 0.60f, 0.45f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.55f, 0.65f, 0.55f, 1.0f);
            newStyle.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.92f, 0.90f, 0.84f, 1.0f);
            newStyle.Colors[ImGuiCol_Button] = ImVec4(0.70f, 0.78f, 0.68f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.65f, 0.75f, 0.62f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonActive] = ImVec4(0.55f, 0.65f, 0.50f, 1.0f);
            newStyle.Colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.0f, 0.98f, 1.0f);
            newStyle.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.96f, 0.90f, 1.0f);
            newStyle.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.88f, 0.92f, 0.84f, 1.0f);
            newStyle.Colors[ImGuiCol_CheckMark] = ImVec4(0.45f, 0.65f, 0.40f, 1.0f);
            newStyle.Colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.70f, 0.50f, 1.0f);
            newStyle.Colors[ImGuiCol_Text] = ImVec4(0.20f, 0.22f, 0.18f, 1.0f);

            newStyle.Colors[ImGuiCol_DockingPreview] = ImVec4(0.45f, 0.65f, 0.40f, 0.7f);
            newStyle.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.96f, 0.94f, 0.88f, 1.0f);
            newStyle.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.45f, 0.65f, 0.40f, 1.0f);
            break;

        case 6: // 暖色调
            newStyle = ImGuiStyle();
            ImGui::StyleColorsDark(&newStyle);

            newStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.20f, 0.10f, 0.08f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.80f, 0.40f, 0.20f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.90f, 0.50f, 0.30f, 1.0f);
            newStyle.Colors[ImGuiCol_Button] = ImVec4(0.70f, 0.35f, 0.15f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.45f, 0.25f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonActive] = ImVec4(0.90f, 0.55f, 0.35f, 1.0f);
            newStyle.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.15f, 0.10f, 1.0f);
            newStyle.Colors[ImGuiCol_Text] = ImVec4(1.00f, 0.90f, 0.80f, 1.0f);

            newStyle.Colors[ImGuiCol_DockingPreview] = ImVec4(0.80f, 0.40f, 0.20f, 0.7f);
            newStyle.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.10f, 0.08f, 1.0f);
            break;

        case 7: // 森林主题
            newStyle = ImGuiStyle();
            ImGui::StyleColorsDark(&newStyle);

            newStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.12f, 0.05f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.30f, 0.10f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.40f, 0.15f, 1.0f);
            newStyle.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.35f, 0.10f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.15f, 0.45f, 0.15f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.55f, 0.20f, 1.0f);
            newStyle.Colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.20f, 0.08f, 1.0f);
            newStyle.Colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.80f, 0.20f, 1.0f);
            newStyle.Colors[ImGuiCol_SliderGrab] = ImVec4(0.20f, 0.60f, 0.20f, 1.0f);
            newStyle.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.90f, 0.70f, 1.0f);

            newStyle.Colors[ImGuiCol_DockingPreview] = ImVec4(0.20f, 0.60f, 0.20f, 0.7f);
            newStyle.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.05f, 0.12f, 0.05f, 1.0f);
            break;

        case 8: // 冷色调主题（清凉夏日）
            newStyle = ImGuiStyle();
            ImGui::StyleColorsDark(&newStyle);

            newStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.85f, 0.92f, 0.98f, 1.0f);
            newStyle.Colors[ImGuiCol_ChildBg] = ImVec4(0.90f, 0.95f, 1.00f, 1.0f);
            newStyle.Colors[ImGuiCol_PopupBg] = ImVec4(0.88f, 0.93f, 0.98f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.30f, 0.60f, 0.85f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.55f, 0.80f, 1.0f);
            newStyle.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.30f, 0.60f, 0.85f, 1.0f);
            newStyle.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.82f, 0.90f, 0.96f, 1.0f);
            newStyle.Colors[ImGuiCol_Button] = ImVec4(0.35f, 0.65f, 0.90f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.70f, 0.95f, 1.0f);
            newStyle.Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.55f, 0.80f, 1.0f);
            newStyle.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
            newStyle.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.92f, 0.96f, 1.00f, 1.0f);
            newStyle.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.92f, 0.98f, 1.0f);
            newStyle.Colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.55f, 0.85f, 1.0f);
            newStyle.Colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.60f, 0.90f, 1.0f);
            newStyle.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 0.50f, 0.80f, 1.0f);
            newStyle.Colors[ImGuiCol_Header] = ImVec4(0.50f, 0.75f, 0.95f, 1.0f);
            newStyle.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.70f, 0.90f, 1.0f);
            newStyle.Colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.60f, 0.85f, 1.0f);
            newStyle.Colors[ImGuiCol_Separator] = ImVec4(0.60f, 0.75f, 0.90f, 1.0f);
            newStyle.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.50f, 0.65f, 0.85f, 1.0f);
            newStyle.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.55f, 0.75f, 1.0f);
            newStyle.Colors[ImGuiCol_Text] = ImVec4(0.10f, 0.20f, 0.35f, 1.0f);
            newStyle.Colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.65f, 0.80f, 1.0f);
            newStyle.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.50f, 0.75f, 0.95f, 0.5f);
            newStyle.Colors[ImGuiCol_Tab] = ImVec4(0.70f, 0.82f, 0.92f, 1.0f);
            newStyle.Colors[ImGuiCol_TabHovered] = ImVec4(0.60f, 0.75f, 0.88f, 1.0f);
            newStyle.Colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.65f, 0.90f, 1.0f);
            newStyle.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.80f, 0.88f, 0.95f, 1.0f);
            newStyle.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.70f, 0.80f, 0.90f, 1.0f);
            newStyle.Colors[ImGuiCol_DockingPreview] = ImVec4(0.30f, 0.65f, 0.90f, 0.7f);
            newStyle.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.85f, 0.92f, 0.98f, 1.0f);
            newStyle.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.25f, 0.65f, 0.95f, 1.0f);
            newStyle.Colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.20f, 0.55f, 0.85f, 1.0f);
            break;
        }

        // 恢复透明度
        newStyle.Alpha = originalAlpha;

        // 应用新样式
        style = newStyle;

        // 恢复布局参数（保持形态样式）
        if (keepLayout) {
            RestoreLayoutParams(style, currentLayout);
        }

        // 恢复全局缩放
        io.FontGlobalScale = 1.0f;
    }

    // 获取主题名称
    const char* GetThemeName(int themeID) {
        const char* themes[] = { u8"深色主题", u8"浅色主题", u8"经典主题", u8"正式主题",
                                 u8"学术主题", u8"护眼主题", u8"暖色调", u8"森林主题", u8"冷色调" };
        if (themeID >= 0 && themeID < IM_ARRAYSIZE(themes)) {
            return themes[themeID];
        }
        return u8"未知主题";
    }

    // 获取主题数量
    int GetThemeCount() {
        return 9;
    }

private:
    ThemeManager() {}
};