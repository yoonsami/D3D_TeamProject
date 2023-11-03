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


   void        File_Dialog();             // ����Ž����� dds�߰�
   void        Select_Texture();          // �ؽ�ó ����
   void        Decide_Transform_Value();  // ������, ��ġ �� ����
   void        Create_Object();           // 
   

   void        Select_Object();           // ������ ������Ʈ �� ����
   void        Delete_Object();           // ������ ������Ʈ ����
   void        Change_Value_Object();     // ������ ������Ʈ �� ����

   void        Change_ParamDesc();        // ������ ������Ʈ �Ķ���� �� ����
   void        Add_Picking_Zone();

   void        Save_Ui_Desc();            // 
   void        Load_Ui_Desc();            // 





private:
   _bool             m_bRender_Create_Ui     = { false };   // Create tool on/off
   _bool             m_bRender_Manapulate_Ui = { false };   // Manapulate tool on/off

   wstring           m_strSearchTexture;                    // �˻�
   wstring           m_strKeyTexture;                       // ������ �ؽ�ó Ű��
   _float3           m_vecPos                = {};          // ������ ��ġ
   _float2           m_vecScale              = {};          // �ؽ�ó ũ��
   _uint             m_iTextureCursor        = { 0 };       // list box ���� �ε���

   wstring           m_strName;                             // ������ ������Ʈ �̸�
   list<wstring>     m_GameobjectName;                      // ������ ������Ʈ�� �̸�
   wstring           m_strSelectObjName;                    // ������ ������Ʈ �̸�
   _uint             m_iObjNameCursor        = { 0 };       // list box ���� �ε���

   _bool             m_bIsChangeCheck        = { false };   // �� ���� ��� ����

   _int              m_iPass_0               = { 0 };       // 
   _float            m_float_0               = { 0.f };     // 

   _int              m_iPickTypeIndex        = { 0 };       // 
   POINT             m_ptPos1                = {};
   POINT             m_ptPos2                = {};
};

