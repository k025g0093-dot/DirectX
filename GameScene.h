#pragma once
#include "KamataEngine.h"

class GameScene {

public:
	
	//コンストラクタとデストラクタ
	GameScene();
	~GameScene();

	KamataEngine::Sprite* sprite_ = nullptr;
	KamataEngine::Model* model_ = nullptr;

	//ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	//カメラ
	KamataEngine::Camera camera_;

	//デバック用カメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	

	// 初期化関数
	void Initialize();

	// 更新処理関数
	void Updatta();

	// 描画処理
	void Draw();

private:

	float inputFloat[3] = {0, 0, 0};

	//画像の読み込み
	uint32_t textureHandle_ = 0;

	//サウンドデータ読み込み
	uint32_t soundDataHandle_ = 0;
	//音楽再生ハンドル
	uint32_t voiceHandle_ = 0;

	//プライベートでシーンごとの旋回用の処理関数を追加


};
