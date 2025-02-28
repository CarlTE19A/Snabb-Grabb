#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

class ImGuiInterfaceImpl;
class ImGuiInterface
{
public:
	ImGuiInterface();
	~ImGuiInterface();
	void Init();
	//void PreFrame();
	/*void Render();*/

private:
	std::unique_ptr<ImGuiInterfaceImpl> myImpl;
};



