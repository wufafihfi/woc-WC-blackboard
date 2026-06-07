#include "M_Application.h"

#include <imgui-SFML.h>

#include "Log/LogSystem.h"

Application::Application() {
    // 初始化应用属性
    InitializeProperties();
}

Application::~Application() {
    SaveConfig();
    ImGui::SFML::Shutdown();
}

void Application::InitializeProperties() {
    fs::path exePath = getExecutablePath();
    fs::path fontPath = getExecutablePath() / "font";
    // fileLoadTool里有定义（不使用这里的值）
    fs::path defaultFontPath = getExecutablePath() / "font" / "DefaultFont" / "SimHei.ttf";
    fs::path mainLogFile = getExecutablePath() / "log" / "MainLog.log";
    /*
    std::cout << u8"程序目录: " << exePath.u8string() << std::endl;
    std::cout << u8"字体目录: " << fontPath.u8string() << std::endl;
    std::cout << u8"默认字体: " << defaultFontPath.u8string() << std::endl;
    */
    m_appProperties.AddGroup("MainData");
    m_appProperties.AddProperty("MainData", M_Property::Property("BasePath", "程序目录路径", exePath.u8string()));
    m_appProperties.AddProperty("MainData", M_Property::Property("FontFilePath", "字体文件夹路径", fontPath.u8string()));
    m_appProperties.AddProperty("MainData", M_Property::Property("DefaultFont_SimHei_Path", "默认字体路径", defaultFontPath.u8string()));
    m_appProperties.AddProperty("MainData", M_Property::Property("MainLogFile", "主日志文件", mainLogFile.u8string()));

    m_appProperties.AddGroup("ChangeFont");
    m_appProperties.AddProperty("ChangeFont", M_Property::Property("ChangeFontFlag", "字体切换消息标志_线程安全", false));
    m_appProperties.AddProperty("ChangeFont", M_Property::Property("TargetFontPath", "要切换的字体_线程安全", defaultFontPath.u8string()));
    m_appProperties.AddProperty("ChangeFont", M_Property::Property("FontChangedFlag", "字体切换成功标志_线程安全", false));

    m_appProperties.AddGroup("MainUIdata");
    m_appProperties.AddProperty("MainUIdata", M_Property::Property("Exit", "退出", false));
    m_appProperties.AddProperty("MainUIdata", M_Property::Property("nowFontID", "正在使用的字体的ID", 0));
    m_appProperties.AddProperty("MainUIdata", M_Property::Property("IMGUIscale", "界面缩放", 1.0f));
    m_appProperties.AddProperty("MainUIdata", M_Property::Property("FontScale", "字体缩放", 16));
    m_appProperties.AddProperty("MainUIdata", M_Property::Property("showDemoWindow", "测试窗口-IMGUI", false));
    m_appProperties.AddProperty("MainUIdata", M_Property::Property("showDebugWindow", "调试窗口-IMGUI日志", false));
    m_appProperties.AddProperty("MainUIdata", M_Property::Property("showLogWindow", "调试窗口-程序日志", false));
    m_appProperties.AddProperty("MainUIdata", M_Property::Property("showDrawToolsWindow", "绘制工具窗口", false));
    m_appProperties.AddProperty("MainUIdata", M_Property::Property("BackgroundColor", "背景颜色", std::vector<float>{0.2f, 0.2f, 0.2f}));
    m_appProperties.AddProperty("MainUIdata", M_Property::Property("themeID", "样式预设", 0));

    m_appProperties.AddGroup("MainDrawData");
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("CanvasSize_int", "画布大小", std::vector<float>{ 2560.0f, 1440.0f}));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("backGroundColor", "背景色", std::vector<float>{ 10.0f, 30.0f, 10.0f, 255.0f }));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("ToolType", "工具类型", 0));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("isCanvasClear", "画布清空标志", false));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("isHorizontal", "横纵布局", false));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("BrushColor", "画笔颜色", std::vector<float>{ 255.0f,255.0f,255.0f,255.0f }));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("BrushThickness", "画笔粗细", 10.0f));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("EraserSize", "橡皮大小", 100.0f));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("scaleSpeed", "画布缩放速度", 0.1f));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("undo_Flag", "撤销标志", false));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("redo_Flag", "恢复标志", false));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("canvasSave_Flag", "画布保存标志", false));
    m_appProperties.AddProperty("MainDrawData", M_Property::Property("canvasSave_Path", "画布保存路径", (getExecutablePath() / "savedCanvas" / "canvas.png").u8string()));
}

void Application::ProcessEvents() {
    while (const auto event = m_window->pollEvent()) {
        ImGui::SFML::ProcessEvent(*m_window, *event);

        // 事件传入模块
        m_mainDraw->HandleEvent(*event);

        if (event->is<sf::Event::Closed>()) {
            m_window->close();
        }
    }

    // 自定义事件
    if (m_appProperties.GetValue<bool>("ChangeFont", "ChangeFontFlag")) {
        bool setFlag = setFont(m_appProperties.GetValue<std::string>("MainData", "DefaultFont_SimHei_Path"),
            m_appProperties.GetValue<std::string>("ChangeFont", "TargetFontPath"),
            m_appProperties.GetValue<int>("MainUIdata", "FontScale"));
        m_appProperties.SetValue("ChangeFont", "FontChangedFlag", true);
    }
}

void Application::Run() {
    // 基本功能初始化####################################################################
    //日志系统初始化
    LogSystem::getInstance().init(m_appProperties.GetValue<std::string>("MainData", "MainLogFile"), LogLevel::DEBUG, true, true, 5000);
    LogSystem::getInstance().setWindowTitle(u8"日志");
    //启动日志
    LOG_INFO(u8"程序启动 Application started");

    LOG_INFO(u8"日志测试 log testing [LOG_INFO] ");
    LOG_WARNING(u8"日志测试 log testing [LOG_WARNING] ");
    LOG_ERROR(u8"日志测试 log testing [LOG_ERROR] ");
    LOG_DEBUG(u8"日志测试 log testing [LOG_DEBUG] ");
    LOG_INFO_STREAM << u8"日志测试 log testing [LOG_INFO_STREAM] " << 123 << " NUM";
    LOG_WARNING_STREAM << u8"日志测试 log testing [LOG_WARNING_STREAM] " << "123 STR";
    LOG_ERROR_STREAM << u8"日志测试 log testing [LOG_ERROR_STREAM] " << true << " BOOL";
    LOG_DEBUG_STREAM << u8"日志测试 log testing [LOG_DEBUG_STREAM] " << false << " BOOL";

    LOG_INFO_STREAM << u8"程序目录 application directory:" << m_appProperties.GetValue<std::string>("MainData", "BasePath");
    LOG_INFO_STREAM << u8"字体目录 font directory:" << m_appProperties.GetValue<std::string>("MainData", "FontFilePath");
    LOG_INFO_STREAM << u8"默认字体路径 defaultFont path:" << m_appProperties.GetValue<std::string>("MainData", "DefaultFont_SimHei_Path");

    // 读取配置
    LOG_INFO(u8"读取配置 Load config");
    LoadConfig();

    // 创建 SFML 窗口
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 4;

    sf::Vector2u desktopSize = sf::VideoMode::getDesktopMode().size;
    m_window = std::make_unique<sf::RenderWindow>(sf::VideoMode({ 1000, 700 }),
        L"沃c黑板",
        sf::Style::Default,
        sf::State::Windowed,
        settings);
    m_window->setVerticalSyncEnabled(true);
    m_window->setFramerateLimit(120);
    //最大化
#ifdef _WIN32
    HWND hwnd = m_window->getNativeHandle();
    ShowWindow(hwnd, SW_MAXIMIZE);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1_128)));
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1_32)));
#endif

    //启动日志
    LOG_INFO(u8"窗口创建 Window created");

    // 2. 初始化 ImGui-SFML
    if (!ImGui::SFML::Init(*m_window)) {
        return;
    }
    LOG_INFO(u8"IMGUI初始化成功 ImGui init OK");
    // 基本功能初始化####################################################################

    // 样式定义####################################################################
    // 颜色主题配置应用
    int savedTheme = m_appProperties.GetValue<int>("MainUIdata", "themeID");
    ThemeManager::Get().ApplyTheme(savedTheme, true);

    // 获取当前的样式对象
    ImGuiStyle& style = ImGui::GetStyle();

    // 全局透明
    style.Alpha = 0.8f;

    // 设置各项圆角半径
    style.WindowRounding = 6.0f;      // 主窗口的圆角[citation:9]
    style.ChildRounding = 6.0f;       // 子窗口的圆角
    style.FrameRounding = 5.0f;       // 框架类控件（如按钮）的圆角[citation:3]
    style.PopupRounding = 6.0f;       // 弹出窗口的圆角
    style.ScrollbarRounding = 9.0f;   // 滚动条的圆角
    style.GrabRounding = 4.0f;        // 滑块把手的圆角
    style.TabRounding = 4.0f;         // 标签页的圆角[citation:2]
    // 边框大小
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    ImGuiIO& io = ImGui::GetIO();
    // DPI-imgui
    io.FontGlobalScale = 1.0; // or any other value hardcoded or loaded from your config logic

    // Docking
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // 启用 Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // 窗口位置保存
    std::string _basepath = m_appProperties.GetValue<std::string>("MainData", "BasePath");
    static std::string layoutiniPath = _basepath + u8"\\config\\imgui_layout.ini";
    io.IniFilename = layoutiniPath.c_str();

    // 字体
    LOG_INFO(u8"字体读取开始 Font loading starts...");
    io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
    io.Fonts->TexDesiredWidth = 2048;

    std::string _fontFilePath = m_appProperties.GetValue<std::string>("MainData", "FontFilePath");
    fontLoadTool(_fontFilePath, io, m_appProperties);
    LOG_INFO(u8"字体读取成功 Font loaded OK");

    // 样式定义####################################################################
    // 模块创建
    m_mainUI = std::make_unique<MainUI>();
    std::vector<float> canvasSize = m_appProperties.GetValue<std::vector<float>>("MainDrawData", "CanvasSize_int");
    m_mainDraw = std::make_unique<MainDraw>(canvasSize);
    // 模块创建

    while (m_window->isOpen()) {
        float deltaTime = m_clock.restart().asSeconds();
        // 获取需要的属性
        auto& props = GetAppProperties();
        if (props.GetValue<bool>("MainUIdata", "Exit")) {
            m_window->close();
            break;
        }

        // 自动保存
        autoSave(deltaTime);

        //事件获取
        ProcessEvents();
        ImGui::SFML::Update(*m_window, m_deltaClock.restart());


        // 创建全屏 DockSpace
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
        // UI绘制
        m_mainUI->Render();

        // 渲染
        std::vector<float> color = props.GetValue<std::vector<float>>("MainUIdata", "BackgroundColor");
        m_window->clear(sf::Color(
            static_cast<std::uint8_t>(color[0] * 255),
            static_cast<std::uint8_t>(color[1] * 255),
            static_cast<std::uint8_t>(color[2] * 255)
        ));
        //自定义绘图
        m_mainDraw->Render();
        //imgui
        ImGui::SFML::Render(*m_window);
        m_window->display();
    }
}

void Application::autoSave(float deltaTime) {
    m_saveTimer += deltaTime;

    // 定时保存（只在有变化时才会真正写入文件）
    if (m_saveTimer >= m_saveInterval) {
        LOG_INFO(u8"自动保存 auto save");
        SaveConfig();
        m_saveTimer = 0.0f;
    }
}