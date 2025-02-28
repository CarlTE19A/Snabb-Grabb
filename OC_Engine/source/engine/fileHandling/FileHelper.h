//#include <filesystem>
//#include "graphics\Texture.h"
//#include "imgui\imgui.h"
//#include <d3d11.h>
//enum class FileType
//{
//	eUnSet,
//	eFBX,
//	ePNG,
//	eDDS,
//	eOBJ,
//	eJSON,
//	eFolder,
//	eMeta,
//	eUnknown,
//};
//struct FileTypeData
//{
//	FileTypeData(FileType aFileType = FileType::eUnSet, Texture* aTexture = new Texture())
//	{
//		myFileType = aFileType;
//		myTexture = aTexture;
//	}
//	FileType myFileType = FileType::eUnSet;
//	Texture* myTexture;
//};
//class FileHelper
//{
//public:
//	FileHelper();
//	~FileHelper();
//
//	void Init();
//	void DragFile(std::filesystem::directory_entry aFile, bool aExternal, FileTypeData* aFileType = new FileTypeData(FileType::eUnSet, nullptr));
//	FileTypeData* GetTypeFromFile(std::filesystem::directory_entry aFile);
//	//Texture* GetTextureFromFile(std::filesystem::directory_entry aFile);
//
//	FileTypeData* texture2;
//	FileTypeData* pngData;
//	FileTypeData* fbxData;
//	FileTypeData* ddsData;
//	FileTypeData* objData;
//	FileTypeData* jsonData;
//	FileTypeData* folderData;
//	FileTypeData* metaFileData;
//	FileTypeData* unknownData;
//private:
//};