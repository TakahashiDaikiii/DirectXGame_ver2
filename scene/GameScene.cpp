#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <AxisIndicator.h>
#include<fstream>
GameScene::GameScene() {}

GameScene::~GameScene() 
{

delete model_;
	delete player_;
	delete debugCamera_;
	delete enemy_;
	delete skydome_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	
	TextureManager::Load("Reticle.jpg");

	textureHandle_ = TextureManager::Load("Playerr1.png");
	model_ = Model::Create();

	worldTransform_.Initialize();

	railCamera_ = new RailCamera();

	railCamera_->Initialize({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f});

	// ビュープロジェクション
	viewProjection_.farZ = 600;

	viewProjection_.Initialize();

	Vector3 playerPosition(0, 0, 50);

	player_ = new Player();
	player_->Initialize(model_, textureHandle_, playerPosition);

	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	enemy_ = new Enemy();

	enemy_->Initialize(model_, {0.0f, 0.0f, 50.0f});

	enemy_->SetPlayer(player_);

	enemy_->SetGameScene(this);

	debugCamera_ = new DebugCamera(1280, 720);

	AxisIndicator::GetInstance()->SetVisible(true);

	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	skydome_ = new Skydome();

	skydome_->Initialize(modelSkydome_);
}

void GameScene::Update() {
#ifdef _DEBUG

	if (input_->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = true;
	}

#endif // _DEBUG

	if (isDebugCameraActive_) {
		debugCamera_->Update();

		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

		viewProjection_.TransferMatrix();
	} else {
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;

		viewProjection_.TransferMatrix();

		// viewProjection_.UpdateMatrix();
	}

	player_->Update(viewProjection_);

	enemy_->Update();

	debugCamera_->Update();

	skydome_->Update();

	railCamera_->Update();

	CheckALLCollisions();
}

void GameScene::SpawnEnemy(const Vector3& position) {
	if (enemy_ != nullptr) {

		delete enemy_;
	}

	enemy_ = new Enemy();
	enemy_->Initialize(model_, position);
	enemy_->SetPlayer(player_);
	enemy_->SetGameScene(this);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	player_->Drow(viewProjection_);

	enemy_->Draw(viewProjection_);

	skydome_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	
	player_->DrawUI();

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckALLCollisions() {

	// 判定対象AとBの座標
	Vector3 posA, posB;
	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();

	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

#pragma region
	// 自キャラの座標
	Vector3 player = player_->GetWorldPosition();

	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
		posB = bullet->GetWorldPosition();

		Vector3 d = Subtract(posA, posB);
		// d x y z を二乗してタス
		float dist = d.x * d.x + d.y * d.y + d.z * d.z;

		float r1 = player_->GetRadius(); // プレイヤーの半径

		float r2 = bullet->GetRadius(); // 敵弾の半径

		float radius = r1 * r1 + r2 * r2;
		if (dist <= radius) {
			player_->OnCollision();

			bullet->OnCollision();
		}
	}

	// 自弾と敵キャラの当たり判定

	// 敵キャラの座標
	Vector3 Enemy = enemy_->GetWorldPosition();

	posA = enemy_->GetWorldPosition();
	// 敵キャラと自弾の当たり判定
	for (PlayerBullet* bullet : playerBullets) {

		posB = bullet->GetWorldPosition();

		Vector3 d = Subtract(posA, posB);

		float dist = d.x * d.x + d.y * d.y + d.z * d.z;

		float r1 = enemy_->GetRadius();

		float r2 = bullet->GetRadius();

		float radius = r1 * r1 + r2 * r2;
		if (dist <= radius) {
			enemy_->OnCollision();

			bullet->OnCollision();
		}
	}

	for (EnemyBullet* enemyBullet : enemyBullets) {

		posA = enemyBullet->GetWorldPosition();

		for (PlayerBullet* playerBullet : playerBullets) {
			posB = playerBullet->GetWorldPosition();

			Vector3 d = Subtract(posA, posB);

			float dist = d.x * d.x + d.y * d.y + d.z * d.z;

			float r1 = enemyBullet->GetRadius();

			float r2 = playerBullet->GetRadius();

			float radius = r1 * r1 + r2 * r2;
			if (dist <= radius) {
				enemyBullet->OnCollision();

				playerBullet->OnCollision();
			}
		}
	}
}

void GameScene::UpdateEnemyBullets() {
	// 敵弾リストの更新処理
	for (auto bullet : enemybullets_) {
		bullet->Update();
	}
}

void GameScene::DrawEnemyBullets() {
	// 敵弾リストの描画処理
	for (auto bullet : enemybullets_) {
		bullet->Draw(viewProjection_);
	}
}

void GameScene::ReleaseEnemyBullets() {
	// 敵弾リストの解放処理
	for (auto bullet : enemybullets_) {
		delete bullet;
	}
	enemybullets_.clear(); // リストをクリア
}

void GameScene::SetGameSceneForEnemy(Enemy* enemy) {
	// 敵にゲームシーンのアドレスをセットする
	enemy->SetGameScene(this);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// 敵弾をリストに登録する
	enemybullets_.push_back(enemyBullet);
}

void GameScene::CheckAllCollisions() {}

void GameScene::LoadEnemyPopData() {
	std::ifstream file;
	file.open("enemyPop.csv");
	assert(file.is_open());

	enemyPopCommands << file.rdbuf();

	file.close();
}

// void GameScene::CheckAllCollisions() {
//	// 当たり判定の処理（例えば、敵と全ての自弾の当たり判定）
//	for (Enemy* enemy : enemys_) {
//		for (EnemyBullet* bullet : enemybullets_) {
//			// 当たり判定処理
//			// ...
//		}
//	}
//  }

void GameScene::UpdateEnemyPopCommands() {

	if (isWaiting_) {
		waitingTimer_--;
	}
	if (waitingTimer_ <= 0) {
		isWaiting_ = false;
	}

	std::string line;

	while (std::getline(enemyPopCommands, line))

	{
		std::istringstream line_stream(line);

		std::string word;

		std::getline(line_stream, word, ',');

		if (word.find("//") == 0) {
			continue;
		}

		if (word.find("POP") == 0) {

			std::getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			std::getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			std::getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			SpawnEnemy(Vector3(x, y, z));
		}

		else if (word.find("WITE") == 0) {
			std::getline(line_stream, word, ',');

			int32_t waitTime = atoi(word.c_str());

			isWaiting_ = true;
			waitingTimer_ = waitTime;

			break;
		}
	}
}