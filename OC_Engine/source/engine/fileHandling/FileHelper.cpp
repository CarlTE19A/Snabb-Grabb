#include <stdafx.h>
//#include "FileHelper.h"
//FileHelper::FileHelper()
//{
//}
//FileHelper::~FileHelper()
//{
//	delete fbxData;
//	delete pngData;
//	delete ddsData;
//	delete objData;
//	delete jsonData;
//	delete folderData;
//	delete metaFileData;
//	delete unknownData;
//	delete texture2;
//}
//void FileHelper::Init()
//{
//	std::string fbxPath = STATIC_EDITOR_PATH + std::string("FBX.dds");
//	fbxData = new FileTypeData();
//	fbxData->myTexture->Initialize(TextureInitializationType::DDS, fbxPath);
//	fbxData->myFileType = FileType::eFBX;
//
//	std::string pngPath = STATIC_EDITOR_PATH + std::string("PNG.dds");
//	pngData = new FileTypeData();
//	pngData->myTexture->Initialize(TextureInitializationType::DDS, pngPath);
//	pngData->myFileType = FileType::ePNG;
//
//	std::string ddsPath = STATIC_EDITOR_PATH + std::string("DDS.dds");
//	ddsData = new FileTypeData();
//	ddsData->myTexture->Initialize(TextureInitializationType::DDS, ddsPath);
//	ddsData->myFileType = FileType::eDDS;
//
//	std::string objPath = STATIC_EDITOR_PATH + std::string("OBJ.dds");
//	objData = new FileTypeData();
//	objData->myTexture->Initialize(TextureInitializationType::DDS, objPath);
//	objData->myFileType = FileType::eOBJ;
//
//	std::string jsonPath = STATIC_EDITOR_PATH + std::string("JSON.dds");
//	jsonData = new FileTypeData();
//	jsonData->myTexture->Initialize(TextureInitializationType::DDS, jsonPath);
//	jsonData->myFileType = FileType::eJSON;
//
//	std::string folderPath = STATIC_EDITOR_PATH + std::string("Folder.dds");
//	folderData = new FileTypeData();
//	folderData->myTexture->Initialize(TextureInitializationType::DDS, folderPath);
//	folderData->myFileType = FileType::eFolder;
//
//	metaFileData = new FileTypeData();
//	metaFileData->myFileType = FileType::eMeta;
//	metaFileData->myTexture = nullptr;
//
//	std::string unknownPath = STATIC_EDITOR_PATH + std::string("Unknown.dds");
//	unknownData = new FileTypeData();
//	unknownData->myTexture->Initialize(TextureInitializationType::DDS, unknownPath);
//	unknownData->myFileType = FileType::eUnknown;
//
//	std::string fbxPath2 = STATIC_EDITOR_PATH + std::string("FBX.dds");
//	texture2 = new FileTypeData();
//	texture2->myTexture->Initialize(TextureInitializationType::DDS, fbxPath2);
//}
//
//void FileHelper::DragFile(std::filesystem::directory_entry aFile, bool aExternal, FileTypeData* aFileType)
//{
//	ImGuiDragDropFlags flags = ImGuiDragDropFlags_None;
//	if (aExternal)
//	{
//		flags = ImGuiDragDropFlags_SourceExtern | ImGuiDragDropFlags_SourceNoPreviewTooltip;
//	}
//	//else
//	//{
//	//	ImGui::PushID(aFile.path().string().c_str());
//	//	ImGui::SetWindowSize({ 0,0 });
//	//	ImGui::SetWindowPos({ 0,0 });
//	//}
//	//
//	if (aFileType->myFileType == FileType::eUnSet)
//	{
//		aFileType = GetTypeFromFile(aFile);
//	}
//
//	if (ImGui::BeginDragDropSource(flags))
//	{
//		switch (aFileType->myFileType)
//		{
//		case FileType::ePNG:
//			ImGui::SetDragDropPayload("PNG", new std::string(aFile.path().string()), sizeof(std::string));
//			break;
//		case FileType::eDDS:
//			ImGui::SetDragDropPayload("DDS", new std::string(aFile.path().string()), sizeof(std::string));
//			break;
//		case FileType::eFBX:
//			ImGui::SetDragDropPayload("FBX", new std::string(aFile.path().string()), sizeof(std::string));
//			break;
//		case FileType::eOBJ:
//			ImGui::SetDragDropPayload("OBJ", new std::string(aFile.path().string()), sizeof(std::string));
//			break;
//		case FileType::eJSON:
//			ImGui::SetDragDropPayload("Json", new std::string(aFile.path().string()), sizeof(std::string));
//			break;
//		case FileType::eFolder:
//			ImGui::SetDragDropPayload("Folder", new std::string(aFile.path().string()), sizeof(std::string));
//			break;
//		case FileType::eMeta:
//			break;
//		case FileType::eUnknown:
//			ImGui::SetDragDropPayload("Unknown", new std::string(aFile.path().string()), sizeof(std::string));
//			break;
//		default:
//
//			break;
//		}
//		ImGui::EndDragDropSource();
//	}
//	//if (!aExternal)
//	//{
//	//	ImGui::PopID();
//	//}
//}
//
//FileTypeData *FileHelper::GetTypeFromFile(std::filesystem::directory_entry aFile)
//{
//	std::string extension = aFile.path().extension().string();
//	if (extension == ".png")
//	{
//		return pngData;
//	}
//	else if (extension == ".dds")
//	{
//		return ddsData;
//	}
//	else if (extension == ".fbx")
//	{
//		return fbxData;
//	}
//	else if (extension == ".obj")
//	{
//		return objData;
//	}
//	else if (extension == ".json")
//	{
//		return jsonData;
//	}
//	else if (extension == ".meta")
//	{
//		return metaFileData;
//	}
//	else if (extension == "")
//	{
//		return folderData;
//	}
//	else
//	{
//		return unknownData;
//	}
//}