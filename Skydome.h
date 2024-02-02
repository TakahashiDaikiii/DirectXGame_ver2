#pragma once

#include "Model.h"
#include "WorldTransform.h"
class Skydome 
{
public:

	///初期化
	void Initialize(Model* model);


	void Update();

	void Draw(ViewProjection& viewProjection);

	private:

		WorldTransform worldTransform_;

		Model* model_ = nullptr;

	





};
