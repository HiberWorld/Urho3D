#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include "Urho3D/IO/Log.h"

#include "CharacterController.h"

CharacterController::CharacterController
(Context* context) :
	LogicComponent(context),
	grounded(false),
	canJump(true),
	timeInAir(0.0f),
	moveMag_(0.0f),
	velocity_(0.0f, 0.0f, 0.5f)
{
	SetUpdateEventMask(USE_FIXEDUPDATE); 
}

void CharacterController::RegisterObject
(Context* context)
{
	context->RegisterFactory<CharacterController>();

	URHO3D_ATTRIBUTE("Controls Yaw", float,
		_PlayerControls.yaw_, 0.0f, AM_DEFAULT); 
	URHO3D_ATTRIBUTE("Controls Pitch", float,
		_PlayerControls.pitch_, 0.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("Grounded", bool, grounded,
		false, AM_DEFAULT); 
	URHO3D_ATTRIBUTE("Can Jump", bool, canJump, true,
		AM_DEFAULT); 
	URHO3D_ATTRIBUTE("Air Timer", float, timeInAir,
		0.0f, AM_DEFAULT); 
}

void CharacterController::Start()
{
	SubscribeToEvent(GetNode(), E_NODECOLLISION,
		URHO3D_HANDLER(CharacterController,
			HandleNodeCollision));
}

void CharacterController::FixedUpdate(float timeStep)
{
	auto* rb = GetComponent<RigidBody>(); 
	auto* animController = node_->GetComponent
		<AnimationController>(true); 

	if (!grounded)
	{
		timeInAir += timeStep; 
	}
	else
	{
		timeInAir = 0.0f; 
	}
	bool softGrounded = timeInAir < INAIR_MAX_TIME;

	const Quaternion& rot = node_->GetRotation();
	Vector3 moveDir = Vector3::ZERO;
	const Vector3& velocity(rb->
		GetLinearVelocity());
	const Vector3 planeVelocity(velocity.x_,
		0.0f, velocity.z_);

	if (_PlayerControls.IsDown(CTRL_FORWARD))
		moveDir += Vector3::FORWARD; 
	if (_PlayerControls.IsDown(CTRL_BACK))
		moveDir += Vector3::BACK;
	if (_PlayerControls.IsDown(CTRL_LEFT))
		moveDir += Vector3::LEFT;
	if (_PlayerControls.IsDown(CTRL_RIGHT))
		moveDir += Vector3::RIGHT;

	if (moveDir.LengthSquared() > 0.0f)
		moveDir.Normalize();

	rb->ApplyImpulse(rot * moveDir *(softGrounded
		? MOVEMENT_FORCE : INAIR_MOVE_FORCE)); 

	if (softGrounded)
	{
		if (_PlayerControls.IsDown(CTRL_JUMP))
		{
			if (canJump)
			{
				rb->ApplyImpulse(Vector3::UP *
					JUMP_VELOCITY); 
				canJump = false; 
			}
		}
		else
		{
			canJump = true; 
		}

		}
			Vector3 inertia = -planeVelocity *
				MOVEMENT_DECCELERATION;
			URHO3D_LOGDEBUG(inertia.ToString()); 
			rb->ApplyImpulse(inertia); 

	grounded = false; 
}

void CharacterController::HandleNodeCollision
(StringHash eventTypes, VariantMap& eventData)
{
	using namespace NodeCollision; 

	MemoryBuffer contacts(eventData[P_CONTACTS].
		GetBuffer()); 

	while (!contacts.IsEof())
	{
		Vector3 contactPos = contacts.ReadVector3();
		Vector3 contactNorm = contacts.ReadVector3();
		contacts.ReadFloat(); 
		contacts.ReadFloat(); 

		if (contactPos.y_ < (node_->GetPosition().y_ + 1.0f))
		{
			float elevation = contactNorm.y_; 

			if (elevation > 0.75f)
			{
				grounded = true; 
			}
		}
	}
}
