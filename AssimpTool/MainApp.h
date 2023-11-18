#pragma once

template<typename>
class Geometry;
class MainApp
{
public:
	MainApp();
	virtual ~MainApp();

public:
	HRESULT Init();
	void	Tick();
	HRESULT Render();

private:
	void	ExportAssets();			// MEMO: �⺻���� �� �ε�� Ȱ��ȭ
	void	ExportParts();

	void	ExportWrongAnimRight();
};

