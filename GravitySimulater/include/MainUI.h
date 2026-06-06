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
};
