#include "Enemy.h"
#include "MyMath.h"
#include <cassert>
#include"Player.h"
#include <TextureManager.h>



Enemy::~Enemy() {
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Enemy::OnCollision() {
	
}


Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::Initialize(Model* model, const Vector3& position) {

	assert(model);

	model_ = model;

	textureHandle_ = TextureManager::Load("enemy2.png");

	// 拡大縮小
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	// 回転量
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	// 座標
	worldTransform_.translation_ = position;

	worldTransform_.Initialize();

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = {0.0f,0.0f,0.1f};

	

	Approach();
}

void Enemy::Update() 
{
	worldTransform_.translation_.x = 3;
	// 座標を移動させる
	worldTransform_.translation_.x -= velocity_.x;
	worldTransform_.translation_.y -= velocity_.y;
	worldTransform_.translation_.z -= velocity_.z;


	/*worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();*/
	worldTransform_.UpdateMatrix();

	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});


	switch (phase_) {

	case Enemy::Phase::Approach:
	default:
		//移動（ベクトルを加算）
		worldTransform_.translation_.x -= velocity_.x;
		worldTransform_.translation_.y -= velocity_.y;
		worldTransform_.translation_.z -= velocity_.z;
		//既定の位置に到達したら離脱
		if (worldTransform_.translation_.z<0.0f) 
		{
			phase_ = Phase::Leave;
		}
		timer++;
		if (timer == 100)
		{
			Fire();

			timer = kFireInterval;
		}

		break;
	case Enemy::Phase::Leave:
		// 移動（ベクトルを加算）
		worldTransform_.translation_.x -= 0.2f;
		worldTransform_.translation_.y += 0.2f;
		worldTransform_.translation_.z -= 0.2f;
		break;
	}

	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

}




void Enemy::Draw(const ViewProjection& viewProjection) 
{

	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}

	model_->Draw(worldTransform_, viewProjection, textureHandle_);

		
}

void Enemy::Fire() {

	assert(player_);
	// 弾の速度
	const float kBulletSpeed = 2.0f;

	Vector3 enemy = Enemy::GetWorldPosition();

	Vector3 player = player_->GetWorldPosition();

	Vector3 velocity = Subtract(player,enemy );

	velocity = Normalise(velocity);

	velocity.x *= kBulletSpeed;
	velocity.y *= kBulletSpeed;
	velocity.z *= kBulletSpeed;

	// 速度ベクトルを自機のN向きに合わせて回転させる
	velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	// 弾を生成し初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	bullets_.push_back(newBullet);

}

void Enemy::Approach() 
{ timer = kFireInterval;

}