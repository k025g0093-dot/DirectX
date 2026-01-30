#pragma once
#include"KamataEngine.h"
class Player {

public:

	// コンストラクタとデストラクタ
	Player();
	~Player();

	// 初期化関数
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera);

	// 更新処理
	void Updata();

	void Draw();

private://プライベート関数群とかのその他

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_;

		// カメラ
	KamataEngine::Camera* camera_=nullptr;
};
