#pragma once

class ImguiMgr
{
   DECLARE_SINGLETON(ImguiMgr)

public:
   ~ImguiMgr();

public:
   HRESULT     Initialize(HWND& hWnd);
   void        Tick();
   HRESULT     Render();

private:
   void        Key_Input();
   void        Create_Ui();
   void        Manapulate_Ui();
   void        Parameter_Ui();


   void        File_Dialog();             // ����Ž����� dds�߰�
   void        Select_Texture();          // �ؽ�ó ����
   void        Decide_Transform_Value();  // ������, ��ġ �� ����
   void        Create_Object();           // 
   

   void        Select_Object();           // ������ ������Ʈ �� ����
   void        Delete_Object();           // ������ ������Ʈ ����
   void        Change_Object_Value();     // ������ ������Ʈ �� ����

   void        Change_ParamDesc();        // ������ ������Ʈ �Ķ���� �� ����
   void        Add_Picking_Zone();

   void        Save_Ui_Desc();            // �Լ� ȣ��� �Ʒ� �� �Լ��� ���� ȣ��
   void        Decide_FilePath();         // ���� ��� �ҷ�����
   void        Load_Ui_Desc();            // 

   void        Record_Two_Point();        // ���� �� ���� �ι��� Ŭ�� ��ġ�� ����





private:
   _bool             m_bRender_Create_Ui     = { true };    // Create tool on/off
   _bool             m_bRender_Manapulate_Ui = { true };    // Manapulate tool on/off
   _bool             m_bRender_Param_Ui      = { true };    // Parameter tool on/off

   wstring           m_strSearchTexture;                    // �˻�
   wstring           m_strKeyTexture;                       // ������ �ؽ�ó Ű��
   _float3           m_vecPos                = {};          // ������ ��ġ
   _float3           m_vecScale              = {};          // �ؽ�ó ũ��
   _uint             m_iTextureCursor        = { 0 };       // list box ���� �ε���

   wstring           m_strName;                             // ������ ������Ʈ �̸�
   list<wstring>     m_GameobjectName;                      // ������ ������Ʈ�� �̸�
   wstring           m_strSelectObjName;                    // ������ ������Ʈ �̸�
   _uint             m_iObjNameCursor        = { 0 };       // list box ���� �ε���

   shared_ptr<GameObject> m_pSampleObj;
   _bool             m_bIsChangeCheck        = { false };   // �� ���� ��� ����
   _int              m_iTypeZmoOper          = { 0 };       // OPERATION �� ���� ���� ��ư
   OPERATION         m_eZmoOper              = { OPERATION::TRANSLATE };

   _int              m_iPass_0               = { 0 };       // �Ķ���� -> �н�
   _float            m_float_0               = { 0.f };     // �Ķ���� -> ���� ��

   _int              m_iPickTypeIndex        = { 0 };       // add picking zone ���� ��ư
   POINT             m_ptPos1                = {};          // ù ��ŷ ���콺 ��ǥ
   POINT             m_ptPos2                = {};          // �ι�° ��ŷ ���콺 ��ǥ
   _bool             m_bIsRecordStart        = { false };   // record ��ư �۵�Ȯ��
   _uint             m_iRecordOrder          = { 0 };       // record Ƚ�� Ȯ��

   wstring           m_strSaveLoadPath;                     // dat ���� ���
   wstring           m_strSaveFileName;

};

