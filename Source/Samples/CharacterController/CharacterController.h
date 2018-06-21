#pragma once


#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D\Input\Controls.h>
#include <Urho3D\Scene\LogicComponent.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Scene/Node.h>

using namespace Urho3D; 

const unsigned CTRL_FORWARD = 1; 
const unsigned CTRL_BACK = 2; 
const unsigned CTRL_LEFT = 4; 
const unsigned CTRL_RIGHT = 8;
const unsigned CTRL_JUMP = 16;

const float MOVEMENT_FORCE = 8.0f; 
const float MOVEMENT_DECCELERATION = 2.0f; 
const float INAIR_MOVE_FORCE = 0.02f; 
const float JUMP_VELOCITY = 7.0f; 
const float INAIR_MAX_TIME = 0.1f; 
const float YAW_SENSITIVITY = 0.1f; 
const float CHARACTER_FRICTION = 0.0f; 


class CharacterController : public LogicComponent
{
	URHO3D_OBJECT(CharacterController,
		LogicComponent); 

public: 

	explicit CharacterController(Context* context); 

	static void RegisterObject(Context* context); 

	void Start() override; 

	void FixedUpdate(float timeStep) override; 

	Controls _PlayerControls; 

private:

	void HandleNodeCollision(StringHash eventType,
		VariantMap& eventData); 

	bool grounded; 

	bool canJump; 

	float timeInAir; 

	Vector3 velocity_; 

	WeakPtr<RigidBody> rb; 
};
