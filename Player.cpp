#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include <algorithm>
#include <cassert>
#include <numbers>

using namespace KamataEngine;
Player::Player() {}
bool isHit = false;
void Player::Initialize(
    KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position

) {

	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 180.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	velocity_ = {0.0f, 0.0f, 0.0f};

	assert(model);
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	camera_ = camera;
}

void Player::Updata() {

#pragma region ビヘイビアの切り替え処理と初期化
	if (behaviorRequest_ != Behavior::kUnknown) {

		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kRoot:

			break;
		case Behavior::kAttack:
			attackCounter_ = 0;
			break;
		default:
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}
#pragma endregion

#pragma region ビヘイビアによる各更新処理

	switch (behavior_) {
	case Behavior::kRoot:
		BehaviorRootUpdate();

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			behaviorRequest_ = Behavior::kAttack;
		}

		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();

		attackCounter_++;

		if (attackCounter_ >= 10) {
			behaviorRequest_ = Behavior::kRoot;
		}

		break;
	default:
		break;
	}
#pragma endregion

#pragma region アフィン行列の作成と行列の更新
	worldTransform_.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
#pragma endregion
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

Player::~Player() {}

#pragma region ビヘイビアに関する処理（リセットと更新）

void Player::BehaviorRootInitialize() {}

void Player::BehaviorAttackInitialize() {}

void Player::BehaviorRootUpdate() {
#pragma region プレイヤーの移動処理

	MovePlayer();

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.velocity_ = velocity_;

	// 1. 衝突判定と押し戻し量の計算
	MapCollsion(collisionMapInfo);

	// 2. 壁衝突後の減衰（MapCollsion の後に呼ぶことで isWall フラグが正しく参照される）
	IsHitWall(collisionMapInfo);

	// 3. 状態判定（座標を動かす前に、計算された移動量を使って判定を済ませる）
	IsHitTop(collisionMapInfo);
	IsGrounded(collisionMapInfo);

	// 4. 最後に一回だけ座標を確定させる
	CheckedMove(collisionMapInfo);
#pragma endregion

#pragma region プレイヤーの旋回処理

	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 20.0f;
		if (turnTimer_ < 0.0f) {
			turnTimer_ = 0.0f;
		}
		float t = 1.0f - turnTimer_;

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		worldTransform_.rotation_.y = turnFistRotationY_ + (destinationRotationY - turnFistRotationY_) * t;
	}

#pragma endregion
}

void Player::BehaviorAttackUpdate() {
#pragma region プレイヤーの攻撃処理

	velocity_.x += kAcceleration;

	velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.velocity_ = velocity_;

	MapCollsion(collisionMapInfo);

	CheckedMove(collisionMapInfo);
#pragma endregion
}

#pragma endregion

void Player::MovePlayer() {

#pragma region プレイヤーの移動処理
	if (onGround_) {

		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			Vector3 acceleration = {};

			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

					turnFistRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 1.0f;
				}

				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x += kAcceleration;
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;

					turnFistRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 1.0f;
				}

				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;
			}
			velocity_.x = acceleration.x;

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_ += Vector3(0, kJumpAcceleration, 0);
		}
	} else {
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
#pragma endregion
}

void Player::CheckedMove(const CollisionMapInfo& info) { worldTransform_.translation_ += info.velocity_; }

#pragma region playerとマップチップの当たり判定

void Player::MapCollsion(CollisionMapInfo& info) {
	CheckCollisionTop(info);
	CheckCollisionBottom(info);
	CheckCollisionRight(info);
	CheckCollisionLeft(info);
}
#pragma endregion

#pragma region 横方向の当たり判定

void Player::CheckCollisionRight(CollisionMapInfo& info) {
	if (info.velocity_.x > 0.0f) {
		std::array<Vector3, kNumCorner> positionsNew;
		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPositio(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
		}

		MapChipField::IndexSet indexSet;
		bool hit = false;
		MapChipField::IndexSet hitIndex{};

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop] + Vector3(0.1f, 0, 0));
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0.1f, 0.5f, 0));
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		if (hit) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(hitIndex.xIndex, hitIndex.yIndex);
			float newVelocityX = (rect.left - worldTransform_.translation_.x) - (kWidth / 2.0f) - 0.01f;
			info.velocity_.x = std::min(0.0f, newVelocityX);
			info.isWall = true;
		}
	}
}

void Player::CheckCollisionLeft(CollisionMapInfo& info) {
	if (info.velocity_.x < 0.0f) {
		std::array<Vector3, kNumCorner> positionsNew;
		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPositio(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
		}

		MapChipField::IndexSet indexSet;
		bool hit = false;
		MapChipField::IndexSet hitIndex{};

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop] + Vector3(-0.1f, 0, 0));
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(-0.1f, 0.5f, 0));
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		if (hit) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(hitIndex.xIndex, hitIndex.yIndex);
			float newVelocityX = (rect.right - worldTransform_.translation_.x) + (kWidth / 2.0f) + 0.01f;
			info.velocity_.x = std::max(0.0f, newVelocityX);
			info.isWall = true;
		}
	}
}

void Player::IsHitWall(const CollisionMapInfo& info) {

	if (info.isWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

#pragma endregion

#pragma region playerとマップチップの上方向判定

void Player::CheckCollisionTop(CollisionMapInfo& info) {
	if (info.velocity_.y > 0.0f) {
		std::array<Vector3, kNumCorner> positionsNew;
		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPositio(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
		}

		MapChipField::IndexSet indexSet;
		bool hit = false;
		MapChipField::IndexSet hitIndex{};

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		if (hit) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(hitIndex.xIndex, hitIndex.yIndex);
			float newVelocityY = (rect.bottom - worldTransform_.translation_.y) - (kHight / 2.0f) - 0.01f;
			info.velocity_.y = std::min(0.0f, newVelocityY);
			info.isCeiling = true; // 修正: isFloor → isCeiling（天井判定のため）
		}
	}
}

// 天井に当たったときに速度を０にする処理
void Player::IsHitTop(const CollisionMapInfo& info) {

	if (info.isCeiling) { // 修正: isFloor → isCeiling
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

#pragma endregion

#pragma region playerとマップチップの地面判定

void Player::CheckCollisionBottom(CollisionMapInfo& info) {
	if (info.velocity_.y > 0.0f)
		return;

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPositio(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
	}

	MapChipField::IndexSet indexSet;
	bool hit = false;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
		hit = true;
	} else {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
		}
	}

	if (hit) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.velocity_.y = (rect.top - worldTransform_.translation_.y) + (kHight / 2.0f) + 0.01f;
		info.isFloor = true; // 修正: isRoof → isFloor（地面判定のため）
	}
}

void Player::IsGrounded(const CollisionMapInfo& info) {

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {

			std::array<Vector3, kNumCorner> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPositio(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
			}

			MapChipType maoChipType;
			bool hit = false;

			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -0.05f, 0));
			maoChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (maoChipType == MapChipType::kBlock) {
				hit = true;
			}

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -0.05f, 0));
			maoChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (maoChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				onGround_ = false;
			}
		}

	} else {

		if (info.isFloor) { // 修正: isRoof → isFloor
			DebugText::GetInstance()->ConsolePrintf("isFloor landing\n");
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}

#pragma endregion

Vector3 Player::CornerPositio(const Vector3& center, Corner corner) {
	float halfW = kWidth / 2.0f;
	float halfH = kHight / 2.0f;

	Vector3 offsetTable[kNumCorner] = {
	    {+halfW, -halfH, 0.0f}, // kRightBottom
	    {-halfW, -halfH, 0.0f}, // kLeftBottom
	    {+halfW, +halfH, 0.0f}, // kRightTop
	    {-halfW, +halfH, 0.0f}  // kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

Vector3 Player::GetWorldPodition() {

	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Player::GetAABB() {

	Vector3 worldPos = GetWorldPodition();

	AABB aabb;

	aabb.min = {worldPos.x - worldTransform_.scale_.x / 2.0f, worldPos.y - worldTransform_.scale_.y / 2.0f, worldPos.z - worldTransform_.scale_.z / 2.0f};
	aabb.max = {worldPos.x + worldTransform_.scale_.x / 2.0f, worldPos.y + worldTransform_.scale_.y / 2.0f, worldPos.z + worldTransform_.scale_.z / 2.0f};

	return aabb;
}

void Player::OnCollsion(const Enemy* enemy) {
	(void)enemy;
	isDead_ = true;

	velocity_ += Vector3(0, 0.4f, 0);
}