#pragma once
#include"ViewProjection.h"

#include"WorldTransform.h"

#include"MyMath.h"
#include "ImGuiManager.h"
#include"ViewProjection.h"
class RailCamera {

	public:

		void Initialize(Vector3 worldPos, Vector3 radian);


		void Update();

		const ViewProjection& GetViewProjection() { return viewProjection_; }

	    const WorldTransform& GetWorldTransform() { return worldTransform_; }

		private:
	    WorldTransform worldTransform_;

		ViewProjection viewProjection_;

		Vector3 velocity_;


};
