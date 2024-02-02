#include "RailCamera.h"

void RailCamera::Initialize(Vector3 worldPos, Vector3 radian) 
{
	//RailCamera::
	worldTransform_.translation_ = worldPos;
	worldTransform_.rotation_ = radian;

	velocity_ = {0.0f, 0.0f, 0.5f};
	worldTransform_.Initialize();
	viewProjection_.Initialize();
}
void RailCamera::Update() 
{
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	/*worldTransform_.rotation_.x += velocity_.x;
	worldTransform_.rotation_.y += velocity_.y;
	worldTransform_.rotation_.z += velocity_.z;*/

	worldTransform_.matWorld_ = MakeAffineMatrix(
	    {1.0f, 1.0f, 1.0f}, worldTransform_.rotation_, worldTransform_.translation_);

	viewProjection_.matView = Inverse(worldTransform_.matWorld_);

	worldTransform_.UpdateMatrix();

	//ImGui::Begin("Camera");

	//static float slider1 = 0.0;

	//ImGui::SliderFloat("slider 1", &slider1, 0.0f, 1.0f);
	ImGui::Begin("Camera");
	float ttt[] = {
	    worldTransform_.translation_.x,
	    worldTransform_.translation_.y,
	    worldTransform_.translation_.z,

	};
	ImGui::SliderFloat3("translation", ttt, 0, 1009);

	worldTransform_.translation_.x = ttt[0];
									 
	worldTransform_.translation_.y = ttt[1];
									 
	worldTransform_.translation_.z = ttt[2];

	float rrr[] = {
	    worldTransform_.rotation_.x,
	    worldTransform_.rotation_.y,
	    worldTransform_.rotation_.z,
	};

	ImGui::SliderFloat3("rotation", rrr, 0, 1);

	worldTransform_.rotation_.x = rrr[0];

	worldTransform_.rotation_.y = rrr[1];

	worldTransform_.rotation_.z = rrr[2];


	ImGui::End();

}
