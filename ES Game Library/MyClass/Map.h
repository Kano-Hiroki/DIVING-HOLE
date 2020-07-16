/**
* @file MapCourse.cpp
* @brief �}�b�v�N���X�w�b�_�[�t�@�C��
* @author ��� �O�M(�q���[�}���A�J�f�~�[���Z)
* @date 2019/3/5
*/
#pragma once

#include "../ESGLib.h"


/**
* �}�b�v�̏���
*/
class Map
{
public:
	bool Initialize(LPCTSTR filename);
	void Update();
	void Draw(Vector3 pos);
	void Moji_Initialize();
	void Moji_Draw();

	MODEL GetTerrain() { return map_stage; }

private:
	MODEL map_stage;
	Vector3 pos_;
//	MODEL moji;
	MODEL ring; 
	
};
