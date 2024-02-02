#pragma once
#include "Model.h"
#include "PlayerBullet.h"
#include "WorldTransform.h"
#include <Input.h>
#include <PrimitiveDrawer.h>
#include <cassert>
#include <list>
#include <Sprite.h>

class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, uint32_t textureHandle,Vector3 playerPosition);
	/// <summary>
	/// 更新
	/// </summary>
	void Update(ViewProjection& viewProjection);

	///
	/// 攻撃
	///
	void Attack();

	/// <summary>
	/// 描画
	/// </summary>
	void Drow(ViewProjection& viewProjection);

	/// <summary>
	/// デストラクタ
	/// </summary>

	~Player();

	Vector3 GetWorldPosition();

	void OnCollision();

	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	float GetRadius() { return r; }
	/// <summary>
	/// 
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(const WorldTransform* parent);

	void DrawUI();

	

private:
	float r = 1.0f;

	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	uint32_t textureHandle_ = 0u;

	Input* input_ = nullptr;

	// 弾
	std::list<PlayerBullet*> bullets_;

	WorldTransform worldTransform3DReticle_;

	Sprite* sprite2DReticle_ = nullptr;
};
