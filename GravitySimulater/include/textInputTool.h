#pragma once
#include <windows.h>
#include <imm.h>      // 输入法管理相关函数

#include <imgui.h>

inline void UpdateIMEPosition() {
    // 1. 检查是否有激活的 ImGui 输入框
    if (!ImGui::IsAnyItemActive()) return;

    // 2. 获取当前活动输入框的屏幕坐标
    ImVec2 itemRectMin = ImGui::GetItemRectMin();  // 输入框左上角（屏幕坐标）
    ImVec2 itemRectMax = ImGui::GetItemRectMax();  // 输入框右下角（屏幕坐标）

    // 候选框出现在输入框下方
    int x = (int)(itemRectMin.x);
    int y = (int)(itemRectMax.y + 2);  // +2 像素间距

    // 3. 调用 Windows API 设置输入法窗口的位置
    HWND hwnd = GetActiveWindow();  // 获取当前活动窗口句柄
    HIMC himc = ImmGetContext(hwnd);

    if (himc) {
        COMPOSITIONFORM cf;
        cf.dwStyle = CFS_POINT;
        cf.ptCurrentPos.x = x;
        cf.ptCurrentPos.y = y;
        ImmSetCompositionWindow(himc, &cf);

        // 同时设置候选框位置
        CANDIDATEFORM caf;
        caf.dwStyle = CFS_CANDIDATEPOS;
        caf.ptCurrentPos.x = x;
        caf.ptCurrentPos.y = y;
        ImmSetCandidateWindow(himc, &caf);

        ImmReleaseContext(hwnd, himc);
    }
}