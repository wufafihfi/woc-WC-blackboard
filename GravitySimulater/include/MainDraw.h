#pragma once
#include <vector>

#include <imgui.h>
#include <imgui-SFML.h>

// 基础
#include "PropertySystem.h"
#include "textInputTool.h"

//绘图
#include "LsSimpleDraw/LsSimpleDraw.hpp"

struct DrawLine {
    sf::Vector2f start;
    sf::Vector2f end;
    sf::Color color;
    float thickness;
};

class MainDraw
{
public:

public:
    MainDraw()
        : m_canvas(3840, 2160)
        , m_drawer(m_canvas)
        , m_rview(sf::Vector2f(m_canvas.getSize()))
        , mousePosition(0, 0)
    { 
    }
    ~MainDraw() {}

    void HandleEvent(const sf::Event& event);
    void Render();

    void SetVisible(bool visible) { m_visible = visible; }
    bool IsVisible() const { return m_visible; }

private:
    bool m_visible = true;

    Canvas m_canvas;
    SimpleDraw m_drawer;
    RatioView m_rview;

    //鼠标位置
    sf::Vector2i mousePosition;
    sf::Vector2f M_worldPositon;
    bool m_hasFocus = false;
    bool m_imguiActive = false;

    // 画布拖拽
    bool m_isDragging = false;
    sf::Vector2f m_lastMousePos;
    // 画布缩放
    float scaleMax = 10.0f;
    float scaleMin = 0.1f;
    float scaleSpeed = 0.1f;

    // 线条绘制
    bool m_isDrawing = false;      // 是否正在绘制
    bool m_isLeftPressed = false;
    sf::Vector2f m_lastDrawPos;    // 上一个绘制点
    std::vector<DrawLine> m_lines;
    std::vector<sf::Vertex> m_vertexBuffer;  // 顶点缓冲区
    bool m_needsUpdate = true;               // 是否需要重建缓冲区
    void BB_DrawLine();
    void rebuildVertexBuffer();
    void addLine(const DrawLine& line);
    void clearCanvas();
    // 橡皮擦删除
    void eraseLinesAt(const sf::Vector2f& pos, float radius);
    bool lineIntersectsCircle(const sf::Vector2f& p1, const sf::Vector2f& p2,
        const sf::Vector2f& center, float radius);
    int getCellKey(float x, float y, float cellSize = 50.0f);
    void rebuildSpatialIndex();
    // 空间索引（用于加速橡皮擦删除）
    std::unordered_map<int, std::vector<int>> m_spatialIndex;
    float m_cellSize = 50.0f;

    //调试
    void drawDebug_View(sf::RenderWindow& window);
};