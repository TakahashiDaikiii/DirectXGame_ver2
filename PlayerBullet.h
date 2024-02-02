#pragma once
#include"Model.h"
#include "WorldTransform.h"
#include<cassert>



class PlayerBullet 
{
public:
	///<summary>
	///初期化
	/// </summary>
	/// <parm name="model">モデル</param>
	/// <parm name="position">初期座標</param>
	
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);


	///<summary>
	///更新
	/// </summary>
	/// 
	
	void Update();

	///< summary>
	/// 描画
	///  </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
  
	void Draw(const ViewProjection& viewProjection);

	void OnCollision();

	bool IsDead() const { return isDead_; }

	float GetRadius() { return r; }

	Vector3 GetWorldPosition();

	private:

		float r = 1.0f;

		WorldTransform worldTransform_;

	    Model* model_ = nullptr;

	    uint32_t textureHandle_ = 0u;

		Vector3 velocity_;

		//寿命
	    static const int32_t kLifeTime = 60 * 5;

		//ですタイマー
	    int32_t deathTimer_ = kLifeTime;
		//デスフラグ
	    bool isDead_ = false;

};
