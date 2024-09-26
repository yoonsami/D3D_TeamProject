#include "pch.h"
#include "UiMessageCreater.h" 

#include "FontRenderer.h"
#include "MeshRenderer.h"
#include "UiMessageController.h"

UiMessageCreater::UiMessageCreater()
{
}

HRESULT UiMessageCreater::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    return S_OK;
}

void UiMessageCreater::Create_Message(shared_ptr<GameObject> pTarget)
{
    auto pScene = CUR_SCENE;
    vector<weak_ptr<GameObject>> addedObj;
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Message_NotHere.dat", addedObj);

    _uint iSize = IDX(addedObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = addedObj[i];
        if (true == pObj.expired())
            continue;

        wstring strName = pObj.lock()->Get_Name();
        if (L"UI_Message_NotHere_Bg" == strName)
        {
            auto pScript = make_shared<UiMessageController>(pTarget, true);
            pObj.lock()->Add_Component(pScript);
            pScript->Init();
        }
        else if (L"UI_Message_NotHere_Font" == strName)
        {
            auto pScript = make_shared<UiMessageController>(pTarget, false);
            pObj.lock()->Add_Component(pScript);
            pScript->Init();
        }
    }
}