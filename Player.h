#pragma once
#include"KamataEngine.h"

class Player {

public:

	//プレイヤーの画像の向きを変えるためのイーナム
	enum class LRDirection {
		kRight,
		kLeft
	};
	//初期の向き
	LRDirection lrDirection_ = LRDirection::kRight;

	//プレイヤーの向きの角度を補間
	float turnFistRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;

	int frameCounter_=0;


	// コンストラクタとデストラクタ
	Player();
	~Player();

	// 初期化関数
	void Initialize(KamataEngine::Model* model, 
		KamataEngine::Camera* camera,
		const KamataEngine::Vector3& position
	);

	// 更新処理
	void Updata();

	void Draw();


	// 慣性系の物
	KamataEngine::Vector3 velocity_ = {};


	//プレイヤーの位置を扱えるように
	/*KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }*/

private://プライベート関数群とかのその他

	
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_;

	// カメラ
	KamataEngine::Camera* camera_=nullptr;

	//加速度を入れるもの
	static inline const float kAcceleration = 0.1f;

	//減速率
	static inline const float kAttenuation = 0.1f;

	//最大足の設定
	static inline const float kLimitRunSpeed = 0.1f;

	//旋回時間＜秒＞
	static inline const float kTimeTrun = 0.3f;

};
