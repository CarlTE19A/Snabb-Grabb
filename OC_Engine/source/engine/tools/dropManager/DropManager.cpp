#include "stdafx.h"
#include <oleidl.h>
#include <filesystem>
#include "DropManager.h"

#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"

#include "mainSingelton\MainSingelton.h"
#include "events\EventSystem.h"

HRESULT DropManager::QueryInterface(REFIID riid, void** ppvObject)
{
	if (riid == IID_IDropTarget)
	{
		*ppvObject = this;	// or static_cast<IUnknown*> if preferred
		// AddRef() if doing things properly
					// but then you should probably handle IID_IUnknown as well;
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

HRESULT DropManager::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	pDataObj;
	grfKeyState;
	pt;

	FORMATETC fmte = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgm;
	if (SUCCEEDED(pDataObj->GetData(&fmte, &stgm)))
	{
		HDROP hDrop = (HDROP)stgm.hGlobal; // or reinterpret_cast<HDROP> if preferred

		UINT size = DragQueryFile(hDrop, 0, NULL, 0);
		std::wstring fileName;
		fileName.resize(size);
		DragQueryFile(hDrop, 0, fileName.data(), size + 1);

		std::replace(fileName.begin(), fileName.end(), '\\', '/');

		std::filesystem::path path = fileName.data();

		std::string extension = path.extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(),
			[](unsigned char c) { return std::tolower(c); });

		//Taken over by myExtensions
		char* extensionChar = new char[extension.length() + 1];
		strcpy_s(extensionChar, extension.length() + 1, extension.c_str());

		ClearDrop();
		myPath = new std::string(path.string());
		myExtension = extensionChar;

		BeginDrop();

		ImGui::GetCurrentContext()->IO.MouseDown[0] = true;
	}

	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

HRESULT DropManager::DragLeave()
{
	ClearDrop();
	ImGui::GetCurrentContext()->IO.MouseDown[0] = false;
	return S_OK;
}

HRESULT DropManager::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	grfKeyState;
	pt;

	ImGui::GetCurrentContext()->IO.MousePos.x = (float)pt.x;
	ImGui::GetCurrentContext()->IO.MousePos.y = (float)pt.y;
	BeginDrop();

	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

HRESULT DropManager::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	pDataObj;
	grfKeyState;
	pt;
	ImGui::GetCurrentContext()->IO.MouseDown[0] = false;
	BeginDrop();
	if (MainSingleton::IsInitiated())
	{
		MainSingleton::GetInstance().GetEventSystem().aTempData = *myPath;

		std::filesystem::path path = myPath->data();
		MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::FileDropped, path));
	}


	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

const bool DropManager::BeginDrop()
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceExtern | ImGuiDragDropFlags_SourceNoPreviewTooltip | ImGuiDragDropFlags_SourceNoDisableHover))
	{

		ImGui::SetDragDropPayload(myExtension, static_cast<void*>(myPath), sizeof(myPath));
		ImGui::EndDragDropSource();
		return true;
	}
	LogWarning("BeginDrop failed");
	return false;
}

const void DropManager::ClearDrop()
{
	if (myPath != nullptr)
	{
		delete myPath;
		myPath = nullptr;
	}
	if (myExtension != nullptr)
	{
		delete myExtension;
		myExtension = nullptr;
	}
}
