#pragma once
#include "KamataEngine.h"
#include"Player.h"
#include "SkyDome.h"
#include<vector>

class GameScene {

public:
	GameScene();
	~GameScene();

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	//3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* Mapmodel_ = nullptr;//マップのモデル
	KamataEngine::Model* modelSkydome_ = nullptr;//スカイドームのモデル
	KamataEngine::Model* modelPlayer_ = nullptr;//プレイヤーのモデル

	std::vector<std::vector<KamataEngine::WorldTransform*> >worldTransformBlocks_;

	// 初期化関数
	void Initialize();

	// 更新処理関数
	void Updatta();

	// 描画処理
	void Draw();

	Player* player_ = nullptr;
	SkyDome* SkyDome_ = nullptr;
private:
	uint32_t textureHandle_;
	bool isDebugCamera_ = false;
	// プライベートでシーンごとの旋回用の処理関数を追加
};
