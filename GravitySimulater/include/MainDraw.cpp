#include "MainDraw.h"

#include <SFML/Graphics.hpp>
#include <iostream>

#include "M_Application.h"
#include "Log/LogSystem.h"

MainDraw::MainDraw(std::vector<float> canvasSize) :
    m_linesChanged(false),
    mousePosition(0, 0)
{
    LOG_INFO(u8"画布创建 canvas creating");
    m_canvas = std::make_unique<Canvas>(canvasSize[0], canvasSize[1]);
    m_drawer = std::make_unique<SimpleDraw>(*m_canvas);
    m_rview = std::make_unique<RatioView>(m_canvas->getSize());
    LOG_INFO(u8"画布创建成功 canvas create OK");
    saveToUndo();
}

void MainDraw::HandleEvent(const sf::Event& event) {
    if (!m_visible) return;

    auto& props = Application::Get().GetAppProperties();
    sf::RenderWindow& window = Application::Get().GetWindow();

    int toolType = props.GetValue<int>("MainDrawData", "ToolType");

    if (!m_hasFocus) {
        return;
    }

    if (m_imguiActive) {
        return;
    };

    if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButton->button == sf::Mouse::Button::Left) {
            m_isLeftPressed = true;

            if (toolType == 1) {
                // 画笔模式：记录起始点
                m_clickStartPos = M_worldPositon - m_canvas->getPosition() + m_canvas->getSize() * 0.5f;
                m_hasMoved = false;
            }
            else if (toolType == 2) {
                // 橡皮模式
                m_isDrawing = true;
                sf::Vector2f mouseCanvas = M_worldPositon - m_canvas->getPosition() + m_canvas->getSize() * 0.5f;
                m_lastDrawPos = mouseCanvas;

                // 然后执行擦除
                eraseLinesAt(mouseCanvas, props.GetValue<float>("MainDrawData", "EraserSize") / 2.0f);
                m_linesChanged = false;  // 重置标志，因为已经保存了
            }
        }
    }

    // 鼠标移动-标记
    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        if (m_isLeftPressed && toolType == 1) {
            m_hasMoved = true;
        }
    }

    // 鼠标释放
    if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseButton->button == sf::Mouse::Button::Left) {
            // 处理单击画点（画笔模式下按下后没有移动）
            if (toolType == 1 && !m_hasMoved && m_isLeftPressed) {
                float brushThickness = props.GetValue<float>("MainDrawData", "BrushThickness");
                std::vector<float> brushColorVer = props.GetValue<std::vector<float>>("MainDrawData", "BrushColor");
                sf::Color brushColor(
                    static_cast<std::uint8_t>(brushColorVer[0]),
                    static_cast<std::uint8_t>(brushColorVer[1]),
                    static_cast<std::uint8_t>(brushColorVer[2]),
                    static_cast<std::uint8_t>(brushColorVer[3])
                );
                addLine({ m_clickStartPos, m_clickStartPos, brushColor, brushThickness });
                saveToUndo();  // 保存单击的点
                m_linesChanged = false;
            }

            m_isLeftPressed = false;
            m_hasMoved = false;

            // 画笔模式：笔画完成时保存（如果还没保存）
            if (toolType == 1 && m_linesChanged) {
                saveToUndo();
                m_linesChanged = false;
            }
            // 橡皮模式：擦除完成时，如果还有未保存的更改，再保存一次
            else if (toolType == 2 && m_linesChanged) {
                saveToUndo();
                m_linesChanged = false;
            }

            m_isDrawing = false;
        }
    }

    if (toolType == 0) {
        // 拖拽模式
        if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
            if (m_isDragging) {
                sf::Vector2f delta = M_worldPositon - m_lastMousePos;
                m_canvas->setPosition(m_canvas->getPosition() + delta);
                m_lastMousePos = M_worldPositon;
            }
        }
        else if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButton->button == sf::Mouse::Button::Left) {
                m_isDragging = true;
                m_lastMousePos = M_worldPositon;
            }
        }
        else if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseButton->button == sf::Mouse::Button::Left) {
                m_isDragging = false;
            }
        }

        // 缩放
        if (const auto& wheelEvent = event.getIf<sf::Event::MouseWheelScrolled>()) {
            if (wheelEvent && wheelEvent->wheel == sf::Mouse::Wheel::Vertical) {
                float currentScale = m_rview->getScaleZoom();
                float zoomFactor = (wheelEvent->delta > 0) ? (1.0f - scaleSpeed) : (1.0f + scaleSpeed);
                float newScale = currentScale * zoomFactor;
                if (newScale < scaleMin) newScale = scaleMin;
                if (newScale > scaleMax) newScale = scaleMax;
                m_rview->setScaleZoom(newScale);
            }
        }
    }
}

void MainDraw::Render() {
    if (!m_visible) return;

    sf::RenderWindow& window = Application::Get().GetWindow();
    auto& props = Application::Get().GetAppProperties();

    mousePosition = sf::Mouse::getPosition(window);
    M_worldPositon = window.mapPixelToCoords(mousePosition);
    m_hasFocus = window.hasFocus();
    m_imguiActive = ImGui::GetIO().WantCaptureMouse;
    if (!m_hasFocus || m_imguiActive) {
        m_isDrawing = false;
        m_isDragging = false;
        m_isLeftPressed = false;
    }

    int toolType = props.GetValue<int>("MainDrawData", "ToolType");
    scaleSpeed = props.GetValue<float>("MainDrawData", "scaleSpeed"); 

    if (props.GetValue<bool>("MainDrawData", "isCanvasClear")) {
        props.SetValue("MainDrawData", "isCanvasClear", false);
        clearCanvas();
    }
    if (props.GetValue<bool>("MainDrawData", "canvasSave_Flag")) {
        props.SetValue("MainDrawData", "canvasSave_Flag", false);
        m_canvas->SaveToFile(props.GetValue<std::string>("MainDrawData", "canvasSave_Path"));
    }
    if (props.GetValue<bool>("MainDrawData", "canvasTransformationReset_Flag")) {
        props.SetValue("MainDrawData", "canvasTransformationReset_Flag", false);
        m_canvas->setPosition(0, 0);
        m_rview->setScaleZoom(1);
    }

    m_rview->applyTo(window);

    //m_canvas->beginDraw();

    // 画线
    if (props.GetValue<bool>("MainDrawData", "undo_Flag"))
    {
        props.SetValue("MainDrawData", "undo_Flag", false);
        undo();
    }
    if (props.GetValue<bool>("MainDrawData", "redo_Flag"))
    {
        props.SetValue("MainDrawData", "redo_Flag", false);
        redo();
    }
    if (m_needsUpdate) {
        rebuildVertexBuffer();
        m_needsUpdate = false;
        //}
        std::vector<float> bkColorVer = props.GetValue<std::vector<float>>("MainDrawData", "backGroundColor");
        m_drawer->clear(
            static_cast<std::uint8_t>(bkColorVer[0]),
            static_cast<std::uint8_t>(bkColorVer[1]),
            static_cast<std::uint8_t>(bkColorVer[2]),
            static_cast<std::uint8_t>(bkColorVer[3])
        );
        if (!m_vertexBuffer.empty()) {
            m_drawer->drawLinesFromVertices(m_vertexBuffer);
        }
    }
    BB_DrawLine();
    //LOG_INFO_STREAM << u8"线条数:" << m_lines.size();

    m_canvas->endDraw();

    m_canvas->renderToWindow(window);

    // 绘制光标
    static SimpleDraw drawer_window(window);
    if (toolType == 2) {
        float eraserSize = props.GetValue<float>("MainDrawData", "EraserSize");
        drawer_window.setColor(200, 200, 200, 100);
        drawer_window.fillCircle(M_worldPositon.x, M_worldPositon.y, eraserSize / 2.0f, 30);
        drawer_window.setColor(255, 255, 255, 200);
        drawer_window.circle(M_worldPositon.x, M_worldPositon.y, eraserSize / 2.0f);
    }
    else if (toolType == 1) {
        std::vector<float> brushColorVer = props.GetValue<std::vector<float>>("MainDrawData", "BrushColor");
        sf::Color brushColor(
            static_cast<std::uint8_t>(brushColorVer[0]),
            static_cast<std::uint8_t>(brushColorVer[1]),
            static_cast<std::uint8_t>(brushColorVer[2]),
            static_cast<std::uint8_t>(brushColorVer[3])
        );
        float brushThickness = props.GetValue<float>("MainDrawData", "BrushThickness");
        drawer_window.setColor(brushColor);
        drawer_window.fillCircle(M_worldPositon.x, M_worldPositon.y, brushThickness / 2.0f, 30);
        if(brushThickness <= 4)
        {
            drawer_window.setColor(255, 255, 255, 150);
            drawer_window.circle(M_worldPositon.x, M_worldPositon.y, 2);
        }
    }

    //drawDebug_View(window);
}

//画线
void MainDraw::BB_DrawLine() {
    sf::RenderWindow& window = Application::Get().GetWindow();
    auto& props = Application::Get().GetAppProperties();

    int toolType = props.GetValue<int>("MainDrawData", "ToolType");

    sf::Vector2f mouseCanvas = M_worldPositon - m_canvas->getPosition() + m_canvas->getSize() * 0.5f;

    std::vector<float> brushColorVer = props.GetValue<std::vector<float>>("MainDrawData", "BrushColor");
    sf::Color brushColor(
        static_cast<std::uint8_t>(brushColorVer[0]),
        static_cast<std::uint8_t>(brushColorVer[1]),
        static_cast<std::uint8_t>(brushColorVer[2]),
        static_cast<std::uint8_t>(brushColorVer[3])
    );

    if (toolType == 1) {  // 画笔模式
        bool isLeftPressed = m_isLeftPressed;
        if (!isLeftPressed && m_hasFocus && !m_imguiActive) {
            isLeftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        }

        if (!m_imguiActive && isLeftPressed && m_hasFocus) {
            float brushThickness = props.GetValue<float>("MainDrawData", "BrushThickness");

            if (!m_isDrawing) {
                if (m_undoStack.empty() || m_lines != m_undoStack.back()) {
                    saveToUndo();
                }
                m_isDrawing = true;
                m_lastDrawPos = mouseCanvas;
                m_linesChanged = false;
            }
            else {
                if (mouseCanvas != m_lastDrawPos) {
                    addLine({ m_lastDrawPos, mouseCanvas, brushColor, brushThickness });
                    m_lastDrawPos = mouseCanvas;
                    m_linesChanged = true;
                }
            }
        }
        else {
            if (m_isDrawing && m_linesChanged) {
                LOG_DEBUG_STREAM << "Mouse release - m_isDrawing - m_linesChanged: " << m_isDrawing
                    << ", m_linesChanged: " << m_linesChanged
                    << ", lines: " << m_lines.size();
                saveToUndo();
                m_linesChanged = false;
            }
            m_isDrawing = false;
        }
    }
    else if (toolType == 2) {  // 橡皮模式
        bool isLeftPressed = m_isLeftPressed;
        if (!isLeftPressed && m_hasFocus && !m_imguiActive) {
            isLeftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        }

        if (!m_imguiActive && isLeftPressed && m_hasFocus) {
            float eraserSize = props.GetValue<float>("MainDrawData", "EraserSize");

            if (!m_isDrawing) {
                // 开始擦除
                m_isDrawing = true;
                m_lastDrawPos = mouseCanvas;
            }
            else {
                // 线段插值删除
                sf::Vector2f delta = mouseCanvas - m_lastDrawPos;
                float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                float step = eraserSize / 4.0f;

                if (distance > step) {
                    int steps = static_cast<int>(distance / step) + 1;
                    for (int i = 1; i <= steps; i++) {
                        float t = (float)i / steps;
                        sf::Vector2f interpPos = m_lastDrawPos + delta * t;
                        eraseLinesAt(interpPos, eraserSize / 2.0f);
                    }
                }
                else {
                    eraseLinesAt(mouseCanvas, eraserSize / 2.0f);
                }
                m_lastDrawPos = mouseCanvas;
            }
        }
        else {
            // 鼠标释放，完成擦除
            if (m_isDrawing && m_linesChanged) {
                saveToUndo();  // 保存擦除后的状态
                m_linesChanged = false;
            }
            m_isDrawing = false;
        }
    }
}
void MainDraw::rebuildVertexBuffer() {
    m_vertexBuffer.clear();
    m_vertexBuffer.reserve(m_lines.size() * 100);

    //LOG_DEBUG_STREAM << "Rebuilding vertex buffer for " << m_lines.size() << " lines";

    for (const auto& line : m_lines) {
        int segments = 12;
        if (line.thickness >= 10) {
            segments = 24;
        }
        else if (line.thickness >= 20) {
            segments = 32;
        }

        m_drawer->roundedThickLineToVertices(
            line.start.x, line.start.y,
            line.end.x, line.end.y,
            line.thickness, line.color,
            m_vertexBuffer,
            segments,
            1
        );
    }
}
// 获取网格键值
int MainDraw::getCellKey(float x, float y, float cellSize) {
    int cx = (int)(x / cellSize);
    int cy = (int)(y / cellSize);
    return cx * 10000 + cy;
}
// 重建空间索引
void MainDraw::rebuildSpatialIndex() {
    m_spatialIndex.clear();

    for (size_t i = 0; i < m_lines.size(); i++) {
        const auto& line = m_lines[i];

        int minX = std::min(line.start.x, line.end.x);
        int maxX = std::max(line.start.x, line.end.x);
        int minY = std::min(line.start.y, line.end.y);
        int maxY = std::max(line.start.y, line.end.y);

        int startCellX = (int)(minX / m_cellSize);
        int endCellX = (int)(maxX / m_cellSize);
        int startCellY = (int)(minY / m_cellSize);
        int endCellY = (int)(maxY / m_cellSize);

        for (int cx = startCellX; cx <= endCellX; cx++) {
            for (int cy = startCellY; cy <= endCellY; cy++) {
                int key = cx * 10000 + cy;
                m_spatialIndex[key].push_back((int)i);
            }
        }
    }
}
// 添加线条时标记需要更新
void MainDraw::addLine(const DrawLine& line) {
    m_lines.push_back(line);
    m_needsUpdate = true;
    m_linesChanged = true;
    //LOG_DEBUG_STREAM << "addLine - lines: " << m_lines.size() << ", linesChanged: " << m_linesChanged;
}
// 清空画布
void MainDraw::clearCanvas() {
    if (m_lines.empty()) return;

    saveToUndo();
    m_lines.clear();
    m_vertexBuffer.clear();
    m_needsUpdate = true;
}
// 检测线段与圆是否相交
bool MainDraw::lineIntersectsCircle(const sf::Vector2f& p1, const sf::Vector2f& p2,
    const sf::Vector2f& center, float radius) {
    float radiusSq = radius * radius;

    // 检查端点
    sf::Vector2f d1 = p1 - center;
    sf::Vector2f d2 = p2 - center;
    if (d1.x * d1.x + d1.y * d1.y <= radiusSq) return true;
    if (d2.x * d2.x + d2.y * d2.y <= radiusSq) return true;

    // 检查线段到圆心的最短距离
    sf::Vector2f dir = p2 - p1;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 0.0001f) return false;

    dir /= len;
    sf::Vector2f toCenter = center - p1;
    float t = toCenter.x * dir.x + toCenter.y * dir.y;

    if (t < 0) t = 0;
    if (t > len) t = len;

    sf::Vector2f closest = p1 + dir * t;
    sf::Vector2f diff = closest - center;
    float distSq = diff.x * diff.x + diff.y * diff.y;

    return distSq <= radiusSq;
}
// 删除与圆相交的线条
void MainDraw::eraseLinesAt(const sf::Vector2f& pos, float radius) {
    // 重建索引
    rebuildSpatialIndex();

    // 获取橡皮周围的网格
    int cx = (int)(pos.x / m_cellSize);
    int cy = (int)(pos.y / m_cellSize);
    std::vector<int> keysToCheck;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            keysToCheck.push_back((cx + dx) * 10000 + (cy + dy));
        }
    }

    std::vector<bool> toDelete(m_lines.size(), false);

    if(!toDelete.empty())
    {
        for (int key : keysToCheck) {
            auto it = m_spatialIndex.find(key);
            if (it != m_spatialIndex.end()) {
                for (int idx : it->second) {
                    if (idx >= 0 && idx < (int)m_lines.size() && !toDelete[idx]) {
                        const auto& line = m_lines[idx];
                        if (lineIntersectsCircle(line.start, line.end, pos, radius)) {
                            toDelete[idx] = true;
                        }
                    }
                }
            }
        }

        // 从后往前删除
        bool anyDeleted = false;
        for (int i = (int)m_lines.size() - 1; i >= 0; i--) {
            if (toDelete[i]) {
                m_lines.erase(m_lines.begin() + i);
                anyDeleted = true;
            }
        }

        if (anyDeleted) {
            m_linesChanged = true;  // 标记有变化（关键：添加这行）
            m_needsUpdate = true;
            rebuildSpatialIndex();
        }
    }
}
// 撤销恢复
void MainDraw::finishStroke() {
    if (m_linesChanged) {  // 需要添加标志
        saveToUndo();
        m_linesChanged = false;
    }
}
void MainDraw::saveToUndo() {
    LOG_DEBUG_STREAM << "=== saveToUndo CALLED ===";
    LOG_DEBUG_STREAM << "Call stack? lines: " << m_lines.size();

    m_undoStack.push_back(m_lines);

    LOG_DEBUG_STREAM << "After push, stack size: " << m_undoStack.size();
    for (size_t i = 0; i < m_undoStack.size(); i++) {
        LOG_DEBUG_STREAM << "  [" << i << "] lines: " << m_undoStack[i].size();
    }

    while (m_undoStack.size() > m_maxUndoSteps) {
        LOG_DEBUG_STREAM << "Erasing oldest, lines: " << m_undoStack.front().size();
        m_undoStack.erase(m_undoStack.begin());
    }

    clearRedo();

    LOG_DEBUG_STREAM << "=== saveToUndo END, stack size: " << m_undoStack.size() << " ===";
}

void MainDraw::undo() {
    if (m_undoStack.empty()) {
        LOG_DEBUG_STREAM << "Nothing to undo";
        return;
    }

    // 保存当前状态到恢复栈
    m_redoStack.push_back(m_lines);

    // 从撤销栈恢复上一个状态
    m_lines = m_undoStack.back();
    m_undoStack.pop_back();

    // 更新渲染 - 关键修复
    m_needsUpdate = true;
    rebuildSpatialIndex();  // 重建空间索引
    m_linesChanged = false;  // 重置变化标志

    LOG_DEBUG_STREAM << "Undo, lines: " << m_lines.size();
}

void MainDraw::redo() {
    if (m_redoStack.empty()) {
        LOG_DEBUG_STREAM << "Nothing to redo";
        return;
    }

    // 保存当前状态到撤销栈
    m_undoStack.push_back(m_lines);

    // 从恢复栈恢复下一个状态
    m_lines = m_redoStack.back();
    m_redoStack.pop_back();

    // 更新渲染 - 关键修复
    m_needsUpdate = true;
    rebuildSpatialIndex();  // 重建空间索引
    m_linesChanged = false;  // 重置变化标志

    LOG_DEBUG_STREAM << "Redo, lines: " << m_lines.size();
}

void MainDraw::clearRedo() {
    m_redoStack.clear();
}

void MainDraw::drawDebug_View(sf::RenderWindow& window) {
    sf::Vector2f center = window.getView().getCenter();
    sf::Vector2f size = window.getView().getSize();
    float left = center.x - size.x / 2;
    float right = center.x + size.x / 2;
    float top = center.y - size.y / 2;
    float bottom = center.y + size.y / 2;
    static SimpleDraw m2_drawer(window);
    // X轴（红色）
    m2_drawer.setColor(255, 0, 0, 150);
    m2_drawer.thickLine(left, center.y, right, center.y, 10);
    // Y轴（绿色）
    m2_drawer.setColor(0, 255, 0, 150);
    m2_drawer.thickLine(center.x, top, center.x, bottom, 10);
    // 原点标记（黄色圆点）
    m2_drawer.setColor(255, 255, 0, 150);
    m2_drawer.fillCircle(center.x, center.y, 5);
    // 轴箭头
    // X轴箭头
    m2_drawer.setColor(255, 0, 0, 150);
    m2_drawer.thickLine(right - 30, center.y - 10, right, center.y, 10);
    m2_drawer.thickLine(right - 30, center.y + 10, right, center.y, 10);
    // Y轴箭头
    m2_drawer.setColor(0, 255, 0, 150);
    m2_drawer.thickLine(center.x - 10, bottom - 30, center.x, bottom, 10);
    m2_drawer.thickLine(center.x + 10, bottom - 30, center.x, bottom, 10);

    m2_drawer.setColor(255, 255, 0, 150);
    m2_drawer.fillCircle(M_worldPositon.x, M_worldPositon.y, 50);
}