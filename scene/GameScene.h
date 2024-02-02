#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include "DebugCamera.h"
#include"Enemy.h"
#include"MyMath.h"
#include"EnemyBullet.h"
#include"PlayerBullet.h"
#include"Skydome.h"
#include"RailCamera.h"
#include<sstream>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void CheckALLCollisions();

	/// <summary>
	/// 敵弾の追加
	/// </summary>
	/// <param name="enemybullet"></param>
	// 新しいメソッド: 敵弾リストの更新
	void UpdateEnemyBullets();

	// 新しいメソッド: 敵弾リストの描画
	void DrawEnemyBullets();

	// 新しいメソッド: 敵弾リストの解放
	void ReleaseEnemyBullets();

	// 敵にゲームシーンのアドレスをセットする
	void SetGameSceneForEnemy(Enemy* enemy);

	// 新しいメソッド: 敵弾をリストに登録する
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	// 新しいメソッド: 当たり判定
	void CheckAllCollisions();

	void LoadEnemyPopData();

	void UpdateEnemyPopCommands();

	void SpawnEnemy(const Vector3& position);

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	/// 
	 uint32_t textureHandle_ = 0;

	Player* player_ = nullptr;

	Enemy* enemy_ = nullptr;

	Skydome* skydome_ = nullptr;

	Model* modelSkydome_ = nullptr;

	std::list<EnemyBullet*> enemybullets_;

	std::list<Enemy*> enemys_;

	std::stringstream enemyPopCommands;

	bool isWaiting_;

	int waitingTimer_;

	Model* model_;

	ViewProjection viewProjection_;

	WorldTransform worldTransform_;

	Vector3 velocity_;

	EnemyBullet enemybullet_;

	RailCamera* railCamera_;

	// デバッグカメラ有効

	bool isDebugCameraActive_ = false;

	DebugCamera* debugCamera_ = nullptr;
};
