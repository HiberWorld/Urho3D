#include <Bullet\btBulletDynamicsCommon.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>
#include <Bullet\BulletDynamics\Character\btKinematicCharacterController.h>

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>

#include "KinematicCharacterController.h"
#include "Global.h"

#undef new 

const int CTRL_FORWARD = 1; 
const int CTRL_BACK = 2; 
const int CTRL_LEFT = 4; 
const int CTRL_RIGHT = 8; 
const int CTRL_JUMP = 16;


KinematicCharacterController::KinematicCharacterController(Context* context)
	: LogicComponent(context)
{
	SetUpdateEventMask(USE_FIXEDUPDATE | USE_UPDATE);
}

KinematicCharacterController::~KinematicCharacterController()
{

}

void KinematicCharacterController::RegisterObject(Context* context)
{
	context->RegisterFactory<KinematicCharacterController>();

	URHO3D_ATTRIBUTE("Controls Yaw", float,
		playerControls_.yaw_, 0.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("Controls Pitch", float,
		playerControls_.pitch_, 0.0f, AM_DEFAULT);
}

void KinematicCharacterController::FixedUpdate(float timestep)
{
	const float MOVEMENT_SPEED = 10.0f;

	Vector3 moveDir; 

	if (playerControls_.IsDown(CTRL_FORWARD))
		moveDir += Vector3::FORWARD;
	if (playerControls_.IsDown(CTRL_BACK))
		moveDir += Vector3::BACK; 
	if (playerControls_.IsDown(CTRL_LEFT))
		moveDir += Vector3::LEFT;
	if (playerControls_.IsDown(CTRL_RIGHT))
		moveDir += Vector3::RIGHT;

	if (playerControls_.IsDown(CTRL_JUMP))
	{
		if (bulletController_->onGround())
		{
			bulletController_->jump(btVector3(0,6,0));
		}
	}
	
	if (moveDir.LengthSquared() > 0.0f)
	{
		moveDir.Normalize();

		moveDir = node_->GetWorldRotation() * moveDir;

		if (bulletController_->onGround())
			bulletController_->setWalkDirection(ToBtVector3
			(moveDir * timestep * MOVEMENT_SPEED));
		else
			bulletController_->setWalkDirection(ToBtVector3(
				moveDir * timestep * MOVEMENT_SPEED * 0.5f));
	}
	else
	{
		bulletController_->setWalkDirection(
			btVector3(0, 0, 0));
	}

		btTransform t; 
		t = bulletController_->getGhostObject()->
			getWorldTransform(); 
		Vector3 newPos = ToVector3(t.getOrigin())
			+ Vector3::DOWN * height_ * 0.5f; 

		node_->SetWorldPosition(newPos);
	}
	void KinematicCharacterController::Update(float timestep)
	{
		auto* input = GetSubsystem<Input>();

		playerControls_.Reset(); 

		playerControls_.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
		playerControls_.Set(CTRL_BACK, input->GetKeyDown(KEY_S)); 
		playerControls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
		playerControls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));
		playerControls_.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE)); 

		btTransform t; 
		t = bulletController_->getGhostObject()->getWorldTransform();
		t.setRotation(ToBtQuaternion(node_->GetRotation())); 
		bulletController_->getGhostObject()->setWorldTransform(t); 
	}
	void KinematicCharacterController::CreatePhysComponents(float height, float diameter)
	{
		btTransform startTransform; 
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(0, 10, 4));
		startTransform.setRotation(ToBtQuaternion
		(Quaternion(90, 0, 0)));

		btConvexShape* capsule = new btCapsuleShape
		(diameter * 0.5, height - diameter);
		height_ = height; 
		diameter_ = diameter; 

		btDiscreteDynamicsWorld* world = GetScene()->
			GetComponent<PhysicsWorld>()->GetWorld();

		ghostObject_ = new btPairCachingGhostObject();
		ghostObject_->setWorldTransform(startTransform);
		world->getBroadphase()->getOverlappingPairCache()->
			setInternalGhostPairCallback(new btGhostPairCallback());
		ghostObject_->setCollisionShape(capsule);
		ghostObject_->setCollisionFlags(btCollisionObject::
			CF_CHARACTER_OBJECT);
		bulletController_ = new btKinematicCharacterController(
			ghostObject_, capsule, 0.3f, btVector3(0, 0, 1));
		bulletController_->setGravity(world->getGravity());

		world->addCollisionObject(ghostObject_, btBroadphaseProxy::
			CharacterFilter,btBroadphaseProxy::AllFilter); 
		world->addAction(bulletController_);
		bulletController_->setMaxJumpHeight(1.5); 
	}

	void KinematicCharacterController::Start()
	{

	}