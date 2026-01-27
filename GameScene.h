#pragma once
#include "KamataEngine.h"

class GameScene {

public:
	

	KamataEngine::Sprite* sprite_ = nullptr;

	uint32_t textureHandle_=0;

	/*GameScene();
	~GameScene();*/

	// 初期化関数
	void Initialize();

	// 更新処理関数
	void Updatta();

	// 描画処理
	void Draw();

private:
	//プライベートでシーンごとの旋回用の処理関数を追加


};
