/**
* @file MapCourse.cpp
* @brief �}�b�v�N���X�\�[�X�t�@�C��
* @author ��� �O�M(�q���[�}���A�J�f�~�[���Z)
* @date 2019/3/5
*/
#include "Map.h"

bool Map::Initialize(LPCTSTR mapname)
{
	auto map_model = mapname;

	map_stage = GraphicsDevice.CreateModelFromFile(map_model);
	map_stage->SetPosition(0, 0, 0);
	map_stage->SetRotation(0, -90, 0);

	return true;
}
void Map::Update()
{
	
}
void Map::Draw(Vector3 pos)
{
	pos_ = pos;
	map_stage->SetPosition(pos_);
	map_stage->Draw();
}
void Map::Moji_Initialize()
{
	//moji = GraphicsDevice.CreateModelFromText(_T("100M"), _T("���C���I"), 300, 0.1f);
	//Material mate(moji);

	//mate.Emissive = Color(1.0f, 1.0f, 1.0f);
	//moji->SetMaterial(mate);
}

void  Map::Moji_Draw()
{
	//moji->SetScale(50);
	//moji->SetRotation(90, -90, 0);
	//moji->Draw();
}
