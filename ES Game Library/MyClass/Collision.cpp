#include "Collision.h"

void Collision::Initialize()
{
	count       = 0;
	distance    = 0;
	items_state = 1;
	big_font    = GraphicsDevice.CreateSpriteFont(_T("メイリオ"),35);

	items_state = EXIST;
}

void Collision::Update(Player&player, Items&items)
{
	items_model  = items.Items_model();
	player_model = player.GetPlayer();

	if(items_state == EXIST)
	{
		distance = Vector3_Distance(player_model->GetPosition(), items_model->GetPosition());
		if (distance < 25)
		{
			count += 1;
			items_state = DELETE_;
		}
	}
}

void Collision::Draw(Items&items, Vector3 pos)
{
	items_model = items.Items_model();
	items_pos = pos;

	if (items_state == EXIST)
	{
		items_model->SetPosition(items_pos);
		items_model->Draw();
	}
}

void Collision::font_Draw()
{
	SpriteBatch.DrawString(big_font, Vector2(500 + 2, 0), Color(0, 255, 0), _T("%dコ"), count);
	SpriteBatch.DrawString(big_font, Vector2(0 + 2, 300), Color(0, 255, 0), _T("アイテムまでの距離%d"), (int)distance);
}