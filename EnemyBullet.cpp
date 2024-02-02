#include "EnemyBullet.h"
#include "MyMath.h"
#include <cassert>
#include <TextureManager.h>
void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	assert(model);

	model_ = model;

	textureHandle_ = TextureManager::Load("black.png");

	// 拡大縮小
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	// 回転量
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	// 座標
	worldTransform_.translation_ = position;

	worldTransform_.Initialize();

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
}

void EnemyBullet::Update() {
	// 座標を移動させる
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// worldTransform_.matWorld_ = MakeAffineMatrix(
	//     worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void EnemyBullet::OnCollision() 
{ isDead_ = true; }

Vector3 EnemyBullet::GetWorldPosition() 

{
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
