/**
* @file MapCourse.cpp
* @brief マップクラスヘッダーファイル
* @author 狩野 弘貴(ヒューマンアカデミー仙台校)
* @date 2019/3/5
*/
#pragma once

#include "../ESGLib.h"


/**
* マップの処理
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
