#include "Player.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include <TextureManager.h>
#include <ViewProjection.h>
#include <cassert>
#include "WinApp.h"


Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	sprite2DReticle_;
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::OnCollision() {}

void Player::SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

void Player::DrawUI() { sprite2DReticle_->Draw(); }

void Player::Initialize(Model* model, uint32_t textureHandle, Vector3 position) {
	assert(model);

	model_ = model;

	textureHandle_ = textureHandle;

	input_ = Input::GetInstance();

	// X,Y,Z方向の平行移動を設定
	worldTransform_.translation_ = position;

	// X,Y,Z 方向のスケーリングを設定
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	// worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	worldTransform_.Initialize();

	worldTransform3DReticle_.Initialize();

	uint32_t textureReticle = TextureManager::Load("Reticle.jpg");

	sprite2DReticle_ =
	    Sprite::Create(textureReticle, {640.0f, 360.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
}

void Player::Update(ViewProjection& viewProjection) {

	Vector3 move = {0, 0, 0};

	const float kCharacterSpeed = 0.1f;

	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	if (input_->PushKey(DIK_UP)) {
		move.y -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y += kCharacterSpeed;
	}

	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y -= move.y;
	worldTransform_.translation_.z += move.z;

	const float kRotSpeed = 0.02f;

	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D))

	{
		worldTransform_.rotation_.y += kRotSpeed;
	}

	worldTransform_.UpdateMatrix();

	// キャラクターの座標を画面表示する処理

	ImGui::Begin("Debug");
	float playerPos[] = {
	    worldTransform_.translation_.x,
	    worldTransform_.translation_.y,
	    worldTransform_.translation_.z,

	};
	ImGui::InputFloat("PlayerPos", playerPos, 0, 1);
	ImGui::SliderFloat3("PlayerPos", playerPos, 0, 1);

	worldTransform_.translation_.x = playerPos[0];

	worldTransform_.translation_.y = playerPos[1];

	worldTransform_.translation_.z = playerPos[2];

	ImGui::End();

	const float kMoveLimitX = 31;

	const float kMoveLimitY = 17;

	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);

	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);

	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);

	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	const float kDistancePlayerTo3DReticle = 50.0f;

	Vector3 offset = {0, 0, 1.0f};

	offset = Multiply(offset, worldTransform_.matWorld_);

	offset = Multiply(kDistancePlayerTo3DReticle, Normalise(offset));

	// 3Dレティクルの座標を設定
	worldTransform3DReticle_.translation_ = Add(GetWorldPosition(), offset);
	worldTransform3DReticle_.UpdateMatrix();

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	Vector3 PositonReticle = Subtract(
	    {worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
	     worldTransform3DReticle_.matWorld_.m[3][2]},
	    GetWorldPosition());

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewPortMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

	Matrix4x4 matViewProjectionViewport =
	    Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

	PositonReticle = Transform(PositonReticle, matViewProjectionViewport);

	sprite2DReticle_->SetPosition(Vector2(PositonReticle.x, PositonReticle.y));

	POINT mousePosition;

	GetCursorPos(&mousePosition);

	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	sprite2DReticle_->SetPosition(Vector2((float)PositonReticle.x, (float)PositonReticle.y));

	Vector2 spritePosition = sprite2DReticle_->GetPosition();

	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		spritePosition.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 5.0f;
		spritePosition.y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 5.0f;
		// スプライトの座標変更を反映
		sprite2DReticle_->SetPosition(spritePosition);
	}

	Matrix4x4 matVPV =
	    Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

	Matrix4x4 matInverseVPV = Inverse(matVPV);

	// スクリーン座標
	Vector3 posNear = {
	    (float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 0};

	Vector3 posFar = {
	    (float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 1};

	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);

	// マウスレイの方向
	Vector3 mouseDirection = Subtract(posFar, posNear);
	mouseDirection = Normalise(mouseDirection);

	// カメラから標準オブジェクトの距離
	const float kDistanceTestObject = 100.0f;

	worldTransform3DReticle_.translation_ =
	    Add(posNear, Multiply(kDistanceTestObject, mouseDirection));

	worldTransform3DReticle_.UpdateMatrix();

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}

	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	worldTransform_.UpdateMatrix();

	Attack();
	// デスフラグの経った球を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		// 速度ベクトルを自機の向きに合わせて回転させる
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		// 弾を生成し初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);

		bullets_.push_back(newBullet);
	}

	// velocity =

	XINPUT_STATE joyState;

	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	// Rトリガーを押していたら
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
	}
}

void Player::Drow(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}

	model_->Draw(worldTransform3DReticle_, viewProjection);
}
