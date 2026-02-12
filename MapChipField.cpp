#include "MapChipField.h"
#include <map>
#include <fstream>
#include <sstream>

// コンストラクタ（ここでメモリを確保する！）
MapChipField::MapChipField() { mapChipData_ = new MapChipData(); }

// デストラクタ（ここで後始末をする！）
MapChipField::~MapChipField() { delete mapChipData_; }

// 無名のネームスペースを使用して衝突を避ける
using namespace KamataEngine;

namespace {

// マップチップの種類と対応するデータの定義
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};

} // namespace

void MapChipField::ResetMaoChipData() {

	//マップチップのデータをリセット
    mapChipData_->data.clear(); // 既存のデータをクリア
	mapChipData_->data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_->data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std ::string& filePath) {

	// マップチップのデータリセット
	ResetMaoChipData();

	// ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	// マップチップのCSV
	std::stringstream mapChipCsv;
	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();
	// ファイルを閉じる
	file.close();

	// CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		std::string line;
		getline(mapChipCsv, line);

		//一行分の文字列をストリーム変換して解析しやすく
		std::istringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
		
			std::string word;
			std::getline(lineStream, word, ',');//カンマ区切りでの読み込み
			
			if (mapChipTable.contains(word)) {
			mapChipData_->data[i][j] = mapChipTable[word];
			}
		
		}

	}


}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	// インデックスからマップチップの種類を取得
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex<0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}

	return mapChipData_->data[yIndex][xIndex];
}

Vector3 MapChipField::GetMapChipPositionByIndex(
	uint32_t xIndex, uint32_t yIndex
) {

	return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0);

}
