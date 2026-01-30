#include "GameScene.h"
//===================================================
// パブリックの処理
//===================================================

using namespace KamataEngine;

 //コンストラクタ
 GameScene::GameScene() {
	Initialize();
 }

void GameScene::Initialize() {

	// 初期化処理の追加

	// 関数を使っての初期化などあとはnew関連も
	//  トランスフォームとカメラの初期化
	worldTransform_.Initialize();
	camera_.Initialize();

	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	debugCamera_ = new DebugCamera(1280, 720);

	//ブレンダーみたいな表示線の関数初期化
	AxisIndicator::GetInstance()->SetVisible(true);

	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	// 音楽や画像などを読み込む場所さらにはそれらを使ものも入れる

#pragma region 画像読み込み範囲

	textureHandle_ = TextureManager::Load("./Resources/mario.jpg");
#pragma endregion

	// 音楽読み込みここから
#pragma region 音楽読み込み範囲
	soundDataHandle_ = Audio::GetInstance()->LoadWave("./Resources/fanfare.wav"); // 音楽読み込み

	// ここまで
#pragma endregion

	// その他の初期化処理
	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true); // 音楽がループで再生するかの

	// スプライトとモデルの作成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	model_ = Model::Create();
}

void GameScene::Updatta() {

	// 更新処理の追加
	Vector2 position = sprite_->GetPosition();
	position.x += 2.0f;
	position.y += 1.0f;

	sprite_->SetPosition(position);

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		Audio::GetInstance()->StopWave(voiceHandle_);
	}

	debugCamera_->Update();


#ifdef DEBUG
	ImGui::Begin("Debug");
	ImGui::Text("Yamato %d.%d.%d", 2025, 1, 30);

	// 自分で入力
	ImGui::InputFloat3("InputFloat3", inputFloat);
	// スライダー
	ImGui::SliderFloat3("SliderFloat3", inputFloat, 0.0f, 1.0f);

	ImGui::ShowDemoWindow();

	ImGui::End();
#endif // DEBUG


}

void GameScene::Draw() {

	// 3Dでのらいんびょうが
	PrimitiveDrawer::GetInstance()->DrawLine3d(
	    {0, 0, 0},               // 始点
	    {0, 10, 0},              // 終点
	    {1.0f, 0.0f, 0.0f, 1.0f} // 色
	);

	// 描画処理の追加

	////写真用
	// Sprite::PreDraw();
	// sprite_->Draw();
	// Sprite::PostDraw();//

	// 3Dモデル描画用
	Model::PreDraw();
	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);
	Model::PostDraw();
}

 GameScene::~GameScene() {

	//解放エクササイズ
	delete sprite_;
	delete model_;
 delete debugCamera_;
 }

//===================================================
// プライベート内の処理
//===================================================
