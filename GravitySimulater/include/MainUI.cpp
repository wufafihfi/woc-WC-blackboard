#include "MainUI.h"

#include "fileTools.h"
#include "textInputTool.h"
#include "Log/LogSystem.h"
#include "M_Application.h"
#include "dataSavingHandle.h"
#include "ThemeManager.h"

void DrawToolsWindow();
void DrawInfo();
void StyleEditor(bool* isOpen = (bool *)false);

void MainUI::Render() {
    if (!m_visible) return;

    auto& props = Application::Get().GetAppProperties();

    int nowFontID = props.GetValue<int>("MainUIdata", "nowFontID");

    static bool showStyleEdit = false;
    static bool showInfo = false;

    static bool showDemoWindow = props.GetValue<bool>("MainUIdata", "showDemoWindow");
    static bool showDebugWindow = props.GetValue<bool>("MainUIdata", "showDebugWindow");
    static bool showLogWindow = props.GetValue<bool>("MainUIdata", "showLogWindow");
    static bool showDrawToolsWindow = props.GetValue<bool>("MainUIdata", "showDrawToolsWindow");
    if (showDemoWindow != props.GetValue<bool>("MainUIdata", "showDemoWindow")) {
        props.SetValue("MainUIdata", "showDemoWindow", showDemoWindow);
    }
    if (showDebugWindow != props.GetValue<bool>("MainUIdata", "showDebugWindow")) {
        props.SetValue("MainUIdata", "showDebugWindow", showDebugWindow);
    }
    if (showLogWindow != props.GetValue<bool>("MainUIdata", "showLogWindow")) {
        props.SetValue("MainUIdata", "showLogWindow", showLogWindow);
    }
    if (showDrawToolsWindow != props.GetValue<bool>("MainUIdata", "showDrawToolsWindow")) {
        props.SetValue("MainUIdata", "showDrawToolsWindow", showDrawToolsWindow);
    }


    // 字体缩放
    static int scale_Last = props.GetValue<int>("MainUIdata", "FontScale");
    static int scale = props.GetValue<int>("MainUIdata", "FontScale");

    // 自定义事件响应
    if (props.GetValue<bool>("ChangeFont", "ChangeFontFlag"))
    {
        props.SetValue("ChangeFont", "ChangeFontFlag", false);

        if (props.GetValue<bool>("ChangeFont", "FontChangedFlag")) {
            props.SetValue("ChangeFont", "FontChangedFlag", false);

            props.SetValue<int>("MainUIdata", "nowFontID", 1);
        }
        else
        {
            props.SetValue<int>("MainUIdata", "nowFontID", 0);
        }
    }

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[nowFontID]);

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(u8"文件")) {
            if (ImGui::MenuItem(u8"存档", u8"保存当前状态")) {
                LOG_INFO(u8"保存 saved");
                SaveConfig();
            }
            ImGui::Separator();
            if (ImGui::MenuItem(u8"退出", "Alt+F4")) {
                props.SetValue<bool>("MainUIdata", "Exit", true);
            }
            ImGui::EndMenu();
        }
        // 界面使用特殊字体=========
        if (ImGui::BeginMenu(u8"界面")) {
            if (ImGui::Button(u8"重置")) {
                ImGui::GetIO().FontGlobalScale = 1;
                scale = 16;
                scale_Last = 16;
                props.SetValue("MainUIdata", "FontScale", 16);
                props.SetValue("ChangeFont", "ChangeFontFlag", true);
            }

            if (ImGui::BeginMenu(u8"字体缩放")) {
                ImGui::SetNextItemWidth(90.0f);
                ImGui::DragInt(u8"字体大小", &scale, 1, 10, 29);

                if (ImGui::Button(u8"刷新"))
                {
                    if (scale != scale_Last)
                    {
                        scale_Last = scale;
                        props.SetValue("MainUIdata", "FontScale", scale);
                        props.SetValue("ChangeFont", "ChangeFontFlag", true);
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(u8"界面缩放")) {
                static float scale_Last = props.GetValue<float>("MainUIdata", "IMGUIscale");
                static float scale = props.GetValue<float>("MainUIdata", "IMGUIscale");

                // 显示当前缩放值
                ImGui::Text(u8"当前缩放: %.2f", scale);
                ImGui::Separator();

                // 按钮
                ImGui::Separator();
                ImGui::Text(u8"预设:");
                if (ImGui::Button("0.8x")) {
                    scale = 0.8f;
                    ImGui::GetIO().FontGlobalScale = scale;
                }
                ImGui::SameLine();
                if (ImGui::Button("1.0x")) {
                    scale = 1.0f;
                    ImGui::GetIO().FontGlobalScale = scale;
                }
                if (ImGui::Button("1.2x")) {
                    scale = 1.2f;
                    ImGui::GetIO().FontGlobalScale = scale;
                }
                ImGui::SameLine();
                if (ImGui::Button("1.5x")) {
                    scale = 1.5f;
                    ImGui::GetIO().FontGlobalScale = scale;
                }
                if (ImGui::Button("1.8x")) {
                    scale = 1.8f;
                    ImGui::GetIO().FontGlobalScale = scale;
                }
                ImGui::SameLine();
                if (ImGui::Button("2.0x")) {
                    scale = 2.0f;
                    ImGui::GetIO().FontGlobalScale = scale;
                }

                if (scale != scale_Last)
                {
                    props.SetValue<float>("MainUIdata", "IMGUIscale", scale);
                    scale_Last = scale;
                }
                ImGui::EndMenu();
            }

            // 字体切换放到最下面防遮挡
            ImGui::PopFont();
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
            if (ImGui::BeginMenu(u8"切换字体")) {

                auto* fontsGroupOpt = props.GetGroup("FontsData");
                if (fontsGroupOpt) {
                    for (auto& afont : fontsGroupOpt->properties) {
                        std::string fontDisplayName = afont.displayName;
                        std::string fontName_paht = std::get<std::string>(afont.value);
                        if (ImGui::MenuItem(fontDisplayName.c_str())) {
                            props.SetValue("ChangeFont", "ChangeFontFlag", true);
                            props.SetValue("ChangeFont", "TargetFontPath", fontName_paht);
                        }
                    }
                }

                ImGui::EndMenu();
            }
            ImGui::PopFont();
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[nowFontID]);

            ImGui::EndMenu();
        }
        // 界面使用特殊字体=========
        if (ImGui::BeginMenu(u8"窗口")) {
            if (ImGui::MenuItem(u8"样式设置窗口")) {
                showStyleEdit = !showStyleEdit;
            }
            if (ImGui::MenuItem(u8"绘图工具窗口")) {
                showDrawToolsWindow = !showDrawToolsWindow;
            }
            if (ImGui::BeginMenu(u8"调试")) {
                if (ImGui::MenuItem(u8"显示调试信息")) {
                    showInfo = !showInfo;
                }
                if (ImGui::MenuItem(u8"日志系统")) {
                    showLogWindow = !showLogWindow;
                }
                if (ImGui::MenuItem(u8"IMGUI日志")) {
                    showDebugWindow = !showDebugWindow;
                }
                if (ImGui::MenuItem(u8"IMGUI测试")) {
                    showDemoWindow = !showDemoWindow;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    //ImGui::ShowMetricsWindow(&showDemoWindow);
    if (showDebugWindow)
        ImGui::ShowDebugLogWindow(&showDebugWindow);
    if (showDemoWindow)
        ImGui::ShowDemoWindow(&showDemoWindow);

    if (showDrawToolsWindow)
        DrawToolsWindow();

    if (showStyleEdit)
        StyleEditor(&showStyleEdit);

    if (showInfo)
        DrawInfo();

    // 日志窗口
    SHOW_LOG_WINDOW(&showLogWindow);

    ImGui::PopFont();
}

void DrawToolsWindow() {
    auto& props = Application::Get().GetAppProperties();

    static bool isHorizontal = props.GetValue<bool>("MainDrawData", "isHorizontal");  // true: 横向布局, false: 纵向布局

    ImGui::Begin(u8"绘图工具箱");

    // 布局切换按钮
    if (ImGui::SmallButton(isHorizontal ? u8"纵向布局" : u8"横向布局")) {
        isHorizontal = !isHorizontal;
        props.SetValue("MainDrawData", "isHorizontal", isHorizontal);
    }

    if (isHorizontal) {
        // ========== 横向布局 ==========
        // 工具按钮行
        static int last_selected_tool = 0;
        static int selected_tool = props.GetValue<int>("MainDrawData", "ToolType");

        ImGui::RadioButton(u8"移动", &selected_tool, 0);
        ImGui::SameLine();
        ImGui::RadioButton(u8"画笔", &selected_tool, 1);
        ImGui::SameLine();
        ImGui::RadioButton(u8"橡皮", &selected_tool, 2);
        ImGui::SameLine();

        if (last_selected_tool != selected_tool) {
            last_selected_tool = selected_tool;
            props.SetValue("MainDrawData", "ToolType", selected_tool);
        }

        // 背景色
        std::vector<float> bkColorVec = props.GetValue<std::vector<float>>("MainDrawData", "backGroundColor");
        float bkColor[4] = {
            bkColorVec[0] / 255.0f,
            bkColorVec[1] / 255.0f,
            bkColorVec[2] / 255.0f,
            bkColorVec[3] / 255.0f
        };

        if (ImGui::ColorEdit4(u8"背景色", bkColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueBar)) {
            std::vector<float> newColor = {
                bkColor[0] * 255.0f,
                bkColor[1] * 255.0f,
                bkColor[2] * 255.0f,
                bkColor[3] * 255.0f
            };
            props.SetValue("MainDrawData", "backGroundColor", newColor);
        }
        ImGui::SameLine();

        // 清空按钮进度条
        static float clear_count = 0;
        if (ImGui::Button(u8"清空画布")) {
        }
        if (ImGui::IsItemActive() && ImGui::IsItemHovered()) {
            clear_count += 0.05f;
            if (clear_count >= 3) {
                props.SetValue("MainDrawData", "isCanvasClear", true);
                clear_count = 0;
            }
        }
        else {
            clear_count -= 0.5f;
            if (clear_count < 0) clear_count = 0;
        }
        ImGui::SameLine();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
        float progress_width = 100.0f;
        float progress_height = 10.0f;
        float fill_width = progress_width * (clear_count / 3.0f);
        if (fill_width > progress_width) fill_width = progress_width;

        draw_list->AddRectFilled(ImVec2(cursor_pos.x, cursor_pos.y + progress_height * 0.5f),
            ImVec2(cursor_pos.x + progress_width, cursor_pos.y + progress_height * 1.5f),
            IM_COL32(100, 100, 100, 255));
        draw_list->AddRectFilled(ImVec2(cursor_pos.x, cursor_pos.y + progress_height * 0.5f),
            ImVec2(cursor_pos.x + fill_width, cursor_pos.y + progress_height * 1.5f),
            IM_COL32(100, 200, 100, 255));
        ImGui::Dummy(ImVec2(progress_width, progress_height));

        // 根据工具显示不同设置
        if (selected_tool == 0) {
            static float scaleSpeed = props.GetValue<float>("MainDrawData", "scaleSpeed");
            ImGui::PushItemWidth(150.0f);
            ImGui::SliderFloat(u8"画布缩放速度", &scaleSpeed, 0.1f, 5.0f, "%.1f");
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                props.SetValue("MainDrawData", "scaleSpeed", scaleSpeed);
            }
        }

        if (selected_tool == 1) {
            ImGui::Separator();
            ImGui::Text(u8"画笔设置");
            static float brushThickness = props.GetValue<float>("MainDrawData", "BrushThickness");
            ImGui::PushItemWidth(150.0f);
            ImGui::SliderFloat(u8"粗细", &brushThickness, 1.0f, 50.0f, "%.1f");
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                props.SetValue("MainDrawData", "BrushThickness", brushThickness);
            }
            ImGui::SameLine();

            std::vector<float> brushColorVec = props.GetValue<std::vector<float>>("MainDrawData", "BrushColor");
            float brushColor[4] = {
                brushColorVec[0] / 255.0f,
                brushColorVec[1] / 255.0f,
                brushColorVec[2] / 255.0f,
                brushColorVec[3] / 255.0f
            };

            if (ImGui::ColorEdit4(u8"笔刷颜色", brushColor,
                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueBar)) {
                std::vector<float> newColor = {
                    brushColor[0] * 255.0f,
                    brushColor[1] * 255.0f,
                    brushColor[2] * 255.0f,
                    brushColor[3] * 255.0f
                };
                props.SetValue("MainDrawData", "BrushColor", newColor);
            }
            ImGui::SameLine();

            // 画笔预览
            ImGui::Text(u8"预览");
            ImGui::SameLine();
            draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            float preview_size = 50.0f;
            ImVec2 center = ImVec2(canvas_pos.x + preview_size / 2, canvas_pos.y + preview_size / 2);
            float radius = brushThickness * (preview_size / 50.0f) / 2.0f;
            if (radius > preview_size / 2) radius = preview_size / 2;

            draw_list->AddRectFilled(canvas_pos,
                ImVec2(canvas_pos.x + preview_size, canvas_pos.y + preview_size),
                IM_COL32(50, 50, 50, 255));
            draw_list->AddCircleFilled(center, radius,
                IM_COL32((int)(brushColor[0] * 255),
                    (int)(brushColor[1] * 255),
                    (int)(brushColor[2] * 255),
                    (int)(brushColor[3] * 255)));
            ImGui::Dummy(ImVec2(preview_size, preview_size));
        }

        if (selected_tool == 2) {
            ImGui::Separator();
            ImGui::Text(u8"橡皮设置");
            static float eraserSize = props.GetValue<float>("MainDrawData", "EraserSize");
            ImGui::PushItemWidth(150.0f);
            if (ImGui::SliderFloat(u8"大小", &eraserSize, 5.0f, 200.0f, "%.0f")) {
                props.SetValue("MainDrawData", "EraserSize", eraserSize);
            }
            ImGui::SameLine();

            // 橡皮预览
            ImGui::Text(u8"预览");
            ImGui::SameLine();
            draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            float preview_size = 50.0f;
            ImVec2 center = ImVec2(canvas_pos.x + preview_size / 2, canvas_pos.y + preview_size / 2);
            float radius = eraserSize * (preview_size / 200.0f) / 2.0f;
            if (radius > preview_size / 2) radius = preview_size / 2;

            draw_list->AddRectFilled(canvas_pos,
                ImVec2(canvas_pos.x + preview_size, canvas_pos.y + preview_size),
                IM_COL32(50, 50, 50, 255));
            draw_list->AddCircleFilled(center, radius, IM_COL32(200, 200, 200, 255));
            ImGui::Dummy(ImVec2(preview_size, preview_size));
        }
    }
    else {
        // ========== 纵向布局 ==========
        static int last_selected_tool = 0;
        static int selected_tool = props.GetValue<int>("MainDrawData", "ToolType");

        // 工具按钮垂直排列
        ImGui::RadioButton(u8"移动", &selected_tool, 0);
        ImGui::RadioButton(u8"画笔", &selected_tool, 1);
        ImGui::RadioButton(u8"橡皮", &selected_tool, 2);

        if (last_selected_tool != selected_tool) {
            last_selected_tool = selected_tool;
            props.SetValue("MainDrawData", "ToolType", selected_tool);
        }

        ImGui::Separator();

        // 背景色
        std::vector<float> bkColorVec = props.GetValue<std::vector<float>>("MainDrawData", "backGroundColor");
        float bkColor[4] = {
            bkColorVec[0] / 255.0f,
            bkColorVec[1] / 255.0f,
            bkColorVec[2] / 255.0f,
            bkColorVec[3] / 255.0f
        };

        if (ImGui::ColorEdit4(u8"背景色", bkColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueBar)) {
            std::vector<float> newColor = {
                bkColor[0] * 255.0f,
                bkColor[1] * 255.0f,
                bkColor[2] * 255.0f,
                bkColor[3] * 255.0f
            };
            props.SetValue("MainDrawData", "backGroundColor", newColor);
        }

        // 清空按钮（纵向布局单独一行）
        static float clear_count = 0;
        if (ImGui::Button(u8"清空画布", ImVec2(-1, 0))) {
        }
        if (ImGui::IsItemActive()) {
            clear_count += 0.05f;
            if (clear_count >= 3) {
                props.SetValue("MainDrawData", "isCanvasClear", true);
                clear_count = 0;
            }
        }
        else {
            clear_count -= 0.5f;
            if (clear_count < 0) clear_count = 0;
        }

        // 进度条
        float progress_width = ImGui::GetContentRegionAvail().x;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
        float fill_width = progress_width * (clear_count / 3.0f);
        if (fill_width > progress_width) fill_width = progress_width;

        draw_list->AddRectFilled(cursor_pos,
            ImVec2(cursor_pos.x + progress_width, cursor_pos.y + 10),
            IM_COL32(100, 100, 100, 255));
        draw_list->AddRectFilled(cursor_pos,
            ImVec2(cursor_pos.x + fill_width, cursor_pos.y + 10),
            IM_COL32(100, 200, 100, 255));
        ImGui::Dummy(ImVec2(progress_width, 10));

        ImGui::Separator();

        // 根据工具显示设置
        if (selected_tool == 0) {
            static float scaleSpeed = props.GetValue<float>("MainDrawData", "scaleSpeed");
            ImGui::Text(u8"画布缩放速度");
            ImGui::PushItemWidth(120.0f);
            ImGui::SliderFloat(u8"##scaleSpeed", &scaleSpeed, 0.1f, 5.0f, "%.1f");
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                props.SetValue("MainDrawData", "scaleSpeed", scaleSpeed);
            }
        }

        if (selected_tool == 1) {
            ImGui::Text(u8"画笔设置");
            static float brushThickness = props.GetValue<float>("MainDrawData", "BrushThickness");
            ImGui::Text(u8"粗细");
            ImGui::PushItemWidth(120.0f);
            ImGui::SliderFloat(u8"##brushThickness", &brushThickness, 1.0f, 50.0f, "%.1f");
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                props.SetValue("MainDrawData", "BrushThickness", brushThickness);
            }

            std::vector<float> brushColorVec = props.GetValue<std::vector<float>>("MainDrawData", "BrushColor");
            float brushColor[4] = {
                brushColorVec[0] / 255.0f,
                brushColorVec[1] / 255.0f,
                brushColorVec[2] / 255.0f,
                brushColorVec[3] / 255.0f
            };

            if (ImGui::ColorEdit4(u8"笔刷颜色", brushColor,
                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueBar)) {
                std::vector<float> newColor = {
                    brushColor[0] * 255.0f,
                    brushColor[1] * 255.0f,
                    brushColor[2] * 255.0f,
                    brushColor[3] * 255.0f
                };
                props.SetValue("MainDrawData", "BrushColor", newColor);
            }


            // 画笔预览
            ImGui::Text(u8"预览");
            draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            float preview_size = 80.0f;
            ImVec2 center = ImVec2(canvas_pos.x + preview_size / 2, canvas_pos.y + preview_size / 2);
            float radius = brushThickness * (preview_size / 50.0f) / 2.0f;
            if (radius > preview_size / 2) radius = preview_size / 2;

            draw_list->AddRectFilled(canvas_pos,
                ImVec2(canvas_pos.x + preview_size, canvas_pos.y + preview_size),
                IM_COL32(50, 50, 50, 255));
            draw_list->AddCircleFilled(center, radius,
                IM_COL32((int)(brushColor[0] * 255),
                    (int)(brushColor[1] * 255),
                    (int)(brushColor[2] * 255),
                    (int)(brushColor[3] * 255)));
            ImGui::Dummy(ImVec2(preview_size, preview_size));
        }

        if (selected_tool == 2) {
            ImGui::Text(u8"橡皮设置");
            static float eraserSize = props.GetValue<float>("MainDrawData", "EraserSize");
            ImGui::Text(u8"大小");
            ImGui::PushItemWidth(120.0f);
            if (ImGui::SliderFloat(u8"##eraserSize", &eraserSize, 5.0f, 200.0f, "%.0f")) {
                props.SetValue("MainDrawData", "EraserSize", eraserSize);
            }

            // 橡皮预览
            ImGui::Text(u8"预览");
            draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            float preview_size = 80.0f;
            ImVec2 center = ImVec2(canvas_pos.x + preview_size / 2, canvas_pos.y + preview_size / 2);
            float radius = eraserSize * (preview_size / 200.0f) / 2.0f;
            if (radius > preview_size / 2) radius = preview_size / 2;

            draw_list->AddRectFilled(canvas_pos,
                ImVec2(canvas_pos.x + preview_size, canvas_pos.y + preview_size),
                IM_COL32(50, 50, 50, 255));
            draw_list->AddCircleFilled(center, radius, IM_COL32(200, 200, 200, 255));
            ImGui::Dummy(ImVec2(preview_size, preview_size));
        }
    }

    ImGui::End();
}

void DrawInfo() {
    static sf::Clock clock;
    static float deltaTime = clock.restart().asSeconds();;

    ImGui::Begin(u8"性能", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    float fps = 1.0f / deltaTime;
    deltaTime = clock.restart().asSeconds();
    ImGui::Text(u8"帧率: %.1f FPS", fps);

    ImGui::End();
}

void StyleEditor(bool* isOpen) {
    auto& props = Application::Get().GetAppProperties();
    auto& themeManager = ThemeManager::Get();

    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Begin(u8"样式编辑器", isOpen, ImGuiWindowFlags_AlwaysAutoResize);

    // 获取当前主题ID
    static int current_theme = props.GetValue<int>("MainUIdata", "themeID");

    // 主题选择下拉菜单
    ImGui::Text(u8"选择主题");
    ImGui::SameLine();
    if (ImGui::Combo(u8"##ThemeSelector", &current_theme,
        [](void* data, int idx, const char** out_text) {
            auto* themeManager = (ThemeManager*)data;
            *out_text = themeManager->GetThemeName(idx);
            return true;
        }, &themeManager, themeManager.GetThemeCount())) {
        // 应用新主题
        themeManager.ApplyTheme(current_theme, true);
        // 保存主题ID到配置
        props.SetValue("MainUIdata", "themeID", current_theme);
    }

    // 显示当前主题名称
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), u8"当前: %s",
        themeManager.GetThemeName(current_theme));

    ImGui::Separator();

    // 重置按钮
    if (ImGui::Button(u8"重置为深色样式")) {
        themeManager.ApplyTheme(0, true);
        current_theme = 0;
        props.SetValue("MainUIdata", "themeID", current_theme);
    }
    ImGui::SameLine();
    if (ImGui::Button(u8"重置为浅色样式")) {
        themeManager.ApplyTheme(1, true);
        current_theme = 1;
        props.SetValue("MainUIdata", "themeID", current_theme);
    }

    ImGui::End();
}