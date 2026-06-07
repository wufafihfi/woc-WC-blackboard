#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

#include "PropertySystem.h"

#include "MainUI.h"
#include "MainDraw.h"
#include "fileTools.h"
#include "dataSavingHandle.h"
#include "ThemeManager.h"

#include "../resource.h"

//###############学习#################
// 阅 - n遍
//####################################

class Application {
public:
    static Application& Get() {
        static Application instance;
        return instance;
    }

    // 禁止拷贝
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // 获取窗口
    sf::RenderWindow& GetWindow() { return *m_window; }

    // 全局属性数据访问
    M_Property::PropertyContainer& GetAppProperties() { return m_appProperties; }

    void Run();

private:
    Application();
    ~Application();

    void InitializeProperties();        // 初始化应用属性
    void ProcessEvents();               // 处理窗口事件

    std::unique_ptr<sf::RenderWindow> m_window;
    sf::Clock m_deltaClock;

    // 属性系统集成
    M_Property::PropertyContainer m_appProperties;              // 应用全局配置

    // 时钟
    sf::Clock m_clock;

    // 模块
    std::unique_ptr<MainUI> m_mainUI;
    std::unique_ptr<MainDraw> m_mainDraw;

    // 自动保存
    float m_saveTimer = 0.0f;
public:
    float m_saveInterval = 20.0f;  // 20秒保存一次
private:
    void autoSave(float deltaTime);
};