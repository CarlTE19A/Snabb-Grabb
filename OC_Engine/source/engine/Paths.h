#pragma once
#define STATIC_ASSET_PATH "../assets/"
#define STATIC_SETTINGS_PATH "../bin/settings/"
#define STATIC_POSTPROCESSING_PATH "../bin/postprocessing/"
#define STATIC_EDITOR_PATH "../assets/EditorAssets/"
#define STATIC_SHADER_PATH "../bin/shaders/"
#define STATIC_ASSET_JSON_SCRIPT_PATH "../assets/json/ScriptData/"

// TODO: premake hlsl output dir files, and includes
#ifndef _RETAIL

//Uses unity if in debug
#define RELATIVE_SCENE_PATH "../../Unity/Assets/Export/"
#define RELATIVE_ASSET_PATH "../../Unity/Assets/"
#define RELATIVE_ASSET_ASSET_PATH "../../unity/assets/assets/"
#define RELATIVE_MATERIAL_PATH "../../Unity/Assets/Assets/Materials/"
#define RELATIVE_ORGIN_PATH "../../Unity/"
#else

//Uses engine if not in debug
#define RELATIVE_SCENE_PATH "../assets/Scenes/"
#define RELATIVE_ASSET_PATH "../assets/"
#define RELATIVE_ASSET_ASSET_PATH "../assets/assets/"
#define RELATIVE_MATERIAL_PATH "../assets/assets/Materials/"
#define RELATIVE_ORGIN_PATH "../"
#endif
