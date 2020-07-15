#include "Items.h"

void Items::Initialize(LPCTSTR Itemsname)
{
	auto items_model = Itemsname;

	Items_ = GraphicsDevice.CreateModelFromFile(Itemsname);
}

void Items::Update()
{

}

void Items::Draw()
{
	Items_->SetScale(1.5);
	Items_->Draw();
}