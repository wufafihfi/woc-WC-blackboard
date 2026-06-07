#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <deque>
#include <algorithm>
#include <cmath>

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
    float thickness = 1.0f;

    bool operator==(const DrawLine& other) const {
        return start == other.start &&
            end == other.end &&
            color == other.color &&
            thickness == other.thickness;
    }

    bool operator!=(const DrawLine& other) const {
        return !(*this == other);
    }
};

class MainDraw
{
public:
    MainDraw(std::vector<float> canvasSize);
    ~MainDraw() {}

    void HandleEvent(const sf::Event& event);
    void Render();

    void SetVisible(bool visible) { m_visible = visible; }
    bool IsVisible() const { return m_visible; }

    // 撤销/恢复公共接口
    void undo();
    void redo();

private:
    bool m_visible = true;

    std::unique_ptr<Canvas> m_canvas;
    std::unique_ptr<SimpleDraw> m_drawer;
    std::unique_ptr<RatioView> m_rview;

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
    sf::Vector2f m_clickStartPos;  // 单击开始位置
    bool m_hasMoved = false;       // 是否移动过
    bool m_isDrawing = false;      // 是否正在绘制
    bool m_isLeftPressed = false;
    sf::Vector2f m_lastDrawPos;    // 上一个绘制点
    std::vector<DrawLine> m_lines;
    std::vector<sf::Vertex> m_vertexBuffer;  // 顶点缓冲区
    bool m_needsUpdate = true;               // 是否需要重建缓冲区
    bool m_linesChanged = false;              // 新增：标记线条是否有变化（用于撤销）

    void BB_DrawLine();
    void rebuildVertexBuffer();
    void addLine(const DrawLine& line);
    void clearCanvas();
    void finishStroke(); // 新增：完成笔画时调用

    // 橡皮擦删除
    void eraseLinesAt(const sf::Vector2f& pos, float radius);
    bool lineIntersectsCircle(const sf::Vector2f& p1, const sf::Vector2f& p2,
        const sf::Vector2f& center, float radius);
    int getCellKey(float x, float y, float cellSize = 50.0f);
    void rebuildSpatialIndex();
    // 空间索引（用于加速橡皮擦删除）
    std::unordered_map<int, std::vector<int>> m_spatialIndex;
    float m_cellSize = 50.0f;

    // 撤销/恢复系统（新增）
    std::vector<std::vector<DrawLine>> m_undoStack;   // 撤销栈
    std::vector<std::vector<DrawLine>> m_redoStack;   // 恢复栈
    size_t m_maxUndoSteps = 50;                        // 最大撤销步数

    void saveToUndo();      // 保存当前状态到撤销栈
    void clearRedo();       // 清空恢复栈

    //调试
    void drawDebug_View(sf::RenderWindow& window);
};