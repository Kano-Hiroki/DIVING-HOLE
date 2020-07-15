#pragma once

#include "../ESGLib.h"
#include "Player.h"
#include "Map.h"
#include "Items.h"

class Collision
{
public:
	
	void Initialize();
	void Update(Player&player, Items&items);
	void Draw(Items&items,Vector3 pos);
	void font_Draw();

	
private:

	FONT big_font;
	////�v���C���[//
	ANIMATIONMODEL player_model;
	
	//�A�C�e��//
	MODEL items_model;
	Vector3 items_pos;

	int items_state;

	int count;

	float distance;


};
