#pragma once
#include "KamataEngine.h"


//角
enum Corner {
	kRightBottom,
	kLeftBottom,
	kRightTop,
	kLeftTop,

	kNumCorner//要素数
};

class MapChipField;

class Player {

public:
#pragma region プレイヤーの向きを変えるのに使用するもの

	// プレイヤーの画像の向きを変えるためのイーナム
	enum class LRDirection { kRight, kLeft };
	// 初期の向き
	LRDirection lrDirection_ = LRDirection::kRight;
	// プレイヤーの向きの角度を補間
	float turnFistRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;

#pragma endregion

#pragma region プレイヤーのジャンプに関する処理

	// プレイヤーのジャンプに関するもの
	bool onGround_ = true;

#pragma endregion

	// コンストラクタとデストラクタ
	Player();
	~Player();

	// 初期化関数
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新処理
	void Updata();

	void Draw();

	// 慣性系の物
	KamataEngine::Vector3 velocity_ = {};

	// プレイヤーの位置を扱えるように
	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; };

// Player.h の修正
	// ...
	void SetMapChipFiled(MapChipField* mapChipField) { mapChipField_ = mapChipField; } // 修正: 代入するように変更


	// Player.h 53行目付近
	struct CollisionMapInfo {
		bool isRoof = false;//天井
		bool isFloor = false;//着地
		bool isWall = false;//壁
		KamataEngine::Vector3 velocity_; 
	};

	void MapCollsion(CollisionMapInfo& info);

	void CheckCollisionRight(CollisionMapInfo& info);
	void CheckCollisionLeft(CollisionMapInfo& info);
	void CheckCollisionTop(CollisionMapInfo& info);
	void CheckCollisionBottom(CollisionMapInfo& info);

	void CheckedMove(const CollisionMapInfo& info);
	void IsHitTop(const CollisionMapInfo& info);


	//キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHight= 0.8f;

static KamataEngine::Vector3 CornerPositio(const KamataEngine::Vector3& center, Corner corner);

private: // プライベート関数群とかのその他
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	//マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

#pragma region プレイヤーの移動に関するもの

	// 加速度を入れるもの
	static inline const float kAcceleration = 0.05f;

	// 減速率
	static inline const float kAttenuation = 0.1f;

	// 最大加速度の設定
	static inline const float kLimitRunSpeed = 0.1f;

	//天井に当たった際の速度
	static inline const float kBlank = 0.0f;

#pragma endregion


#pragma region	ジャンプに関するもの

	//重力加速度
	static inline const float kGravityAcceleration = 0.03f;

	// 最大落下速度
	static inline const float kLimitFallSpeed = 0.5f;

	// ジャンプ初速
	static inline const float kJumpAcceleration = 0.4f;

#pragma endregion

	// 旋回時間＜秒＞
	static inline const float kTimeTrun = 0.3f;

	//プライベート関数
	void MovePlayer();

};
