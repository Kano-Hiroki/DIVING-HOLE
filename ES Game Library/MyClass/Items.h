#pragma once

#include "../ESGLib.h"
#include "Player.h"

class Items
{
public:
	
	void Initialize(LPCTSTR Itemsname);
	void Update();
	void Draw();
	
	MODEL Items_model() { return Items_; }

private:

	//�A�C�e��//
	MODEL Items_;
	int Items_state;

};
