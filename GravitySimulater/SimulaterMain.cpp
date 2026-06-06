#include <windows.h>
#include <imm.h>      // 输入法管理相关函数

#include <string>


#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "include/M_Application.h"

#include <imgui.h>
#include <imgui-SFML.h>

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
#else
int main()
#endif
{
#ifdef _WIN32
#include <windows.h>
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // 互斥体创建
    HANDLE hMutex = CreateMutex(NULL, TRUE,
        L"bzhf-blackboard-c41fe43e-0bb3-46d3-804a-c92e38e574c0");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxW(NULL, L"错误码:114514100867891\nBZF_EXEERRO_REOPEN\n程序正在运行,无法重复打开此程序", L"不对啊", MB_OK | MB_ICONERROR);
        if (hMutex)
        {
            CloseHandle(hMutex);
        }
        return 0;
    }
#endif

    Application::Get().Run();

    // 清理资源
    ImGui::SFML::Shutdown();
#ifdef _WIN32
        // 释放互斥句柄
        if (hMutex)
        {
            ReleaseMutex(hMutex);
            CloseHandle(hMutex);
            hMutex = NULL;
        };
#endif
    return 0;
}

// 杂物间
/*
        auto& props = Application::Get().GetAppProperties();

        // 获取所有UI状态
        bool showDemoWindow = props.GetValue<bool>("MainUIdata", "ShowDemoWindow");
        bool showAnotherWindow = props.GetValue<bool>("MainUIdata", "ShowAnotherWindow");
        bool checkboxValue = props.GetValue<bool>("MainUIdata", "CheckboxValue");
        float sliderValue = props.GetValue<float>("MainUIdata", "SliderValue");
        int counter = props.GetValue<int>("MainUIdata", "Counter");
        int currentItem = props.GetValue<int>("MainUIdata", "CurrentItem");
        std::string textBuffer = props.GetValue<std::string>("MainUIdata", "TextBuffer");
        std::vector<float> color = props.GetValue<std::vector<float>>("MainUIdata", "BackgroundColor");

        sf::RenderWindow *window = &Application::Get().GetWindow();


        // 显示 ImGui 自带的演示窗口
        if (showDemoWindow) {
            ImGui::ShowDemoWindow(&showDemoWindow);
            props.SetValue("UI状态", "ShowDemoWindow", showDemoWindow);
        }

        // 创建主测试窗口
        ImGui::Begin(u8"ImGui 测试窗口");

        // 显示帧率
        ImGui::Text(u8"帧率: %.1f FPS", ImGui::GetIO().Framerate);
        ImGui::Text(u8"窗口大小: %.0f x %.0f",
            (float)window->getSize().x,
            (float)window->getSize().y);

        ImGui::Separator();

        // 按钮
        if (ImGui::Button(u8"点我 +1")) {
            counter++;
            props.SetValue("UI状态", "Counter", counter);
        }
        ImGui::SameLine();
        ImGui::Text(u8"点击次数: %d", counter);

        ImGui::Separator();

        // 滑块
        if (ImGui::SliderFloat(u8"滑块", &sliderValue, 0.0f, 1.0f)) {
            props.SetValue("UI状态", "SliderValue", sliderValue);
        }
        ImGui::Text(u8"滑块值: %.3f", sliderValue);

        // 复选框
        if (ImGui::Checkbox(u8"显示另一个窗口", &showAnotherWindow)) {
            props.SetValue("UI状态", "ShowAnotherWindow", showAnotherWindow);
        }
        if (ImGui::Checkbox(u8"演示复选框", &checkboxValue)) {
            props.SetValue("UI状态", "CheckboxValue", checkboxValue);
        }

        // 颜色选择器
        float colorArray[3] = { color[0], color[1], color[2] };
        if (ImGui::ColorEdit3(u8"背景颜色", colorArray))
        {
            color = { colorArray[0], colorArray[1], colorArray[2] };
            props.SetValue("UI状态", "BackgroundColor", color);
        }

        // 下拉框
        const char* items[] = { u8"选项 1", u8"选项 2", u8"选项 3", u8"选项 4" };
        if(ImGui::Combo(u8"下拉选择", &currentItem, items, IM_ARRAYSIZE(items))){
            props.SetValue("UI状态", "CurrentItem", currentItem);
        }

        // 输入文本
        char tempBuffer[256];
        strcpy_s(tempBuffer, textBuffer.c_str());
        if (ImGui::InputText(u8"文本输入", tempBuffer, IM_ARRAYSIZE(tempBuffer)))
        {
            textBuffer = tempBuffer;
            props.SetValue("UI状态", "TextBuffer", textBuffer);
        }

        // 开关演示窗口的按钮
        ImGui::Separator();
        if (ImGui::Button(u8"切换 ImGui 演示窗口")) {
            showDemoWindow = !showDemoWindow;
            props.SetValue("UI状态", "ShowDemoWindow", showDemoWindow);
        }

        ImGui::End();

        // 第二个测试窗口
        if (showAnotherWindow) {
            if (ImGui::Begin(u8"另一个窗口", &showAnotherWindow))
            {
                props.SetValue("UI状态", "ShowAnotherWindow", showAnotherWindow);
            }

            ImGui::Text(u8"这是一个可以拖动的窗口");
            ImGui::Text(u8"点击窗口右上角的 X 可以关闭");

            if (ImGui::Button(u8"关闭此窗口")) {
                showAnotherWindow = false;
                props.SetValue("UI状态", "ShowAnotherWindow", showAnotherWindow);
            }
            ImGui::End();
        }

        UpdateIMEPosition();
*/