#include "Map_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Camera.h"
Map_Manager::Map_Manager()
{
}

Map_Manager::~Map_Manager()
{
}

HRESULT Map_Manager::Initialize()
{

    filesystem::path folderPath = "../../Resources/Data/Map";

    if (!filesystem::exists(folderPath))
        return E_FAIL;

    for (const auto& entry : filesystem::directory_iterator(folderPath))
    {
        // 파일인지 체크
        if (!entry.is_regular_file())
            continue;

        // 확장자 체크
        if (entry.path().extension() != ".json")
            continue;

        std::ifstream file(entry.path());

        if (!file.is_open())
            continue;

        nlohmann::json j;
        file >> j;

  
        string key = entry.path().stem().string();

        m_mapJsonFiles.emplace(key, std::move(j));
        m_MapNames.emplace_back(key);
    }

    filesystem::path rootPath =
        "../../Resources/Model";

    if (!filesystem::exists(rootPath))
        return E_FAIL;

    for (const auto& entry :
        filesystem::recursive_directory_iterator(rootPath))
    {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension() != ".bin")
            continue;

        filesystem::path relative =
            filesystem::relative(entry.path(), rootPath);


        string fileName =
            relative.filename().string();

        if (fileName.rfind("AN_", 0) == 0)
        {
            continue;
        }
        // 최상위 폴더
        string category = 
            relative.begin()->string();
     

        string folderName =
            relative.parent_path().filename().string();

        groupedFiles[category].emplace_back(folderName);

    } 

    return S_OK;
} 

HRESULT Map_Manager::Save(string _mapDataName, bool _overwrite) {
    using json = nlohmann::json;

    filesystem::path folderPath = "../../Resources/Data/Map";

    if (!filesystem::exists(folderPath))
    {
        filesystem::create_directories(folderPath);
    }

    filesystem::path filePath = folderPath / (_mapDataName + ".json");

    // 이미 존재하는 파일인지 체크
    bool bAlreadyExist = filesystem::exists(filePath);

    if (bAlreadyExist && !_overwrite)
    {
        return E_FAIL;
    }

    json j;

    j["MapName"] = _mapDataName;

    // 실제 데이터 저장
    // -----------------------------------
    // ETOUI(LEVEL::MAPEDITOR) 맵 저장은 map Editor에서만 이루어질거다 
    auto& layerList = CGameInstance::Get().Find_Layer_Lists(3);

    for (auto& [layerName, layerPtr] : layerList)
    {
        // Layer 안의 GameObject 리스트
        auto& gameObjects = layerPtr->Get_GameObjects();

        for (auto& gameObject : gameObjects)
        {
            if (gameObject == nullptr)
                continue;

            // 여기서 저장 처리
            // ----------------------------

            auto& ObjectINfO = gameObject->GetObjectINFO();
            
            json objectJson;
            
            objectJson["ObjectType"] =
                ObjectINfO.ObjectType;

            objectJson["bCollider"] =
                ObjectINfO.m_bCollider;

            objectJson["LevelIndex"] =
                ObjectINfO.LevelIndex;

            objectJson["Layer"] =
                string(layerName.begin(), layerName.end());

            objectJson["Prototype_Object_Name"] =
                string(
                    ObjectINfO.m_strPrototypeObjectName.begin(),
                    ObjectINfO.m_strPrototypeObjectName.end());
            objectJson["Prototype_Base_Name"] =
                string(
                    ObjectINfO.m_strPrototypeBaseName.begin(),
                    ObjectINfO.m_strPrototypeBaseName.end());
            objectJson["Object_Name"] =
                string(
                    ObjectINfO.m_strName.begin(),
                    ObjectINfO.m_strName.end());

            objectJson["CameraType"] =
                ObjectINfO.pCameraType;

            objectJson["fSpeedPerSec"] =
                ObjectINfO.fSpeedPerSec;

            objectJson["fRotationPerSec"] =
                ObjectINfO.fRotationPerSec;

            // Transform 저장
            Transform* pTransform =
                gameObject->GetTransform();

            if (pTransform != nullptr)
            {
                _float4x4 worldMatrix =
                    pTransform->GetWorldMatrix();

                json matrixJson =
                {
                    {
                        worldMatrix._11,
                        worldMatrix._12,
                        worldMatrix._13,
                        worldMatrix._14
                    },

                    {
                        worldMatrix._21,
                        worldMatrix._22,
                        worldMatrix._23,
                        worldMatrix._24
                    },

                    {
                        worldMatrix._31,
                        worldMatrix._32,
                        worldMatrix._33,
                        worldMatrix._34
                    },

                    {
                        worldMatrix._41,
                        worldMatrix._42,
                        worldMatrix._43,
                        worldMatrix._44
                    }
                };

                objectJson["WorldMatrix"] = matrixJson;
            }
            // Prototype 저장
            auto& components =
                gameObject->GetComponents();

            json componentArray =
                json::array();

            for (auto& [componentTag, componentPtr] : components)
            {
                if (componentPtr == nullptr)
                    continue;

                json componentJson;

                componentJson["ComponentTag"] =
                    string(
                        componentTag.begin(),
                        componentTag.end());

                componentArray.push_back(componentJson);
            }

            objectJson["Components"] =
                componentArray;
         
            // Component 저장
            

            j["GameObjects"].push_back(objectJson);

            // ----------------------------
        }
    }
    // -----------------------------------

    std::ofstream file(filePath);

    if (!file.is_open())
        return E_FAIL;

    file << j.dump(4);

    file.close();
    
    // 메모리 json overwrite
    m_mapJsonFiles[_mapDataName] = j;

    // 새 파일일 때만 이름 추가
    if (!bAlreadyExist)
    {
        m_MapNames.emplace_back(_mapDataName);
    }

    return S_OK;
}
HRESULT Map_Manager::Load(string _mapDataName, uint32_t Levelindex)
{
    using json = nlohmann::json;

    // =====================================================
    // 저장된 json 찾기
    // =====================================================

    auto iter = m_mapJsonFiles.find(_mapDataName);

    if (iter == m_mapJsonFiles.end())
        return E_FAIL;

    json& j = iter->second;

    // =====================================================
    // 기존 맵 제거
    // =====================================================


    CGameInstance::Get().Clear_Resource_SameLevel(Levelindex);

  

    // =====================================================
    // 저장된 오브젝트 생성
    // =====================================================

    for (auto& objectJson : j["GameObjects"])
    {
        // -------------------------------------------------
        // string -> wstring
        // -------------------------------------------------

        string layerStr =
            objectJson["Layer"];

        string prototypeStr =
            objectJson["Prototype_Object_Name"];

        string prototypeBaseStr =
            objectJson["Prototype_Base_Name"];


        string objectNameStr =
            objectJson["Object_Name"];

        wstring layerName(
            layerStr.begin(),
            layerStr.end());

        wstring prototypeName(
            prototypeStr.begin(),
            prototypeStr.end());

        wstring prototypeBaseName(
            prototypeBaseStr.begin(),
            prototypeBaseStr.end());

        wstring objectName(
            objectNameStr.begin(),
            objectNameStr.end());

        // -------------------------------------------------
        // Desc 복원
        // -------------------------------------------------

        GameObject::GAMEOBJECT_DESC desc{};
        desc.ObjectType = objectJson["ObjectType"];
        desc.LevelIndex = objectJson["LevelIndex"];
        desc.m_strName = objectName;
        desc.m_bCollider = objectJson["bCollider"];
        desc.m_strPrototypeObjectName = prototypeName;
        desc.m_strPrototypeBaseName = prototypeBaseName;


        desc.pCameraType =
            objectJson["CameraType"];

        desc.fSpeedPerSec =
            objectJson["fSpeedPerSec"];

        desc.fRotationPerSec =
            objectJson["fRotationPerSec"];


        // -------------------------------------------------
        // ObjectType 확인
        // -------------------------------------------------
        switch (desc.ObjectType) {
        case ETOUI(OBJECTTYPE::OBJECT_CAMERA):
        {
            if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(
                Levelindex,
                prototypeName,
                Levelindex,
                layerName,
                &desc)))
            {
                MSG_BOX("Map Load FAILED : GameObject");
            }

            auto gameObject =
                CGameInstance::Get().Find_Object(
                    Levelindex,
                    layerName,
                    objectName);

            if (gameObject == nullptr)
                MSG_BOX("Map Load FAILED : GameObject");


            // -------------------------------------------------
            // Matrix 복원
            // -------------------------------------------------

            auto& m =
                objectJson["WorldMatrix"];

            _float4x4 mat{};

            mat._11 = m[0][0];
            mat._12 = m[0][1];
            mat._13 = m[0][2];
            mat._14 = m[0][3];

            mat._21 = m[1][0];
            mat._22 = m[1][1];
            mat._23 = m[1][2];
            mat._24 = m[1][3];

            mat._31 = m[2][0];
            mat._32 = m[2][1];
            mat._33 = m[2][2];
            mat._34 = m[2][3];

            mat._41 = m[3][0];
            mat._42 = m[3][1];
            mat._43 = m[3][2];
            mat._44 = m[3][3];

            gameObject->GetTransform()->Set_WorldMatrix(mat);
            if (desc.pCameraType == 0)
            {
                CGameInstance::Get().Add_Camera(
                    desc.pCameraType,
                    dynamic_pointer_cast<Camera>(gameObject));


            }

        }

            break;

        case ETOUI(OBJECTTYPE::OBJECT_MONSTER):
        {
            if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(
                Levelindex,
                prototypeName,
                Levelindex,
                layerName,
                &desc)))
            {
                MSG_BOX("Map Load FAILED : GameObject");
            }

            auto gameObject =
                CGameInstance::Get().Find_Object(
                    Levelindex,
                    layerName,
                    objectName);

            if (gameObject == nullptr)
                MSG_BOX("Map Load FAILED : GameObject");


            // -------------------------------------------------
            // Matrix 복원
            // -------------------------------------------------

            auto& m =
                objectJson["WorldMatrix"];

            _float4x4 mat{};

            mat._11 = m[0][0];
            mat._12 = m[0][1];
            mat._13 = m[0][2];
            mat._14 = m[0][3];

            mat._21 = m[1][0];
            mat._22 = m[1][1];
            mat._23 = m[1][2];
            mat._24 = m[1][3];

            mat._31 = m[2][0];
            mat._32 = m[2][1];
            mat._33 = m[2][2];
            mat._34 = m[2][3];

            mat._41 = m[3][0];
            mat._42 = m[3][1];
            mat._43 = m[3][2];
            mat._44 = m[3][3];

            gameObject->GetTransform()->Set_WorldMatrix(mat);
        }
           
            break;
        case ETOUI(OBJECTTYPE::OBJECT_TERRIAN):
        {
            if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(
                Levelindex,
                prototypeName,
                Levelindex,
                layerName,
                &desc)))
            {
                MSG_BOX("Map Load FAILED : GameObject");
            }

            auto gameObject =
                CGameInstance::Get().Find_Object(
                    Levelindex,
                    layerName,
                    objectName);

            if (gameObject == nullptr)
                MSG_BOX("Map Load FAILED : GameObject");


            // -------------------------------------------------
            // Matrix 복원
            // -------------------------------------------------

            auto& m =
                objectJson["WorldMatrix"];

            _float4x4 mat{};

            mat._11 = m[0][0];
            mat._12 = m[0][1];
            mat._13 = m[0][2];
            mat._14 = m[0][3];

            mat._21 = m[1][0];
            mat._22 = m[1][1];
            mat._23 = m[1][2];
            mat._24 = m[1][3];

            mat._31 = m[2][0];
            mat._32 = m[2][1];
            mat._33 = m[2][2];
            mat._34 = m[2][3];

            mat._41 = m[3][0];
            mat._42 = m[3][1];
            mat._43 = m[3][2];
            mat._44 = m[3][3];

            gameObject->GetTransform()->Set_WorldMatrix(mat);
        }

        break;
        case ETOUI(OBJECTTYPE::OBJECT_PLAYER):


            break;

        case ETOUI(OBJECTTYPE::OBJECT_STATIC):

        {
            if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(
                Levelindex,
                prototypeName,
                Levelindex,
                layerName,
                &desc)))
            {
                MSG_BOX("Map Load FAILED : GameObject");
            }

            auto gameObject =
                CGameInstance::Get().Find_Object(
                    Levelindex,
                    layerName,
                    objectName);

            if (gameObject == nullptr)
                MSG_BOX("Map Load FAILED : GameObject");


            // -------------------------------------------------
            // Matrix 복원
            // -------------------------------------------------

            auto& m =
                objectJson["WorldMatrix"];

            _float4x4 mat{};

            mat._11 = m[0][0];
            mat._12 = m[0][1];
            mat._13 = m[0][2];
            mat._14 = m[0][3];

            mat._21 = m[1][0];
            mat._22 = m[1][1];
            mat._23 = m[1][2];
            mat._24 = m[1][3];

            mat._31 = m[2][0];
            mat._32 = m[2][1];
            mat._33 = m[2][2];
            mat._34 = m[2][3];

            mat._41 = m[3][0];
            mat._42 = m[3][1];
            mat._43 = m[3][2];
            mat._44 = m[3][3];

            gameObject->GetTransform()->Set_WorldMatrix(mat);
        }

            break;
        case ETOUI(OBJECTTYPE::OBJECT_SKYBOX):

        {
            if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(
                Levelindex,
                prototypeName,
                Levelindex,
                layerName,
                &desc)))
            {
                MSG_BOX("Map Load FAILED : GameObject");
            }

            auto gameObject =
                CGameInstance::Get().Find_Object(
                    Levelindex,
                    layerName,
                    objectName);

            if (gameObject == nullptr)
                MSG_BOX("Map Load FAILED : GameObject");


            // -------------------------------------------------
            // Matrix 복원
            // -------------------------------------------------

            auto& m =
                objectJson["WorldMatrix"];

            _float4x4 mat{};

            mat._11 = m[0][0];
            mat._12 = m[0][1];
            mat._13 = m[0][2];
            mat._14 = m[0][3];

            mat._21 = m[1][0];
            mat._22 = m[1][1];
            mat._23 = m[1][2];
            mat._24 = m[1][3];

            mat._31 = m[2][0];
            mat._32 = m[2][1];
            mat._33 = m[2][2];
            mat._34 = m[2][3];

            mat._41 = m[3][0];
            mat._42 = m[3][1];
            mat._43 = m[3][2];
            mat._44 = m[3][3];

            gameObject->GetTransform()->Set_WorldMatrix(mat);
     

        }

            break;
        }

        CGameInstance::Get().Change_Camera(0);
    }

    return S_OK;
}


unique_ptr<Map_Manager> Map_Manager::Create()
{
	auto		pInstance = unique_ptr<Map_Manager>(new Map_Manager());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Map_Manager");
		return nullptr;
	}

	return pInstance;
}



string Map_Manager::WStringToString(const wstring& wstr)
{
    if (wstr.empty())
        return "";

    int sizeNeeded = WideCharToMultiByte(
        CP_UTF8,
        0,
        &wstr[0],
        (int)wstr.size(),
        nullptr,
        0,
        nullptr,
        nullptr);

    string str(sizeNeeded, 0);

    WideCharToMultiByte(
        CP_UTF8,
        0,
        &wstr[0],
        (int)wstr.size(),
        &str[0],
        sizeNeeded,
        nullptr,
        nullptr);

    return str;
}

wstring Map_Manager::StringToWString(const string& str)
{
    if (str.empty())
        return L"";

    int sizeNeeded = MultiByteToWideChar(
        CP_UTF8,
        0,
        &str[0],
        (int)str.size(),
        nullptr,
        0);

    wstring wstr(sizeNeeded, 0);

    MultiByteToWideChar(
        CP_UTF8,
        0,
        &str[0],
        (int)str.size(),
        &wstr[0],
        sizeNeeded);

    return wstr;
}