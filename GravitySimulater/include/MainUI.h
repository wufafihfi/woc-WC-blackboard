#pragma once
#include <vector>

#include <imgui.h>
#include <imgui-SFML.h>

#include "PropertySystem.h"

class MainUI
{
public:

public:
    MainUI() {};
    ~MainUI() {};

    void Render();

	void SetVisible(bool visible) { m_visible = visible; }
	bool IsVisible() const { return m_visible; }

private:
    bool m_visible = true;

    void handleCanvasSave();
    void handleCanvasSaveAs();
    void handleOpenSaveFolder();
    void renderOverwriteDialog();
    void renderSaveAsDialog();
    // ¸²¸ÇÈ·ÈÏµ¯´°×´Ì¬
    bool m_showOverwriteDialog = false;
    std::string m_overwriteTempPath;
    // Áí´æÎªµ¯´°×´Ì¬
    bool m_showSaveAsDialog = false;
    std::string m_saveAsFileName;
    std::string m_saveAsDirectory;
};
