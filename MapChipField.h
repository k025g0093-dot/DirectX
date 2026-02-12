#pragma once
#include "KamataEngine.h"
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {

public:

	MapChipField();
	~MapChipField();

	void ResetMaoChipData();
	void LoadMapChipCsv(const std ::string& filePath);
	int GetNumBlockHorizontal() const { return kNumBlockHorizontal; };
	int GetNumBlockVirtical() const { return kNumBlockVirtical; };
	//マップチップの種類の取得
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	//ポジション取得
	KamataEngine::Vector3 GetMapChipPositionByIndex(
		uint32_t xIndex, uint32_t yIndex
	);
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 40;

	MapChipData* mapChipData_;
};
