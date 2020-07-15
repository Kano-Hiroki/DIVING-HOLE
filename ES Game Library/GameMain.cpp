// #include "Extension\DirectX11\DXGraphics11.hpp"
#include "StdAfx.h"
#include "GameMain.h"

/*---私が制作したプログラムです。---*/

/**
 * @brief  ゲームシーンの初期化
 * @return 成功 … true / 失敗 … false
 * @detail
 */
bool GameMain::GameSceneInitialize()
{
	title = GraphicsDevice.CreateSpriteFromFile(_T("UI/Title.png"));

	title_flag     = true;
	main_flag      = false;
	gameover_flag_ = false;

	return true;
}

/**
 * @brief カメラの初期化
 * @return  成功 … true / 失敗 … false
 * @detail
 */
bool GameMain::CameraInitialize()
{
	Viewport view = GraphicsDevice.GetViewport();

	camera_->SetView(Vector3(0.0f, 30.0f, -160.0f), Vector3(0.0f, 0.0f, 0.0f));
	camera_->SetPerspectiveFieldOfView(45.0f, (float)view.Width, (float)view.Height, 1.0f, 10000.0f);
	GraphicsDevice.SetCamera(camera_);

	return true;
}

/**
 * @brief プレイヤー初期化
 * @return  成功 … true / 失敗 … false
 * @detail
 */
bool GameMain::PlayerInitialize() 
{
	//プレイヤー
	player_ = GraphicsDevice.CreateAnimationModelFromFile(_T("3D/Player/player.X"));
	player_position_ = Vector3(0.0f, 0.0f, 0.0f);//プレイヤー座標
	player_->SetPosition(player_position_);
	player_->SetRotation(0.0f, -90.0f, 0.0f);
	player_->SetTrackLoopMode(0, AnimationLoopMode_Once);


	//ゲームオーバーアニメーション
	crash = GraphicsDevice.CreateAnimationModelFromFile(_T("3D/Player/crash.X"));
	crash->SetRotation(0.0f, -90.0f, 0.0f);
	crash->SetTrackLoopMode(0, AnimationLoopMode_Once);

	player_->SetTrackEnable(1, FALSE);

	//スタート地点
	starting_point_ = GraphicsDevice.CreateModelFromFile(_T("3D/仮用/battery/battery.X"));
	point_pos_ = Vector3(0.0f, 0.0f, 0.0f);

	//プレイヤーの状態
	move_ju_fa_gr_state_ = NORMAL;

	fall_time_   = 0;
	jump_time_   = 0;
	gravity_y_   = 0;
	jump_speed_  = 60;
	jump_a_time_ = 0;
	speed_limit_ = 1;
	anim_time_   = 0.0f;
	move_speed_x = 0;
	move_speed_z = 0;

	cam_pos_ = 0;

	//スローモーションモード
	slow_gauge_ = MAX;
	slow_mode_  = false;
	slow_flag_  = false;
	slow_count_ = LOW;
	slow_sound  = SoundDevice.CreateSoundFromFile(_T("SE/heartbeat.wav"));
	slow_sound->Stop();

	//回復
	heal_mode    = 99;
	heal_count   = 0;
	hp_heal_time = 0;

	fall_meters_ = 5000 * 10;
	fall_dist    = 0;
	old_fall     = 0;

	select_bar = GraphicsDevice.CreateSpriteFromFile(_T("UI/bar.png"));
	bar_pos_y  = 390;
	bar_state  = false;

	listener = SoundDevice.GetListener();

	return true;
}
/**
 * @brief UI初期化
 * @return  成功 … true / 失敗 … false
 * @detail
 */
bool GameMain::UiInitialize()
{
	result_ = GraphicsDevice.CreateSpriteFromFile(_T("UI/Result.png"));

	//フォント生成
	medium_font_ = GraphicsDevice.CreateSpriteFont(_T("メイリオ"), 25);
	big_font_    = GraphicsDevice.CreateSpriteFont(_T("メイリオ"), 50);
	start_font   = GraphicsDevice.CreateSpriteFont(_T("Metrophobic"), 100);

	//ゲージ
	gauge_   = 450;
	HP_gauge = 600.0f;

	//スコア読み込み
	reader_rank.Open(_T("txt/rank.txt"));
	for (int i = 0; i < 3; i++) {
		const int score = reader_rank.ReadInt32();
		if (reader_rank.IsEOF())
			break;
		rank_score[i] = score;
	}
	reader_rank.Close();

	rank_score_list1.clear();

	return true;
}

/**
 * @brief 現在の深さを3Dで表示、初期化
 * @return 成功 … true / 失敗 … false
 * @detail
 */
bool GameMain::FallDistInitialize()
{
	next_meters_ = 100 * 10;
	next_meters_model = nullptr;
	SetNextMetersModel(next_meters_);
	next_meters_model->SetPosition(-1500.0f, -5000.0f * 10, -700.0f);

	Material mate(next_meters_model);
	mate.Emissive = Color(0.0f, 0.5f, 1.0f);
	next_meters_model->SetMaterial(mate);
	next_meters_model->SetRotation(90, -90, 0);

	return true;
}

/**
 * @brief  ワールド(3Dの仮想空間)の初期化
 * @return 成功 … true / 失敗 … false
 * @detail
 */
bool GameMain::WorldInitialize()
{
	Light light_;

	light_.Type      = Light_Directional;
	light_.Diffuse   = Color(1.0f, 1.0f, 1.0f);
	light_.Specular  = Color(1.0f, 1.0f, 1.0f);
	light_.Ambient   = Color(1.0f, 1.0f, 1.0f);
	light_.Direction = Vector3(-2.0f, -5.0f, 0.0f);

	GraphicsDevice.SetLight(light_);

	//フォグ
	GraphicsDevice.SetRenderState (Fog_Enable);					//フォグを有効にする
	GraphicsDevice.SetRenderState (FogVertexMode_Linear);		//線形フォグ
	GraphicsDevice.SetRenderState (FogColor(red, green, blue)); //フォグの色
	GraphicsDevice.SetRenderStateF(FogStart, 200.0f);			//フォグの開始点
	GraphicsDevice.SetRenderStateF(FogEnd, 5000.0f);			//フォグの終了点

	//3Dサウンド
	wind_sound_ = SoundDevice.CreateBufferFromFile(_T("SE/DownFoll_mono.wav"), true);
	wind_3D_ = wind_sound_->GetSound3D();
	wind_x   = 0.0f;
	wind_z   = 1.5f;


	//パーティクル
	s_point = GraphicsDevice.CreateModelFromFile(_T("3D/sen.X"));
	s_point->SetScale(0.015f, 0.003f, 0.015f);
	camera_pos = camera_->GetPosition();
	for (int i = 0; i < SPEEDPOINT_MAX; i++)
	{
		mat[i].Emissive = Color  (175, 255, 255);
		s_point_pos[i]  = Vector3(camera_pos.x, camera_pos.y - 10000.0f, camera_pos.z);

		//範囲
		s_point_direction[i] = Vector3(MathHelper_Random(-80, 80) / 1000.0f, 3, MathHelper_Random(-80, 80) / 1000.0f);
		//速度
		s_point_direction[i] = Vector3_Normalize(s_point_direction[i]) * MathHelper_Random(10, 100);
	}
	////-------

	//ぼかし
	blur = GraphicsDevice.CreateEffectFromFile(_T("FX/blur.fx"));

	blur->SetParameter("AddU", (float)(1.0 / 1280) * blur_depth);
	blur->SetParameter("AddV", (float)(1.0 / 720.0) * blur_depth);

	offscreen_ = GraphicsDevice.CreateRenderTarget(1280, 720, PixelFormat_RGBA8888, DepthFormat_Unknown);

	return true;
}

/**
 * @brief  ステージ初期化
 * @return 成功 … true / 失敗 … false
 * @detail
 */
bool GameMain::StageInitialize()
{
	//マップ
	map[0].Initialize(_T("3D/Map/hasira.X"));
	map[1].Initialize(_T("3D/Map/hasira_itanaga.X"));
	map[2].Initialize(_T("3D/Map/hasira_migi_naname.X"));
	map[3].Initialize(_T("3D/Map/hasira_hidari_naname.X"));

	//アイテム
	items_[0] = GraphicsDevice.CreateModelFromFile(_T("3D/item/tubu.X"));
	items_[1] = GraphicsDevice.CreateModelFromFile(_T("3D/item/hurisuku.X"));

	item_scale[0] = 3;//フリスク
	item_scale[1] = 3;//フリスクの箱

	Map_Read();

	return true;
}

/**
 * @brief プレイヤーのアップデート処理
 * @param [in] map マップ(地形判定に使用)
 * @return なし
 */
void GameMain::PlayerUpdate(Map* map)
{
	//マップタイプの読み込み
	for (int i = 0; i < map_type; i++)
	{
		map_model_[i] = map[i].GetTerrain();
	}

	//現在の深さを読み込み、3Dでの表示
	player_position_ = player_->GetPosition();
	for (int i = 1; i < DIST_MAX; i++)
	{
		if (player_position_.y <= (-5000.0f * i * 10) - 250 && next_meters_ <= 100 * i * 10)
		{
			next_meters_ += 100 * 10;
			SetNextMetersModel(next_meters_);
			fall_meters_ += 5000 * 10;
			next_meters_model->SetPosition(-1500.0f, -fall_meters_, -700.0f);

			Material mate(next_meters_model);
			mate.Emissive = Color(0.0f, 0.5f, 1.0f);
			next_meters_model->SetMaterial(mate);
			next_meters_model->SetRotation(90, -90, 0);
		}
	}

	if (!gameover_flag_){ PlayerMove(); }

	//回復モード
	HPGauge(heal_mode);

	//回復(デバッグ用)
	if (key_buffer_.IsPressed(Keys_D1)){ heal_mode = 0; }

	//当たり判定//
	float min_dist = FLT_MAX;

	auto collision = [&](std::list<Vector3> & object_list, int map_index)
	{
		for (auto& hasira_pos : object_list)
		{
			float  dist_ = FLT_MAX;
			map_model_[map_index]->SetPosition(hasira_pos);
			map_model_[map_index]->IntersectRay(player_->GetPosition() + Vector3(0.0f, 0.0f, 0.0f), Vector3_Down, &dist_);
			
			if (min_dist > dist_){ min_dist = dist_; }
		}
	};

	collision(hasira_position_list_,      0);
	collision(hasira2_position_list_,     1);
	collision(ana_position_list_,         2);
	collision(hasira_yoko_position_list_, 3);

	//スローモーションモード
	if (key_buffer_.IsPressed(Keys_LeftShift)
		|| pad_buffer_.IsPressed(GamePad_Button1)
		|| pad_buffer_.IsPressed(GamePad_Button2)
		|| pad_buffer_.IsPressed(GamePad_Button3)
		|| pad_buffer_.IsPressed(GamePad_Button4)
		|| pad_buffer_.IsPressed(GamePad_Button5)
		|| pad_buffer_.IsPressed(GamePad_Button6)
		|| pad_buffer_.IsPressed(GamePad_Button7))
	{
		slow_sound->PlayLooping(); //SE再生
	}

	if (sen_state == 0 && cam_pos_ < 0.0)
	{
		if (key_buffer_.IsReleased(Keys_LeftShift)
			|| pad_buffer_.IsReleased(GamePad_Button1)
			|| pad_buffer_.IsReleased(GamePad_Button2)
			|| pad_buffer_.IsReleased(GamePad_Button3)
			|| pad_buffer_.IsReleased(GamePad_Button4)
			|| pad_buffer_.IsReleased(GamePad_Button5)
			|| pad_buffer_.IsReleased(GamePad_Button6)
			|| pad_buffer_.IsReleased(GamePad_Button7))
		{
			slow_sound->Stop(); //SE停止
			sen_state = 1; 
			sen_time = 0;
			camera_pos = camera_->GetPosition();

			blur_depth = 0.0f;

			blur->SetParameter("AddU", (float)(1.0 / 1280) * 3.0f + blur_depth);
			blur->SetParameter("AddV", (float)(1.0 / 720.0) * 3.0f + blur_depth);

			//強調線の表示
			for (int i = 0; i < SPEEDLINE_MAX; i++)
			{
				mtrl[i].Emissive = Color(MathHelper_Random(128, 255), MathHelper_Random(128, 255), MathHelper_Random(128, 255));
				sen_pos[i] = Vector3(camera_pos.x, camera_pos.y - 10000.0f, camera_pos.z);
				sen_direction[i] = Vector3(MathHelper_Random(-100, 100) / 100.0f, 3, MathHelper_Random(-100, 100) / 100.0f);
				sen_direction[i] = Vector3_Normalize(sen_direction[i]) * 200;
			}
		}
	}

	if (key_state_.IsKeyDown(Keys_LeftShift) && slow_count_ >= LOW ||
		pad_state_.Buttons[0] != 0 && slow_count_ >= LOW ||
		pad_state_.Buttons[1] != 0 && slow_count_ >= LOW ||
		pad_state_.Buttons[2] != 0 && slow_count_ >= LOW ||
		pad_state_.Buttons[3] != 0 && slow_count_ >= LOW ||
		pad_state_.Buttons[4] != 0 && slow_count_ >= LOW ||
		pad_state_.Buttons[5] != 0 && slow_count_ >= LOW ||
		pad_state_.Buttons[6] != 0 && slow_count_ >= LOW)
	{
		slow_flag_ = true; //スローモーションモードON
	}
	else
	{
		slow_sound->Stop(); //SE停止
		slow_flag_ = false; //スローモーションモードOFF
	}

	if (slow_flag_) //スローモーション
	{
		speed_limit_ = 7;

		//カメラ
		     if (cam_pos_ > 0.0f) { cam_pos_ -= 6; }
		else if (cam_pos_ <= 0.0f){ cam_pos_ += 0.01f; }

		//アニメーション
		anim_time_ -= 0.06f;

		if (anim_time_ < 0){ anim_time_ = 0; }

		player_->SetTrackPosition(0, anim_time_);
	}
	else if (!slow_flag_) //スローモーションOFF
	{
		speed_limit_ = 1;
		cam_pos_ += 2;

		anim_time_ += 0.1f;
		if (anim_time_ > 0.8f)
		{
			anim_time_ = 0.8f;
		}
		player_->SetTrackPosition(0, anim_time_);
	}

	SlowGaugeStatus(); //スローモーションゲージ

	//現在のプレイヤーの状態
	if (move_ju_fa_gr_state_ == NORMAL)
	{
		jump_a_time_ = 0;

		if (min_dist >= 2)
		{
			move_ju_fa_gr_state_ = FALL;
		}
	}
	else if (move_ju_fa_gr_state_ != NORMAL)
	{
		if (move_speed_x >= 10.0f) { move_speed_x = 10.0f; }
		if (move_speed_x <= -10.0f){ move_speed_x = -10.0f; }

		if (move_speed_z >= 10.0f) { move_speed_z = 10.0f; }
		if (move_speed_z <= -10.0f){ move_speed_z = -10.0f; }
	}

	if (move_ju_fa_gr_state_ == FALL)
	{
		if (!gameover_flag_){ FreeFall(); }
	}

	if (gameover_flag_ == false)
	{
		old_fall = fall_dist;
		fall_dist = Vector3_Distance(player_->GetPosition(), point_pos_);
	}

	crash->SetPosition(player_->GetPosition());
}

/**
 * @brief プレイヤー3D用描画処理
 */
void GameMain::PlayerDraw()
{
	if (main_flag)
	{
		if (!gameover_flag_)
		{
			player_->Draw(); //プレイヤー描画
		}
		player_->AdvanceTime(0.01);
		next_meters_model->Draw(); //現在の深さを3D描画

		//ゲームオーバー(プレイヤーアニメーション)
		if (gameover_flag_)
		{
			crash->Draw();
			crash->AdvanceTime(0.1);
		}
	}
}

/**
 * @brief プレイヤー2D用描画処理
 */
void GameMain::PlayerUi()
{
	//2D関係の描画

	//到達深度ランキング表示
	auto DrawRankString = [&](int score_index)
	{
		const int STRING_Y = 400.0f + score_index * 100;
		SpriteBatch.DrawString(start_font, Vector2(900.0f, STRING_Y - 2), Color(0.0f, 0.0f, 0.0f), _T("%dm"), rank_score[score_index]);
		SpriteBatch.DrawString(start_font, Vector2(900.0f, STRING_Y + 2), Color(0.0f, 0.0f, 0.0f), _T("%dm"), rank_score[score_index]);
		SpriteBatch.DrawString(start_font, Vector2(900.0f - 2, STRING_Y), Color(0.0f, 0.0f, 0.0f), _T("%dm"), rank_score[score_index]);
		SpriteBatch.DrawString(start_font, Vector2(900.0f + 2, STRING_Y), Color(0.0f, 0.0f, 0.0f), _T("%dm"), rank_score[score_index]);
		SpriteBatch.DrawString(start_font, Vector2(900.0f, STRING_Y), Color(255.0f, 0.0f, 0.0f), _T("%dm"), rank_score[score_index]);
	};

	if (gameover_flag_)
	{
		SpriteBatch.Draw(*result_, Vector3(0.0f, 0.0f, -10000.0f));
		
		//ゲームオーバー時の到達深度
		SpriteBatch.DrawString(start_font, Vector2(900.0f, 300.0f - 2), Color(0.0f, 0.0f, 0.0f), _T("%dm"), (int)fall_dist / 50);
		SpriteBatch.DrawString(start_font, Vector2(900.0f, 300.0f + 2), Color(0.0f, 0.0f, 0.0f), _T("%dm"), (int)fall_dist / 50);
		SpriteBatch.DrawString(start_font, Vector2(900.0f - 2, 300.0f), Color(0.0f, 0.0f, 0.0f), _T("%dm"), (int)fall_dist / 50);
		SpriteBatch.DrawString(start_font, Vector2(900.0f + 2, 300.0f), Color(0.0f, 0.0f, 0.0f), _T("%dm"), (int)fall_dist / 50);
		SpriteBatch.DrawString(start_font, Vector2(900.0f, 300.0f), Color(255.0f, 0.0f, 0.0f), _T("%dm"), (int)fall_dist / 50);

		DrawRankString(0); // 1位
		DrawRankString(1); // 2位
		DrawRankString(2); // 3位

		if (bar_state == true)
		{
			SpriteBatch.Draw(*select_bar, Vector3(60, bar_pos_y, 0));
		}
	}

	if (title_flag == true)
	{
		SpriteBatch.Draw(*title, Vector3(0.0f, 0.0f, -10000.0f));

		//タイトルでのランキング表示
		DrawRankString(0); // 1位
		DrawRankString(1); // 2位
		DrawRankString(2); // 3位
	}
	else if (main_flag)
	{
		//現在の到達深度
		SpriteBatch.DrawString(big_font_, Vector2(620.0f, 0.0f - 2), Color(0.0f, 0.0f, 0.0f), _T("%dm"), (int)fall_dist / 50);
		SpriteBatch.DrawString(big_font_, Vector2(620.0f, 0.0f + 2), Color(0.0f, 0.0f, 0.0f), _T("%dm"), (int)fall_dist / 50);
		SpriteBatch.DrawString(big_font_, Vector2(620.0f - 2, 0.0f), Color(0.0f, 0.0f, 0.0f), _T("%dm"), (int)fall_dist / 50);
		SpriteBatch.DrawString(big_font_, Vector2(620.0f + 2, 0.0f), Color(0.0f, 0.0f, 0.0f), _T("%dm"), (int)fall_dist / 50);
		SpriteBatch.DrawString(big_font_, Vector2(620.0f, 0.0f),   Color(0.0f, 255.0f, 0.0f), _T("%dm"), (int)fall_dist / 50);
	}
}

/**
 * @brief プレイヤーの移動処理
 */
void GameMain::PlayerMove()
{
	float d, u, r, l;
	float min_d, min_u, min_r, min_l;

	//下方向の当たり判定
	min_d = FLT_MAX;

	auto collision_down = [&](std::list<Vector3>& object_list,int map_index)
	{
		for (auto& map_pos : object_list)
		{
			d = -1;
			map_model_[map_index]->SetPosition(map_pos);
			map_model_[map_index]->IntersectRay(player_->GetPosition() + Vector3(0.0f, 0.0f, 0.0f), Vector3_Right, &d);
			if (d >= 0)
			{
				if (min_d > d){ min_d = d; }
			}
		}
	};

	collision_down(hasira_position_list_,      0);
	collision_down(hasira2_position_list_,     1);
	collision_down(ana_position_list_,         2);
	collision_down(hasira_yoko_position_list_, 3);

	//------------------------------------------------------------------------------------

	//上方向の当たり判定
	min_u = FLT_MAX;

	auto collision_up = [&](std::list<Vector3> & object_list, int map_index)
	{
		for (auto& map_pos : object_list)
		{
			u = -1;
			map_model_[map_index]->SetPosition(map_pos);
			map_model_[map_index]->IntersectRay(player_->GetPosition() + Vector3(0.0f, 0.0f, 0.0f), Vector3_Left, &u);
			if (u >= 0)
			{
				if (min_u > u){ min_u = u; }
			}
		}
	};

	collision_up(hasira_position_list_,      0);
	collision_up(hasira2_position_list_,     1);
	collision_up(ana_position_list_,         2);
	collision_up(hasira_yoko_position_list_, 3);

	//-----------------------------------------------------------------------------------

	//右方向の当たり判定
	min_r = FLT_MAX;

	auto collision_right = [&](std::list<Vector3> & object_list, int map_index)
	{
		for (auto& map_pos : object_list)
		{
			r = -1;
			map_model_[map_index]->SetPosition(map_pos);
			map_model_[map_index]->IntersectRay(player_->GetPosition() + Vector3(0.0f, 0.0f, 0.0f), Vector3_Forward, &r);
			if (r >= 0)
			{
				if (min_r > r){ min_r = r; }
			}
		}
	};

	collision_right(hasira_position_list_,      0);
	collision_right(hasira2_position_list_,     1);
	collision_right(ana_position_list_,         2);
	collision_right(hasira_yoko_position_list_, 3);

	//-----------------------------------------------------------------------------------

	//左方向の当たり判定
	min_l = FLT_MAX;

	auto collision_left = [&](std::list<Vector3> & object_list, int map_index)
	{
		for (auto& map_pos : object_list)
		{
			l = -1;
			map_model_[map_index]->SetPosition(map_pos);
			map_model_[map_index]->IntersectRay(player_->GetPosition() + Vector3(0.0f, 0.0f, 0.0f), Vector3_Backward, &l);
			if (l >= 0)
			{
				if (min_l > l){ min_l = l; }
			}
		}
	};

	collision_left(hasira_position_list_,      0);
	collision_left(hasira2_position_list_,     1);
	collision_left(ana_position_list_,         2);
	collision_left(hasira_yoko_position_list_, 3);

	//-----------------------------------------------------------------------------------

	Vector3 pos = player_->GetPosition();

	//水平移動
	if (key_state_.IsKeyDown(Keys_Down) || pad_state_.Y > Axis_Center)  //上方向移動
	{
		if (min_d <= 11)
		{
			GameOver();
		}
		else
		{
			move_speed_z -= 0.5;
			player_->Move(0.0f, 0.0f, move_speed_z);
		}
	}	
	if (key_state_.IsKeyDown(Keys_Up) || pad_state_.Y < Axis_Center)  //下方向移動
	{
		if (min_u <= 11)
		{
			GameOver();
		}
		else
		{
			move_speed_z += 0.5;
			player_->Move(0.0f, 0.0f, move_speed_z);
		}
	}
	if (key_state_.IsKeyDown(Keys_Right) || pad_state_.X > Axis_Center)  //右方向移動
	{
		if (min_r <= 11)
		{
			GameOver();
		}
		else
		{
			move_speed_x += 0.5;
			player_->Move(move_speed_x, 0.0f, 0.0f);

			wind_x += 0.01f;
			if (wind_x > 1.5f)
			{
				wind_x = 1.5f;
			}
		}
	
	}
	else
	{
		WindSound();
	}
	if (key_state_.IsKeyDown(Keys_Left) || pad_state_.X < Axis_Center)  //左方向移動
	{
		if (min_l <= 11)
		{
			GameOver();
		}
		else
		{
			move_speed_x -= 0.5;
			player_->Move(move_speed_x, 0.0f, 0.0f);

			wind_x -= 0.01f;
			if (wind_x < -1.5f)
			{
				wind_x = -1.5f;
			}
		}
	}
	else
	{
		WindSound();
	}

	//左右移動してない場合
	if (key_state_.IsKeyUp(Keys_Right) && key_state_.IsKeyUp(Keys_Left) && pad_state_.X == Axis_Center)
	{
		//右移動していた場合
		if (move_speed_x > 0)
		{
			move_speed_x -= 0.05;
			if (move_speed_x < 0)
			{
				move_speed_x = 0;
			}
		}

		//左移動していた場合
		else if (move_speed_x < 0)
		{
			move_speed_x += 0.05;
			if (move_speed_x > 0)
			{
				move_speed_x = 0;
			}
		}

		//移動へ反映
		player_->Move(move_speed_x, 0.0f, 0.0f);
	}
	//上下移動していない場合
	if (key_state_.IsKeyUp(Keys_Up) && key_state_.IsKeyUp(Keys_Down) && pad_state_.Y == Axis_Center)
	{
		//上移動していた場合
		if (move_speed_z > 0)
		{
			move_speed_z -= 0.5;
			if (move_speed_z <= 0)
			{
				move_speed_z = 0;
			}
		}

		//下移動していた場合
		if (move_speed_z < 0)
		{
			move_speed_z += 0.5;
			if (move_speed_z >= 0)
			{
				move_speed_z = 0;
			}
		}
		player_->Move(0.0f, 0.0f, move_speed_z);
	}

	//3Dサウンドの座標
	wind_3D_->SetPosition(wind_x, 0, 0);
}

/**
 * @brief 自由落下の処理
 */
void GameMain::FreeFall()
{
	fall_time_ = 100;
	fall_time_++;

	float time = fall_time_ / 300.0f;

	gravity_y_ -= 4.9 * (time * time); //重力

	if (gravity_y_ <= -200.0f) //重力最大値
	{
		gravity_y_ = -200.0f;
	}

	player_->Move(0.0f, gravity_y_ / speed_limit_, 0.0f); //落下移動

	//当たり判定//
	fall_min_dist_ = FLT_MAX;

	auto fall_collision = [&](std::list<Vector3> & object_list, int map_index)
	{
		for (auto& map_pos : object_list)
		{
			height_down_ = -1;
			map_model_[map_index]->SetPosition(map_pos);
			map_model_[map_index]->IntersectRay(player_->GetPosition() + Vector3(0.0f, 0.0f, 0.0f), Vector3_Down, &height_down_);
			if (height_down_ >= 0)
			{
				if (fall_min_dist_ > height_down_)
				{
					fall_min_dist_ = height_down_;
				}
			}
		}
	};

	fall_collision(hasira_position_list_,      0);
	fall_collision(hasira2_position_list_,     1);
	fall_collision(ana_position_list_,         2);
	fall_collision(hasira_yoko_position_list_, 3);

	if (fall_min_dist_ <= 150)
	{
		FreeFallEnd();
		GameOver();
	}

	//スピードの違いによるポリゴン抜け防止
	if (fall_time_ > 200.0f)
	{
		if (fall_min_dist_ > 0.0f && fall_min_dist_ < 400.0f)
		{
			FreeFallEnd();
			GameOver();
		}
	}
	else if (fall_time_ > 150.0f)
	{
		if (fall_min_dist_ > 0.0f && fall_min_dist_ < 350.0f)
		{
			FreeFallEnd();
			GameOver();
		}
	}
	else if (fall_time_ >= 100.0f)
	{
		if (fall_min_dist_ > 0.0f && fall_min_dist_ < 300.0f)
		{
			FreeFallEnd();
			GameOver();
		}
	}
	else if (fall_time_ >= 50.0f)
	{
		if (fall_min_dist_ > 0.0f && fall_min_dist_ < 200.0f)
		{
			FreeFallEnd();
			GameOver();
		}
	}
}

/**
 * @brief スローモーションゲージ
 */
void GameMain::SlowGaugeStatus()
{
	if (gauge_ > 450)  //ゲージ最大値
	{
		gauge_ = 450;
	}
	else if (gauge_ <= 0) //ゲージが0になった時
	{
		gauge_      = 0;
		slow_count_ = 0;
	}
	slow_count_++;

	// スローモーション時……ゲージを減らす
	// スローモードでない時…ゲージを回復する
	if (slow_flag_) { gauge_--; }
	else			{ gauge_ += 0.5; }
}

/**
 * @brief HPゲージ
 */
void GameMain::HPGauge(int a)
{
	if (HP_gauge < 0) //HPが0になった時
	{
		HP_gauge = 0;
		GameOver();
	}

	if (HP_gauge >= 600) //HPがMAXまで回復した時、回復初期化を呼び出す(オーバーヒール対策)
	{
		HpReset();
	}
	HP_gauge -= 1;

	//HP回復
	if (a == 99) //何もしない
	{
		;
	}
	else if (a == 0) //回復(大)
	{
		HpHeal();
		if (heal_count >= 150 / 5) //回復量
		{
			HpReset();
		}
	}
	else if (a == 1) //回復(小)
	{
		HpHeal();
		if (heal_count >= 20 / 5)
		{
			HpReset();
		}
	}
}
/**
 * @brief 三人称カメラ
 */
void GameMain::ThirdPersonCamera()
{
	//カメラ位置最大値
	if (cam_pos_ >= 50.0f) { cam_pos_ = 50.0f; }

	if (main_flag) //メインのカメラ位置
	{
		if (player_position_.y <= 10.0)
		{
			camera_->SetLookAt(player_position_ + Vector3(0.001f, 30.0f + cam_pos_, 0.0f), player_position_, Vector3_Up);
		}
	}
	else if (title_flag) //タイトル時のカメラ位置
	{
		camera_->SetLookAt(Vector3_Zero + Vector3(1.0f, 30.0f, 0.0f), Vector3_Zero, Vector3_Up);
	}
}

/**
 * @brief 自由落下が終わった時の処理
 */
void GameMain::FreeFallEnd()
{
	player_->Move(0.0f, 0.0f, 0.0f);
	fall_time_ = 0;
	gravity_y_ = 0;
	move_ju_fa_gr_state_ = NORMAL;
	speed_limit_ = 0;
}

/**
 * @brief HP回復
 */
void GameMain::HpHeal() 
{
	hp_heal_time++;
	if (hp_heal_time >= 1)	//回復速度
	{
		heal_count++;
		hp_heal_time = 0;
		HP_gauge += 5;      //回復(HP)
	}
}

/**
 * @brief  回復初期化
 * @detail もう一度回復出来るように
 */
void GameMain::HpReset()
{
	heal_count = 0;
	heal_mode  = 99;
}
/**
*@brief  3Dサウンドの制御
*/
void GameMain::WindSound()
{
	if (wind_x > 0.0f)
	{
		wind_x -= 0.05f;
		if (wind_x <= 0.0f)
		{
			wind_x = 0.0f;
		}
	}
	else if (wind_x < 0.0f)
	{
		wind_x += 0.05f;
		if (wind_x >= 0.0f)
		{
			wind_x = 0.0f;
		}
	}
}

      ////////////////////////////////////////////////////////
    //------------------------------------------------------//  
  //--------------------------------------------------------//
/*---↓これより下は別のプログラマーが制作したコードです。↓---*/
  //--------------------------------------------------------//    
   //-----------------------------------------------------//    
   ///////////////////////////////////////////////////////

bool GameMain::Initialize()
{
	WindowTitle(_T("DIVINGHOLE"));

	GameTimer.SetFPS(60);
	InputDevice.CreateGamePad(1);

	GameSceneInitialize();
	ShaderInitialize();
	CameraInitialize();
	PlayerInitialize();
	UiInitialize();
	FallDistInitialize();
	WorldInitialize();
	StageInitialize();
	Effect_Initialize();
	Collsion_Initialize();

	return true;
}

/**
 * @brief  シェーダー初期化
 * @return 成功 … true / 失敗 … false
 * @detail
 */
bool GameMain::ShaderInitialize()
{
	red = 225;
	green = 225;
	blue = 255;

	dividing = 0;
	blur_depth = 0.0f;

	sen = GraphicsDevice.CreateModelFromFile(_T("3D/sen.X"));
	sen->SetScale(2.0f, 10.0f, 1.0f);
	sen_state = 0;

	return true;
}

/// <summary>
/// Finalize will be called once per game and is the place to release
/// all resource.
/// </summary>
void GameMain::Finalize()
{
	// TODO: Add your finalization logic here

}

/// <summary>
/// Allows the game to run logic such as updating the world,
/// checking for collisions, gathering input, and playing audio.
/// </summary>
/// <returns>
/// Scene continued value.
/// </returns>
int GameMain::Update()
{
	key_state_  = Keyboard->GetState();
	key_buffer_ = Keyboard->GetBuffer();
	pad_state_  = GamePad(0)->GetState();
	pad_buffer_ = GamePad(0)->GetBuffer();


	if (key_buffer_.IsPressed(Keys_Enter) 
     || pad_buffer_.IsPressed(GamePad_Button8)
     || pad_buffer_.IsPressed(GamePad_Button10)
	 || pad_buffer_.IsPressed(GamePad_Button2))
	{
		title_flag = false;
		main_flag  = true;

		wind_sound_->PlayLooping();
	}

	
	if(main_flag && slow_flag_)
	{
		wind_sound_->Stop();
	}

	if (bar_state)
	{
		if (key_buffer_.IsPressed(Keys_Up) || pad_state_.Y < Axis_Center)
		{
			bar_pos_y = 390;
		}

		if (key_buffer_.IsPressed(Keys_Down) || pad_state_.Y > Axis_Center)
		{
			bar_pos_y = 600;
		}

		if (bar_pos_y == 600 &&
			(pad_buffer_.IsPressed(GamePad_Button1)    
			 || pad_buffer_.IsPressed(GamePad_Button2) 
		     || pad_buffer_.IsPressed(GamePad_Button3) 
			 || pad_buffer_.IsPressed(GamePad_Button4) 
			 || key_buffer_.IsPressed(Keys_Enter)))
		{
			Initialize();
			bar_state = false;
			title_flag = true;

		}
		else if (bar_pos_y == 390 &&
			(pad_buffer_.IsPressed(GamePad_Button1) ||
				pad_buffer_.IsPressed(GamePad_Button2) ||
				pad_buffer_.IsPressed(GamePad_Button3) ||
				pad_buffer_.IsPressed(GamePad_Button4) ||
				key_buffer_.IsPressed(Keys_Enter)))
		{
			Initialize();
			bar_state = false;
			gameover_flag_ = false;
			main_flag = true;
			title_flag = false;
		}

	}

	if (key_buffer_.IsReleased(Keys_LeftShift)
		|| pad_buffer_.IsReleased(GamePad_Button1)|| pad_buffer_.IsReleased(GamePad_Button2)
		|| pad_buffer_.IsReleased(GamePad_Button3)|| pad_buffer_.IsReleased(GamePad_Button4)
		|| pad_buffer_.IsReleased(GamePad_Button5)|| pad_buffer_.IsReleased(GamePad_Button6)
		|| pad_buffer_.IsReleased(GamePad_Button7))
	{
		wind_sound_->PlayLooping();
	}

	Map_Copy();

	GraphicsDevice.SetRenderState(FogColor(red, green, blue)); //フォグの色

	if (main_flag)
	{

		const int  FALL_DIST = fall_dist / 50;
		const int  OLD_FALL = old_fall / 50;

		if (FALL_DIST >= 3000 + dividing && FALL_DIST < 5000 + dividing)
		{
			red -= 0.0001f;
			blue -= 0.0001f;
			if (red <= 200)
			{
				red = 200;
			}
			if (blue <= 225)
			{
				blue = 225;
			}
		}

		if (FALL_DIST >= 5000 + dividing && FALL_DIST < 7000 + dividing)
		{
			blue -= 0.0001f;
			if (blue <= 150)
			{
				blue = 150;
			}
		}

		if (FALL_DIST >= 7000 + dividing && FALL_DIST < 9000 + dividing)
		{
			green -= 5.0f;
			blue -= 1.0f;
			red += 0.0001f;
			if (green <= 120)
			{
				green = 120;
			}
			if (blue <= 120)
			{
				blue = 120;
			}
			if (red >= 255)
			{
				red = 255;
			}
		}

		if (FALL_DIST >= 9000 + dividing && FALL_DIST < 10000)
		{
			red -= 1.0f;
			blue += 1.0f;
			green += 1.0f;
			if (red <= 225)
			{
				red = 225;
			}
			if (blue >= 255)
			{
				blue = 255;
			}
			if (green >= 225)
			{
				green = 225;
			}
		}

		if (FALL_DIST / 10000 != OLD_FALL / 10000)
		{
			dividing += 10000;
		}

		
		//パーティクル
		camera_pos = camera_->GetPosition();
		for (int i = 0; i < SPEEDPOINT_MAX; i++)
		{
			s_point_pos[i] += s_point_direction[i];
			if (s_point_pos[i].y > camera_pos.y + 1.0f) {
				mat[i].Emissive = Color(175, 255, 255);
				s_point_pos[i] = Vector3(camera_pos.x, camera_pos.y - 10000.0f, camera_pos.z);

				//範囲
				s_point_direction[i] = Vector3(MathHelper_Random(-80, 80) / 1000.0f, 3, MathHelper_Random(-80, 80) / 1000.0f);
				//速度
				s_point_direction[i] = Vector3_Normalize(s_point_direction[i]) * MathHelper_Random(10, 100);
			}
		}

		PlayerUpdate(map);
		Collsion_Update();

		if (sen_state == 1)
		{
			camera_pos = camera_->GetPosition();
			for (int i = 0; i < SPEEDLINE_MAX; i++)
			{
				sen_pos[i] += sen_direction[i];
				if (sen_pos[i].y > camera_pos.y + 100.0f) {
					mtrl[i].Emissive = Color(MathHelper_Random(128, 255), MathHelper_Random(128, 255), MathHelper_Random(128, 255));
					sen_pos[i] = Vector3(camera_pos.x, camera_pos.y - 10000.0f, camera_pos.z);sen_direction[i] = Vector3(MathHelper_Random(-100, 100) / 100.0f, 3, MathHelper_Random(-100, 100) / 100.0f);
					sen_direction[i] = Vector3_Normalize(sen_direction[i]) * 200;
				}
			}

			//ぼかし
			blur_depth += 0.1f;
			if (blur_depth >= 3.0)
			{
				blur_depth = 3.0f;
			}

			blur->SetParameter("AddU", (float)(1.0 / 1280)* (3.0f - blur_depth));
			blur->SetParameter("AddV", (float)(1.0 / 720.0)* (3.0f - blur_depth));

			sen_time++;
			if (sen_time >= 25)
			{
				sen_state = 0;
			}
		}
	}
	
	
	ThirdPersonCamera();
	GraphicsDevice.SetCamera(camera_);

	return 0;
}
/// <summary>
/// This is called when the game should draw itself.
/// </summary>
void GameMain::Draw()
{
	GraphicsDevice.Clear(Color_GhostWhite);

	GraphicsDevice.BeginScene();

	GraphicsDevice.SetRenderTarget(offscreen_);

	GraphicsDevice.Clear(Color_GhostWhite);

	if (main_flag)
	{
		HP_Draw();
		Gage_Draw();
	}
	GraphicsDevice.SetRenderState(Fog_Enable); //フォグを有効にする


	for (auto& hasira_pos : hasira_position_list_)
	{
		map[0].Draw(hasira_pos);
	}

	for (auto& hasira2_pos : hasira2_position_list_)
	{
		map[1].Draw(hasira2_pos);
	}

	for (auto& ana_pos : ana_position_list_)
	{
		map[2].Draw(ana_pos);
	}

	for (auto& hasira_yoko_pos : hasira_yoko_position_list_)
	{
		map[3].Draw(hasira_yoko_pos);
	}

	auto it_pos2 = tubu_position_list_.begin();
	auto it_rot2 = tubu_rotation_x_list.begin();
	while (true)
	{
		if (it_pos2 == tubu_position_list_.end() ||
			it_rot2 == tubu_rotation_x_list.end())
			break;
		items_[0]->SetPosition(*it_pos2);
		items_[0]->SetRotation(*it_rot2, 0.0f, 0.0f);
		items_[0]->SetScale(item_scale[0]);
		items_[0]->Draw();

		*it_rot2 += 10.0f;
		if (*it_rot2 >= 360.0f)
			* it_rot2 -= 360.0f;

		it_pos2++;
		it_rot2++;
	}

	auto it_pos = furisuku_position_list_.begin();
	auto it_rot = furisuku_rotation_x_list.begin();
	while (true) {
		if (it_pos == furisuku_position_list_.end() ||
			it_rot == furisuku_rotation_x_list.end())
			break;
		items_[1]->SetPosition(*it_pos);
		items_[1]->SetRotation(*it_rot, 0.0f, 0.0f);
		items_[1]->SetScale(item_scale[1]);
		items_[1]->Draw();

		*it_rot += 10.0f;
		if (*it_rot >= 360.0f)
			* it_rot -= 360.0f;

		it_pos++;
		it_rot++;
	}

	PlayerDraw();

	GraphicsDevice.SetRenderState(Fog_Disable); //フォグを無効にする

	if (sen_state == 1)
	{
		GraphicsDevice.SetRenderState(CullMode_None);
		for (int i = 0; i < SPEEDLINE_MAX; i++)
		{
			sen->SetPosition(sen_pos[i]);
			sen->SetMaterial(mtrl[i]);
			sen->Draw();
		}
		GraphicsDevice.SetRenderState(CullMode_CullCounterClockwiseFace);
	}

	//パーティクル
	GraphicsDevice.SetRenderState(CullMode_None);
	if (!slow_flag_)
	{
		for (int i = 0; i < SPEEDPOINT_MAX; i++)
		{
			s_point->SetPosition(s_point_pos[i]);
			s_point->SetMaterial(mat[i]);
			s_point->Draw();
		}
	}
	GraphicsDevice.SetRenderState(CullMode_CullCounterClockwiseFace);


	SpriteBatch.Begin();
	
	if (!gameover_flag_)
	{
		Effect_Draw();
	}
	
	PlayerUi();

	SpriteBatch.End();

	GraphicsDevice.RenderTargetToBackBuffer(offscreen_, blur);

	GraphicsDevice.EndScene();
}
void GameMain::Map_Read()
{
	hasira_position_list_.clear();
	for (int i = 0; i < 2; i++) 
	{
		Map_Create();
	}
	for (auto& read_y : file_read_y)
	{
		read_y = 0;
	}
}
void GameMain::Map_Create()
{
	float   most_y = FLT_MAX;

	auto reader_position = [&](std::list<Vector3> & position_list)
	{
		map_pos.x = reader_map.ReadFloat();
		map_pos.y = reader_map.ReadFloat() + file_read_y[1];
		map_pos.z = reader_map.ReadFloat();

		if (most_y > map_pos.y)
			most_y = map_pos.y;
	};

	reader_randam = MathHelper_Random(1, 5);

	switch (reader_randam)
	{
	case 1:
		reader_map.Open(_T("txt/Stage5.txt"));
		break;
	case 2:
		reader_map.Open(_T("txt/Stage6.txt"));
		break;
	case 3:
		reader_map.Open(_T("txt/Stage7.txt"));
		break;
	case 4:
		reader_map.Open(_T("txt/Stage8.txt"));
		break;
	case 5:
		reader_map.Open(_T("txt/Stage9.txt"));
		break;
	}

	while (true)
	{
		int reader_type = reader_map.ReadChar();
		if (reader_map.IsEOF())
			break;
		if (reader_type == 't')
		{
			reader_position(tubu_position_list_);
			tubu_position_list_.push_back(map_pos);
			tubu_rotation_x_list.push_back(0.0f);
		}
		else if (reader_type == 'f')
		{
			reader_position(furisuku_position_list_);
			furisuku_position_list_.push_back(map_pos);
			furisuku_rotation_x_list.push_back(0.0f);
		}
		else if (reader_type == 'h')
		{
			reader_position(hasira_position_list_);
			hasira_position_list_.push_back(map_pos);
		}
		else if (reader_type == 'i')
		{
			reader_position(hasira2_position_list_);
			hasira2_position_list_.push_back(map_pos);
		}
		else if (reader_type == 'l')
		{
			reader_position(ana_position_list_);
			ana_position_list_.push_back(map_pos);
		}
		else if (reader_type == 'r')
		{
			reader_position(hasira_yoko_position_list_);
			hasira_yoko_position_list_.push_back(map_pos);
		}

	}
	reader_map.Close();

	file_read_y[0] = file_read_y[1];
	file_read_y[1] = most_y;
}
void GameMain::Map_Copy()
{
	player_pos = player_->GetPosition();

	auto Re = [&](std::list<Vector3> & position_list)
	{
		auto& it = position_list.begin();
		while (it != position_list.end())
		{
			if (it->y > player_pos.y + 2000)
			{
				it = position_list.erase(it);
			}
			else {
				it++;
			}
		}
	};

	Re(tubu_position_list_);
	Re(furisuku_position_list_);
	Re(hasira_position_list_);
	Re(hasira2_position_list_);
	Re(ana_position_list_);
	Re(hasira_yoko_position_list_);

	if (file_read_y[0] >= player_pos.y)
	{
		Map_Create();
	}

}
void GameMain::Gage_Draw()
{
// 同じ値を設定
for (auto& gage : gauge_vertex) {
	gage.z = 0.0f;
	gage.color = Color(255, 40, 0);
}

for (auto& gage2 : gauge_vertex2) {
	gage2.z = 0.0f;
	gage2.color = Color(255, 80, 0);
}

for (auto& gage3 : gauge_vertex3) {
	gage3.z = 0.0f;
	gage3.color = Color(255, 120, 0);
}

Vector3   naname = Vector3_Transform(Vector3_Down, Matrix_CreateRotationZ(60.0f));//角度

if (gauge_ > 0)
{
	int  gauge_len = gauge_;
	if (gauge_len > 100)
	{
		gauge_len = 100;
	}

	gauge_vertex[0].x = -32.0f;
	gauge_vertex[0].y = 100.0f;

	gauge_vertex[1] = gauge_vertex[0];
	gauge_vertex[1].x += naname.x * 32.0f;
	gauge_vertex[1].y += naname.y * 32.0f;

	gauge_vertex[2].x = gauge_vertex[0].x + 100 - (100 - gauge_len);
	gauge_vertex[2].y = gauge_vertex[0].y;

	gauge_vertex[3] = gauge_vertex[1];

	gauge_vertex[4].x = gauge_vertex[3].x + 100 - (100 - gauge_len);
	gauge_vertex[4].y = gauge_vertex[3].y;

	gauge_vertex[5] = gauge_vertex[2];
	GraphicsDevice.DrawUserPrimitives(PrimitiveType_TriangleList, gauge_vertex, 2, ScreenVertex::FVF());
}

if (gauge_ > 100)
{
	int  gage_len2 = gauge_;
	if (gage_len2 > 250)
	{
		gage_len2 = 250;
	}

	gauge_vertex2[0] = gauge_vertex[5];

	gauge_vertex2[1] = gauge_vertex2[0];
	gauge_vertex2[1].x += naname.x * 48.0f;
	gauge_vertex2[1].y += naname.y * 48.0f;

	gauge_vertex2[2].x = gauge_vertex2[0].x + 150 - (250 - gage_len2);
	gauge_vertex2[2].y = gauge_vertex[0].y;

	gauge_vertex2[3] = gauge_vertex2[1];

	gauge_vertex2[4].x = gauge_vertex2[3].x + 150 - (250 - gage_len2);
	gauge_vertex2[4].y = gauge_vertex2[3].y;

	gauge_vertex2[5] = gauge_vertex2[2];

	GraphicsDevice.DrawUserPrimitives(PrimitiveType_TriangleList, gauge_vertex2, 2, ScreenVertex::FVF());
}

if (gauge_ > 250)
{
	int  gage_len3 = gauge_;
	if (gage_len3 > 450)
	{
		gage_len3 = 450;
	}
	gauge_vertex3[0] = gauge_vertex2[5];

	gauge_vertex3[1] = gauge_vertex3[0];
	gauge_vertex3[1].x += naname.x * 64.0f;
	gauge_vertex3[1].y += naname.y * 64.0f;

	gauge_vertex3[2].x = gauge_vertex3[0].x + 200 - (450 - gage_len3);
	gauge_vertex3[2].y = gauge_vertex[0].y;

	gauge_vertex3[3] = gauge_vertex3[1];

	gauge_vertex3[4].x = gauge_vertex3[3].x + 200 - (450 - gage_len3);
	gauge_vertex3[4].y = gauge_vertex3[3].y;

	gauge_vertex3[5] = gauge_vertex3[2];

	GraphicsDevice.DrawUserPrimitives(PrimitiveType_TriangleList, gauge_vertex3, 2, ScreenVertex::FVF());
}

}
void GameMain::HP_Draw()
{
	// 同じ値を設定
	for (auto& hp_gage : HP_vertex) {
		hp_gage.z = 0.0f;
		hp_gage.color = Color(0, 230, 140);
	}

	for (auto& hp_gage2 : HP_vertex2) {
		hp_gage2.z = 0.0f;
		hp_gage2.color = Color(0, 230, 140);
	}

	for (auto& hp_gage3 : HP_vertex3) {
		hp_gage3.z = 0.0f;
		hp_gage3.color = Color(0, 230, 140);
	}

	Vector3   hp_naname = Vector3_Transform(Vector3_Down, Matrix_CreateRotationZ(60.0f));//角度

	if (HP_gauge > 0)
	{
		int  hp_len = HP_gauge;
		if (hp_len > 200)
		{
			hp_len = 200;
		}

		HP_vertex[0].x = 0.0f;
		HP_vertex[0].y = 85.0f;

		HP_vertex[1].x = HP_vertex[0].x;
		HP_vertex[1].y = 55.0f;

		HP_vertex[2].x = HP_vertex[1].x + 200 - (200 - hp_len);
		HP_vertex[2].y = HP_vertex[1].y;

		HP_vertex[3] = HP_vertex[0];

		HP_vertex[4] = HP_vertex[2];

		HP_vertex[5].x = HP_vertex[3].x + 200 - (200 - hp_len);
		HP_vertex[5].y = HP_vertex[3].y;

		GraphicsDevice.DrawUserPrimitives(PrimitiveType_TriangleList, HP_vertex, 2, ScreenVertex::FVF());
	}

	if (HP_gauge > 200)
	{
		int  hp_len2 = HP_gauge;
		if (hp_len2 > 400)
		{
			hp_len2 = 400;
		}

		HP_vertex2[0] = HP_vertex[5];

		HP_vertex2[1] = HP_vertex[4];

		HP_vertex2[2].x = HP_vertex2[1].x + 200 - (400 - hp_len2);
		HP_vertex2[2].y = HP_vertex2[1].y;

		HP_vertex2[3] = HP_vertex2[0];

		HP_vertex2[4] = HP_vertex2[2];

		HP_vertex2[5].x = HP_vertex2[3].x + 200 - (400 - hp_len2);
		HP_vertex2[5].y = HP_vertex2[3].y;

		GraphicsDevice.DrawUserPrimitives(PrimitiveType_TriangleList, HP_vertex2, 2, ScreenVertex::FVF());
	}

	if (HP_gauge > 400)
	{
		int  hp_len3 = HP_gauge;
		if (hp_len3 > 600)
		{
			hp_len3 = 600;
		}
		HP_vertex3[0] = HP_vertex2[5];

		HP_vertex3[1] = HP_vertex2[4];

		HP_vertex3[2].x = HP_vertex3[1].x + 200 - (600 - hp_len3);
		HP_vertex3[2].y = HP_vertex3[1].y;

		HP_vertex3[3] = HP_vertex3[0];

		HP_vertex3[4] = HP_vertex3[2];

		HP_vertex3[5].x = HP_vertex3[3].x + 200 - (600 - hp_len3);
		HP_vertex3[5].y = HP_vertex3[3].y;

		GraphicsDevice.DrawUserPrimitives(PrimitiveType_TriangleList, HP_vertex3, 2, ScreenVertex::FVF());
	}
}
void GameMain::Effect_Initialize()
{
	TCHAR   file_name[MAX_PATH];
	for (int i = 0; i < EFFCT_MAX; i++)
	{
		::wsprintf(file_name, _T("sloweffect/effect_%05d.png"), i);
		effect[i] = GraphicsDevice.CreateSpriteFromFile(file_name, PixelFormat_DXT5);
	}
}
void GameMain::Effect_Draw()
{
	int pattern = (int)(gauge_ / 20.0f);//エフェクトスピード調整

	if(slow_flag_)
	{
		
		if (pattern >= 10)
		{
			pattern = 10;
		}
		SpriteBatch.Draw(*effect[10 - pattern], Vector3(0.0f, 0.0f, -1000.0f));
	}
	else
	{
		pattern = 0;
	}
}
void GameMain::Collsion_Initialize()
{
	count = 0;
	for (int i = 0; i < item_type; i++)
	{
		distance[i] = 0;
	}
}
void GameMain::Collsion_Update()
{
	auto it_pos2 = tubu_position_list_.begin();
	auto it_rot2 = tubu_rotation_x_list.begin();

	auto player_pos = player_->GetPosition();

	while (true)
	{
		if (it_pos2 == tubu_position_list_.end())
			break;

		distance[0] = Vector3_Distance(player_pos, *it_pos2);
		if (distance[0] >= 90)
		{
			it_pos2++;
		}
		else 
		{
			count++;
			heal_mode = 1;
			it_pos2 = tubu_position_list_.erase(it_pos2);
			it_rot2 = tubu_rotation_x_list.erase(it_rot2);
		}
	}

	auto it_pos = furisuku_position_list_.begin();
	auto it_rot = furisuku_rotation_x_list.begin();

	while (true)
	{
		if (it_pos == furisuku_position_list_.end())
			break;

		distance[1] = Vector3_Distance(player_pos, *it_pos);
		if (distance[1] >= 90)
		{
			it_pos++;
		}
		else
		{
			count++;
			heal_mode = 0;
			it_pos = furisuku_position_list_.erase(it_pos);
			it_rot = furisuku_rotation_x_list.erase(it_rot);
		}
	}
}

void GameMain::SetNextMetersModel(const int next_meters)
{
	GraphicsDevice.ReleaseModel(next_meters_model);

	TCHAR   nextmeters_text[32];
	::wsprintf(nextmeters_text, _T("%dm"), next_meters);

	next_meters_model = GraphicsDevice.CreateModelFromText(nextmeters_text, _T("メイリオ"), 300, 2.5f);
	
	next_meters_model->SetScale(50 * 5);
}
void GameMain::GameOver()
{

	blur->SetParameter("AddU", (float)(1.0 / 1280) * 0.0f);
	blur->SetParameter("AddV", (float)(1.0 / 720.0) * 0.0f);

	if(gameover_flag_)
		return;

	crash->SetTrackEnable(0, TRUE);

	gameover_flag_ = true;
	//死んだらスコアを書き込む//
	rank_score[3] = fall_dist / 50;
	for (int i = 2; i >= 0; i--)
	{
		if (rank_score[i] < rank_score[i + 1]) {
			int w = rank_score[i];
			rank_score[i] = rank_score[i + 1];
			rank_score[i + 1] = w;
		}
		else
		{
			break;
		}
	}
	write_rank.Open(_T("txt/rank.txt"));
	for (int i = 0; i < 3; i++) {
		write_rank.WriteInt32(rank_score[i]);


	write_rank.WriteChar(_T('\n'));
	}
	write_rank.Close();
	//-------------------------------------//
	bar_state = true;
	wind_sound_->Stop();
}
void GameMain::Initialize_ze()
{
	player_position_ = Vector3(0.0f, 0.0f, 0.0f);//プレイヤー座標
	//ゲージ
	gauge_ = 450;
	HP_gauge = 600.0f;
	red = 225;
	green = 225;
	blue = 255;
	
	Collsion_Initialize();
	Map_Read();
	//スコア読み込み
	reader_rank.Open(_T("txt/rank.txt"));
	for (int i = 0; i < 3; i++) {
		const int score = reader_rank.ReadInt32();
		if (reader_rank.IsEOF())
			break;
		rank_score[i] = score;
	}
	reader_rank.Close();

	rank_score_list1.clear();
	move_ju_fa_gr_state_ = NORMAL;

	fall_time_ = 0;
	jump_time_ = 0;
	gravity_y_ = 0;
	jump_speed_ = 60;
	jump_a_time_ = 0;
	speed_limit_ = 1;
	anim_time_ = 0.0f;

	cam_pos_ = 0;

	slow_gauge_ = MAX;
	slow_mode_ = false;
	slow_flag_ = false;

	move_speed_x = 0;
	move_speed_z = 0;

	slow_count_ = LOW;

	heal_mode = 99;
	heal_count = 0;
	hp_heal_time = 0;

	fall_meters_ = 5000 * 10;
	fall_dist = 0;
	old_fall = 0;

	bar_pos_y = 390;
	bar_state = false;
}