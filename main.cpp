#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

// グローバルな名前を持つ蒲田エンジンを作成
using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	//============================
	// 初期化
	//============================

	// 蒲田エンジンのロード
	KamataEngine::Initialize(L"AE2?_99_ヤマト_ユウヤ_AL2");

	// DirectXのロード
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	// ゲームシーンの呼び出し
	GameScene* gameScene = new GameScene();
	gameScene->Initialize();

	// ここから下のクラスのインスタンスなどの情報を追加

	//============================
	// 更新処理
	//============================

	while (true) {

		// 蒲田エンジンのアップデート
		if (KamataEngine::Update()) {
			break;
		}

		//---------------------------------------------------
		// 更新処理ここから
		//---------------------------------------------------

		// この中に更新処理などを随時追加していく

		gameScene->Updatta();

		//---------------------------------------------------
		// 更新処理ここまで
		//---------------------------------------------------

		//---------------------------------------------------
		// 描画処理ここから
		//---------------------------------------------------

		// DirectXの描画処理を追加
		dxCommon->PreDraw(); // ここから開始して

		gameScene->Draw();

		dxCommon->PostDraw(); // ここで終了

		//---------------------------------------------------
		// 描画処理こまで
		//---------------------------------------------------
	}



	//---------------------------------------------------
	//解放エクササイズ
	//---------------------------------------------------

	// ゲームシーンの解放

	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;

	// ゲームの終了
	KamataEngine::Finalize();
	return 0;
}
