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
            if (ImGui::BeginMenu(u8"画布")) {
                // 保存画布
                if (ImGui::MenuItem(u8"保存画布")) {
                    handleCanvasSave();
                }

                // 画布另存为
                if (ImGui::MenuItem(u8"画布另存为")) {
                    handleCanvasSaveAs();
                }

                // 打开画布存储文件夹
                if (ImGui::MenuItem(u8"打开画布存储文件夹")) {
                    handleOpenSaveFolder();
                }

                ImGui::EndMenu();
            }

            ImGui::Text(u8"自动保存 - 每 %0.1f s一次", Application::Get().m_saveInterval);
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

        if (ImGui::BeginMenu(u8"功能")) {
            if (ImGui::BeginMenu(u8"画布")) {
                std::vector<float> canvasSize = props.GetValue<std::vector<float>>("MainDrawData", "CanvasSize_int");
                static int canvasSize_X = canvasSize[0];
                static int canvasSize_Y = canvasSize[1];
                //::Text(u8"画布大小: %d x %d", (int)canvasSize[0], (int)canvasSize[1]);

                ImGui::Text(u8"画布大小:");
                ImGui::SetNextItemWidth(80.0f);
                if (ImGui::DragInt("##CanvasWidth", &canvasSize_X, 10, 10, 10000, "%d")) {
                    if (canvasSize_X > 10000)
                        canvasSize_X = 10000;
                    if (canvasSize_X < 10)
                        canvasSize_X = 10;
                };
                ImGui::SameLine();
                ImGui::Text(u8"x");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80.0f);
                if (ImGui::DragInt("##CanvasHeight", &canvasSize_Y, 10, 10, 10000, "%d")) {
                    if (canvasSize_Y > 10000)
                        canvasSize_Y = 10000;
                    if (canvasSize_Y < 10)
                        canvasSize_Y = 10;
                };
                ImGui::SameLine();
                ImGui::Text(u8"像素");
                ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"画布越大内存(显存)占用越大,太大可能导致崩溃");
                ImGui::Text(u8"此设置将在下次启动后应用");

                if (canvasSize_X != canvasSize[0] || canvasSize_Y != canvasSize[1]) {
                    canvasSize[0] = canvasSize_X;
                    canvasSize[1] = canvasSize_Y;
                    props.SetValue("MainDrawData", "CanvasSize_int", canvasSize);

                    SaveConfig();
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

    // 渲染覆盖确认弹窗
    renderOverwriteDialog();
    renderSaveAsDialog();

    ImGui::PopFont();
}

void M_setMouseCursor(int toolType) {
    if(!ImGui::GetIO().WantCaptureMouse)
    {
        switch (toolType)
        {
        case 0:
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            break;

        case 1:
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            break;

        case 2:
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            break;

        default:
            break;
        }
    }
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
        if (ImGui::Button(u8"清空画布"));
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

        ImGui::SameLine();
        if (ImGui::Button(u8"<撤销")) {
            props.SetValue("MainDrawData", "undo_Flag", true);
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"重做>")) {
            props.SetValue("MainDrawData", "redo_Flag", true);
        }

        // 根据工具显示不同设置
        M_setMouseCursor(selected_tool);

        if (selected_tool == 0) {
            static float scaleSpeed = props.GetValue<float>("MainDrawData", "scaleSpeed");
            ImGui::PushItemWidth(150.0f);
            ImGui::SliderFloat(u8"画布缩放速度", &scaleSpeed, 0.1f, 5.0f, "%.1f");
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                props.SetValue("MainDrawData", "scaleSpeed", scaleSpeed);
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"重置")) {
                props.SetValue("MainDrawData", "canvasTransformationReset_Flag", true);
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

        if (ImGui::Button(u8"<撤销")) {
            props.SetValue("MainDrawData", "undo_Flag", true);
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"重做>")) {
            props.SetValue("MainDrawData", "redo_Flag", true);
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
            if (ImGui::Button(u8"重置")) {
                props.SetValue("MainDrawData", "canvasTransformationReset_Flag", true);
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

void MainUI::handleCanvasSave() {
    auto& props = Application::Get().GetAppProperties();

    LOG_INFO(u8"保存画布 canvas saved");

    std::string savePath = props.GetValue<std::string>("MainDrawData", "canvasSave_Path");

    // 如果没有保存过，生成默认路径
    if (savePath.empty()) {
        fs::path defaultPath = getExecutablePath() / "savedCanvas" / "canvas.png";
        savePath = defaultPath.u8string();
    }

    // 检查文件是否存在
    if (CheckFileExists(savePath)) {
        m_showOverwriteDialog = true;
        m_overwriteTempPath = savePath;
    }
    else {
        props.SetValue("MainDrawData", "canvasSave_Flag", true);
        props.SetValue("MainDrawData", "canvasSave_Path", savePath);
    }
}

void MainUI::handleCanvasSaveAs() {
    LOG_INFO(u8"画布另存为 canvas save as");

    // 生成带时间戳的默认文件名
    std::string defaultFilename = generateTimestampFilename(".png");
    fs::path defaultDir = getExecutablePath() / "savedCanvas";

    // 确保目录存在
    if (!fs::exists(defaultDir)) {
        fs::create_directories(defaultDir);
    }

    // 设置弹窗默认值
    m_saveAsFileName = defaultFilename;
    m_saveAsDirectory = defaultDir.u8string();
    m_showSaveAsDialog = true;
}

void MainUI::handleOpenSaveFolder() {
    LOG_INFO(u8"打开画布存储文件夹");

    fs::path saveDir = getExecutablePath() / "savedCanvas";

    // 确保目录存在
    if (!fs::exists(saveDir)) {
        fs::create_directories(saveDir);
    }

    OpenFolderInExplorer(saveDir.u8string());
}

void MainUI::renderSaveAsDialog() {
    if (!m_showSaveAsDialog) {
        return;
    }

    ImGui::OpenPopup(u8"另存为");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoCollapse;

    if (ImGui::BeginPopupModal(u8"另存为", nullptr, flags)) {
        ImGui::Text(u8"保存画布到：");
        ImGui::Separator();

        // 目录输入
        ImGui::Text(u8"目录路径：");
        ImGui::PushItemWidth(350);

        // 使用临时缓冲区
        static char dirBuffer[512] = { 0 };
        static char nameBuffer[256] = { 0 };

        // 每次打开弹窗时更新缓冲区
        static bool firstOpen = true;
        if (firstOpen) {
            strcpy_s(dirBuffer, m_saveAsDirectory.c_str());
            strcpy_s(nameBuffer, m_saveAsFileName.c_str());
            firstOpen = false;
        }

        if (ImGui::InputText(u8"##Directory", dirBuffer, sizeof(dirBuffer))) {
            m_saveAsDirectory = dirBuffer;
        }

        ImGui::SameLine();
        if (ImGui::Button(u8"重置为默认")) {
            fs::path defaultDir = getExecutablePath() / "savedCanvas";
            m_saveAsDirectory = defaultDir.string();  // 使用 string() 而不是 u8string()
            strcpy_s(dirBuffer, m_saveAsDirectory.c_str());
        }

        // 文件名输入
        ImGui::Text(u8"文件名：");
        if (ImGui::InputText(u8"##Filename", nameBuffer, sizeof(nameBuffer))) {
            m_saveAsFileName = nameBuffer;
        }

        //ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f),
            //u8"如果目录为空，将使用默认目录；如果文件名为空，将使用时间戳");

        ImGui::Separator();

        // 按钮
        float buttonWidth = 100.0f;
        float availWidth = ImGui::GetContentRegionAvail().x;
        float buttonPos = (availWidth - buttonWidth * 2 - 10) * 0.5f;

        ImGui::SetCursorPosX(buttonPos);

        if (ImGui::Button(u8"保存", ImVec2(buttonWidth, 0))) {
            auto& props = Application::Get().GetAppProperties();

            // 处理目录
            std::string finalDir = m_saveAsDirectory;
            if (finalDir.empty()) {
                fs::path defaultDir = getExecutablePath() / "savedCanvas";
                finalDir = defaultDir.string();
            }

            // 处理文件名
            std::string finalName = m_saveAsFileName;
            if (finalName.empty()) {
                finalName = generateTimestampFilename(".png");
            }

            // 确保有 .png 扩展名
            if (finalName.find(".png") == std::string::npos) {
                finalName += ".png";
            }

            // Windows: 使用宽字符串路径
#ifdef _WIN32
            // 将 UTF-8 转换为宽字符串
            std::wstring wDir = Utf8ToWide(finalDir);
            std::wstring wName = Utf8ToWide(finalName);
            fs::path fullPath = fs::path(wDir) / wName;
            std::string savePath = fullPath.string();
#else
            fs::path fullPath = fs::path(finalDir) / finalName;
            std::string savePath = fullPath.u8string();
#endif

            // 检查文件是否已存在
            bool fileExists = false;
            try {
                fileExists = fs::exists(fullPath);
            }
            catch (const std::exception& e) {
                LOG_ERROR_STREAM << "检查文件存在时出错 Error while checking file existence: " << e.what();
                fileExists = false;
            }

            if (fileExists) {
                // 文件存在，显示覆盖确认
                m_overwriteTempPath = savePath;
                m_showOverwriteDialog = true;
                m_showSaveAsDialog = false;
                ImGui::CloseCurrentPopup();
            }
            else {
                // 文件不存在，直接保存
                try {
                    if (!fs::exists(fullPath.parent_path())) {
                        fs::create_directories(fullPath.parent_path());
                    }
                    props.SetValue("MainDrawData", "canvasSave_Flag", true);
                    props.SetValue("MainDrawData", "canvasSave_Path", savePath);
                }
                catch (const std::exception& e) {
                    LOG_ERROR_STREAM << "保存画布时出错: " << e.what();
                }
                m_showSaveAsDialog = false;
                ImGui::CloseCurrentPopup();
            }

            firstOpen = true;
        }

        ImGui::SameLine();

        if (ImGui::Button(u8"取消", ImVec2(buttonWidth, 0))) {
            m_showSaveAsDialog = false;
            ImGui::CloseCurrentPopup();
            firstOpen = true;
        }

        ImGui::EndPopup();
    }
}

void MainUI::renderOverwriteDialog() {
    if (!m_showOverwriteDialog) {
        return;
    }

    std::string fileName = fs::path(m_overwriteTempPath).filename().u8string();

    ImGui::OpenPopup(u8"文件已存在");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoCollapse;

    if (ImGui::BeginPopupModal(u8"文件已存在", nullptr, flags)) {
        ImGui::Text(u8"文件已存在:");
        ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "%s", fileName.c_str());
        ImGui::Text(u8"是否覆盖？");

        ImGui::Separator();

        float buttonWidth = 100.0f;
        float availWidth = ImGui::GetContentRegionAvail().x;
        float buttonPos = (availWidth - buttonWidth * 2 - 10) * 0.5f;

        ImGui::SetCursorPosX(buttonPos);

        if (ImGui::Button(u8"覆盖", ImVec2(buttonWidth, 0))) {
            auto& props = Application::Get().GetAppProperties();
            props.SetValue("MainDrawData", "canvasSave_Flag", true);
            props.SetValue("MainDrawData", "canvasSave_Path", m_overwriteTempPath);
            m_showOverwriteDialog = false;
            m_overwriteTempPath.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button(u8"取消", ImVec2(buttonWidth, 0))) {
            m_showOverwriteDialog = false;
            m_overwriteTempPath.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}