#pragma once

#include <Urho3D\Input\Controls.h>
#include <Urho3D\Scene\LogicComponent.h>

using namespace Urho3D; 

const unsigned CTRL_JUMP = 16;

const float INAIR_MOVE_FORCE = 0.02f; 
const float JUMP_VELOCITY = 7.0f; 
const float INAIR_MAX_TIME = 0.1f; 
const float YAW_SENSITIVITY = 0.1f; 


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

};
