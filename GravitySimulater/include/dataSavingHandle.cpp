#include "dataSavingHandle.h"

namespace fs = std::filesystem;

void SaveConfig() {
    auto& props = Application::Get().GetAppProperties();

    if (!props.IsDirty()) {
        return;
    }

    Json::Value ConfigData;

    fs::path BasePath = getExecutablePath();

    // ChangeFont 组
    ConfigData["ChangeFont_TargetFontPath"] = props.GetValue<std::string>("ChangeFont", "TargetFontPath");

    // MainUIdata 组
    ConfigData["MainUIdata_IMGUIscale"] = props.GetValue<float>("MainUIdata", "IMGUIscale");
    ConfigData["MainUIdata_FontScale"] = props.GetValue<int>("MainUIdata", "FontScale");
    ConfigData["MainUIdata_showDemoWindow"] = props.GetValue<bool>("MainUIdata", "showDemoWindow");
    ConfigData["MainUIdata_showDebugWindow"] = props.GetValue<bool>("MainUIdata", "showDebugWindow");
    ConfigData["MainUIdata_showLogWindow"] = props.GetValue<bool>("MainUIdata", "showLogWindow");
    ConfigData["MainUIdata_showDrawToolsWindow"] = props.GetValue<bool>("MainUIdata", "showDrawToolsWindow");
    ConfigData["MainUIdata_themeID"] = props.GetValue<int>("MainUIdata", "themeID");

    // MainDrawData 背景色
    std::vector<float> bkcolor = props.GetValue<std::vector<float>>("MainDrawData", "backGroundColor");
    ConfigData["MainDrawData_backGroundColor"]["R"] = bkcolor[0];
    ConfigData["MainDrawData_backGroundColor"]["G"] = bkcolor[1];
    ConfigData["MainDrawData_backGroundColor"]["B"] = bkcolor[2];
    ConfigData["MainDrawData_backGroundColor"]["A"] = bkcolor[3];

    // MainDrawData 画笔颜色
    std::vector<float> brushColor = props.GetValue<std::vector<float>>("MainDrawData", "BrushColor");
    ConfigData["MainDrawData_BrushColor"]["R"] = brushColor[0];
    ConfigData["MainDrawData_BrushColor"]["G"] = brushColor[1];
    ConfigData["MainDrawData_BrushColor"]["B"] = brushColor[2];
    ConfigData["MainDrawData_BrushColor"]["A"] = brushColor[3];

    ConfigData["MainDrawData_BrushThickness"] = props.GetValue<float>("MainDrawData", "BrushThickness");
    ConfigData["MainDrawData_EraserSize"] = props.GetValue<float>("MainDrawData", "EraserSize");
    ConfigData["MainDrawData_scaleSpeed"] = props.GetValue<float>("MainDrawData", "scaleSpeed");

    // MainDrawData 横纵布局
    ConfigData["MainDrawData_isHorizontal"] = props.GetValue<bool>("MainDrawData", "isHorizontal");

    // MainDrawData 画布大小
    std::vector<float> canvasSize = props.GetValue<std::vector<float>>("MainDrawData", "CanvasSize_int");
    ConfigData["MainDrawData_CanvasSize_int"]["X"] = canvasSize[0];
    ConfigData["MainDrawData_CanvasSize_int"]["Y"] = canvasSize[1];

    // 保存文件
    std::string path = (BasePath / "config" / "mainConfig.json").u8string();
    if (!saveJsonFile(path, ConfigData)) {
        LOG_WARNING("Config file save failed");
    }
    else {
        // 保存成功，清除脏标记
        props.ClearDirty();
    }
}

// 加载配置
void LoadConfig() {
    auto& props = Application::Get().GetAppProperties();
    Json::Value ConfigData;

    fs::path BasePath = getExecutablePath();

    std::string path = (BasePath / "config" / "mainConfig.json").u8string();

    if (!readJsonFile(path, ConfigData)) {
        LOG_WARNING_STREAM << path;
        LOG_WARNING("配置文件读取失败,将使用默认值 Failed to read config file, will use defaults");
        return;
    }

    // 读取ChangeFont
    // 设置保存的字体
    if (ConfigData.isMember("ChangeFont_TargetFontPath")) {
        props.SetValue("ChangeFont", "ChangeFontFlag", true);
        props.SetValue("ChangeFont", "TargetFontPath", ConfigData["ChangeFont_TargetFontPath"].asString());
        LOG_INFO_STREAM << "配置文件字体路径 font path form config:" << ConfigData["ChangeFont_TargetFontPath"].asString();
    }

    // 读取 MainUIdata
    if (ConfigData.isMember("MainUIdata_IMGUIscale")) {
        props.SetValue("MainUIdata", "IMGUIscale", ConfigData["MainUIdata_IMGUIscale"].asFloat());
    }
    if (ConfigData.isMember("MainUIdata_FontScale")) {
        props.SetValue("MainUIdata", "FontScale", ConfigData["MainUIdata_FontScale"].asInt());
    }
    if (ConfigData.isMember("MainUIdata_showDemoWindow")) {
        props.SetValue("MainUIdata", "showDemoWindow", ConfigData["MainUIdata_showDemoWindow"].asBool());
    }
    if (ConfigData.isMember("MainUIdata_showDebugWindow")) {
        props.SetValue("MainUIdata", "showDebugWindow", ConfigData["MainUIdata_showDebugWindow"].asBool());
    }
    if (ConfigData.isMember("MainUIdata_showLogWindow")) {
        props.SetValue("MainUIdata", "showLogWindow", ConfigData["MainUIdata_showLogWindow"].asBool());
    }
    if (ConfigData.isMember("MainUIdata_showDrawToolsWindow")) {
        props.SetValue("MainUIdata", "showDrawToolsWindow", ConfigData["MainUIdata_showDrawToolsWindow"].asBool());
    }
    if (ConfigData.isMember("MainUIdata_themeID")) {
        props.SetValue("MainUIdata", "themeID", ConfigData["MainUIdata_themeID"].asInt());
    }

    // 读取 MainDrawData 背景色
    if (ConfigData.isMember("MainDrawData_backGroundColor")) {
        const auto& color = ConfigData["MainDrawData_backGroundColor"];
        std::vector<float> bkcolor = {
            color["R"].asFloat(),
            color["G"].asFloat(),
            color["B"].asFloat(),
            color["A"].asFloat()
        };
        props.SetValue("MainDrawData", "backGroundColor", bkcolor);
    }

    // 读取 MainDrawData 画笔颜色
    if (ConfigData.isMember("MainDrawData_BrushColor")) {
        const auto& color = ConfigData["MainDrawData_BrushColor"];
        std::vector<float> brushColor = {
            color["R"].asFloat(),
            color["G"].asFloat(),
            color["B"].asFloat(),
            color["A"].asFloat()
        };
        props.SetValue("MainDrawData", "BrushColor", brushColor);
    }

    // 读取 MainDrawData 其他属性
    if (ConfigData.isMember("MainDrawData_BrushThickness")) {
        props.SetValue("MainDrawData", "BrushThickness", ConfigData["MainDrawData_BrushThickness"].asFloat());
    }
    if (ConfigData.isMember("MainDrawData_EraserSize")) {
        props.SetValue("MainDrawData", "EraserSize", ConfigData["MainDrawData_EraserSize"].asFloat());
    }
    if (ConfigData.isMember("MainDrawData_scaleSpeed")) {
        props.SetValue("MainDrawData", "scaleSpeed", ConfigData["MainDrawData_scaleSpeed"].asFloat());
    }

    // 读取 MainDrawData 横纵布局
    if (ConfigData.isMember("MainDrawData_isHorizontal")) {
        props.SetValue("MainDrawData", "isHorizontal", ConfigData["MainDrawData_isHorizontal"].asBool());
    }

    // 读取 MainDrawData 画布大小
    if (ConfigData.isMember("MainDrawData_CanvasSize_int")) {
        const auto& size = ConfigData["MainDrawData_CanvasSize_int"];
        std::vector<float> canvasSize = {
            size["X"].asFloat(),
            size["Y"].asFloat()
        };
        if (canvasSize[0] > 10000)
            canvasSize[0] = 10000;
        if (canvasSize[0] < 10)
            canvasSize[0] = 10;
        if (canvasSize[1] > 10000)
            canvasSize[1] = 10000;
        if (canvasSize[1] < 10)
            canvasSize[1] = 10;
        props.SetValue("MainDrawData", "CanvasSize_int", canvasSize);
    }
}