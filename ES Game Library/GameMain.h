#pragma once

#include "ESGLib.h"
#include "GameScene/GameScene.hpp"
//#include "MyClass/Player.h"
#include "MyClass/Map.h"
#include "MyClass/Items.h"

class GameMain : public CGameScene 
{
public:
	GameMain() : DefaultFont(GraphicsDevice.CreateDefaultFont())
	{
//		ContentRootDirectory(_T("Content"));
	}

	virtual ~GameMain()
	{
#ifdef _INC_SQUIRREL
		Squirrel.ReleaseAllScripts();
#endif
#ifdef _INC_NUI
		NUI.ReleaseAllKinects();
#endif
#ifdef _INC_LIVE2D
		Live2D.ReleaseAllModels();
#endif
#ifdef _INC_EFFEKSEER
		Effekseer.ReleaseAllEffects();
#endif
#ifdef _INC_DIRECT2D
		Direct2D.ReleaseAllResources();
#endif
		MediaManager.ReleaseAllMedia();

		SoundDevice.ReleaseAllMusics();
		SoundDevice.ReleaseAllSounds();

		GraphicsDevice.ReleaseAllRenderTargets();
		GraphicsDevice.ReleaseAllStateBlocks();
		GraphicsDevice.ReleaseAllFonts();
		GraphicsDevice.ReleaseAllSprites();
		GraphicsDevice.ReleaseAllAnimationModels();
		GraphicsDevice.ReleaseAllModels();
		GraphicsDevice.ReleaseAllVertexBuffers();
		GraphicsDevice.ReleaseAllEffects();

		Finalize();
	}

public:
	virtual bool Initialize();

	virtual int  Update();
	virtual void Draw();

	bool GameSceneInitialize();
	bool CameraInitialize();
	bool FallDistInitialize();
	bool UiInitialize();
	bool WorldInitialize();
	bool StageInitialize();
	bool ShaderInitialize();

	//プレイヤー//
	bool PlayerInitialize();
	void PlayerUpdate(Map* map);
	void PlayerDraw();
	void PlayerUi();
	void PlayerMove();
	void FreeFall();



	void ThirdPersonCamera();

	void FreeFallEnd();

	void SlowGaugeStatus();
	void HPGauge(int a);

	void WindSound();

	FONT medium_font_;
	FONT big_font_;
	FONT test_font;
	//----------//

private:
	void Finalize();
	FONT DefaultFont;

private:
	// 変数宣言

	Items item;

	std::list<Vector3>hasira_position_list_;
	std::list<Vector3>hasira2_position_list_;
	std::list<Vector3>ana_position_list_;
	std::list<Vector3>hasira_yoko_position_list_;

	Vector3 map_pos;

	int select;

	StreamReader reader_map;//読み込み

	ANIMATIONMODEL player_model;
	Vector3 player_pos;

	Vector3 get_map_pos;

	// 関数宣言
	void Map_Read();
	void Map_Copy();
	void Gage_Draw();
	void HP_Draw();
	void Map_Create();
	void GameOver();
	void Effect_Initialize();
	void Effect_Draw();
	void Collsion_Initialize();
	void Collsion_Update();
	void Initialize_ze();

	//プレイヤー//-----//
	CAMERA camera_;

	ANIMATIONMODEL player_;

	MODEL starting_point_;

	MODEL next_meters_model;
	int   next_meters_;
	void  SetNextMetersModel(const int next_meters);

	GamePadState   pad_state_;
	GamePadBuffer  pad_buffer_;
	KeyboardState  key_state_;
	KeyboardBuffer key_buffer_;

	SPRITE result_;

	Vector3 player_position_;
	Vector3 player_rotation_;
	Vector3 jump_position_;
	Vector3 point_pos_;

	int fall_time_;
	int move_ju_fa_gr_state_;
	int jump_time_;
	int speed_limit_;
	int slow_gauge_;
	int slow_count_;
	//ハイスコア

	float pad_paramater_;
	float gravity_y_;
	float height_;
	float height_down_;
	float jump_speed_;
	float jump_a_time_;
	float move_speed_x;
	float move_speed_z;
	float cam_pos_;
	float fall_min_dist_;
	float anim_time_;
	float fall_dist;
	float fall_meters_;

	bool slow_mode_;
	bool slow_flag_;
	bool gameover_flag_;

	//サウンド//
	SOUND   wind_sound_;
	SOUND3D wind_3D_;

	SOUNDLISTENER listener;

	float wind_x;
	float wind_z;

	SOUND slow_sound;

	//-------------------------------------------------------
	//マップ//
	SPRITE effect[EFFCT_MAX];

	Map   map[map_type];
	MODEL map_model_[map_type];
	float file_read_y[2];
	
	int reader_randam;

	//ゲージ//
	ScreenVertex gauge_vertex[6];//頂点の数
	ScreenVertex gauge_vertex2[6];
	ScreenVertex gauge_vertex3[6];
	float        gauge_;

	//体力ゲージ//
	ScreenVertex   HP_vertex[VERTEX_MAX];
	ScreenVertex   HP_vertex2[VERTEX_MAX];
	ScreenVertex   HP_vertex3[VERTEX_MAX];
	float          HP_gauge;

	//
	void  HpReset();
	int   heal_count;
	int   heal_mode;

	void  HpHeal();
	float hp_heal_time;
	//

	MODEL items_[item_type];
	int   count;
	float distance[item_type];
	float item_scale[item_type];

	std::list<Vector3>tubu_position_list_;
	std::list<float>tubu_rotation_x_list;

	std::list<Vector3>furisuku_position_list_;
	std::list<float>furisuku_rotation_x_list;

	//---------//-----//

	MODEL sen;
	Material mtrl[SPEEDLINE_MAX];
	Vector3 sen_pos[SPEEDLINE_MAX];
	Vector3 sen_direction[SPEEDLINE_MAX];
	Vector3 camera_pos;
	int sen_time;
	int sen_state;

	////パーティクル
	MODEL s_point;
	Material mat[SPEEDPOINT_MAX];
	Vector3 s_point_pos[SPEEDPOINT_MAX];
	Vector3 s_point_direction[SPEEDPOINT_MAX];

	bool title_flag;
	bool main_flag;
	SPRITE title;



	SPRITE select_bar;
	FONT start_font;
	float   bar_pos_y;
	int     bar_state;

	int rank_score[4];//スコア

	StreamWriter write_rank;//ランキング書き込み
	StreamReader reader_rank;//ランキング読み込み

	std::list<int>rank_score_list1;

	//フォグカラー//
	int red,green,blue;
	float old_fall;
	int dividing;


	ANIMATIONMODEL crash;

	//ぼかしシェーダー
	EFFECT blur;
	float blur_depth;
	RENDERTARGET offscreen_;

};