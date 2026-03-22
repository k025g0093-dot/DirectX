#pragma once
#include "KamataEngine.h"
#include "AABB.h"
#include "DeathParticles.h"
// 角
enum Corner {
	kRightBottom,
	kLeftBottom,
	kRightTop,
	kLeftTop,

	kNumCorner // 要素数
};

class MapChipField;
class Enemy;

class Player {

public:
#pragma region プレイヤーの向きを変えるのに使用するもの

	// プレイヤーの画像の向きを変えるためのイーナム
	enum class LRDirection { kLeft, kRight };
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
	void OnCollsion(const Enemy* enemy);


	KamataEngine::Vector3 GetWorldPodition();
	

	AABB GetAABB();
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
		bool isRoof = false;  // 天井
		bool isFloor = false; // 着地
		bool isWall = false;  // 壁
		KamataEngine::Vector3 velocity_;
	};

	void MapCollsion(CollisionMapInfo& info);

	void CheckCollisionRight(CollisionMapInfo& info);
	void CheckCollisionLeft(CollisionMapInfo& info);
	void CheckCollisionTop(CollisionMapInfo& info);
	void CheckCollisionBottom(CollisionMapInfo& info);

	void CheckedMove(const CollisionMapInfo& info); // 判定を取ってから移動
	void IsHitTop(const CollisionMapInfo& info);    // 天井に当たったか
	void IsHitWall(const CollisionMapInfo& info);
	// 着地常態化の判定関数
	void IsGrounded(const CollisionMapInfo& info);

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHight = 0.8f;

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

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

#pragma region プレイヤーの移動に関するもの

	// 加速度を入れるもの
	static inline const float kAcceleration = 0.1f;

	// 減速率
	static inline const float kAttenuation = 0.1f;

	// 最大加速度の設定
	static inline const float kLimitRunSpeed = 0.3f;

	// 天井に当たった際の速度
	static inline const float kBlank = 0.0f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.01f;

	// 壁接着時の速度減衰率
	static inline const float kAttenuationWall = 0.1f;

#pragma endregion

#pragma region ジャンプに関するもの

	// 重力加速度
	static inline const float kGravityAcceleration = 0.03f;

	// 最大落下速度
	static inline const float kLimitFallSpeed = 0.5f;

	// ジャンプ初速
	static inline const float kJumpAcceleration = 0.6f;

#pragma endregion

	// 旋回時間＜秒＞
	static inline const float kTimeTrun = 0.3f;

	// プライベート関数
	void MovePlayer();
};
