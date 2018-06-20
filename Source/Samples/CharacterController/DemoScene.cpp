#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

#include "CharacterController.h"
#include "DemoScene.h"
#include "CharacterController\Touch.h"

#include <Urho3D\/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(DemoScene)

DemoScene::DemoScene(Context* context) :
	Sample(context)
{
	CharacterController::RegisterObject(context); 
}

DemoScene::~DemoScene() = default; 

void DemoScene::Start()
{
	Sample::Start();
	if (touchEnabled_)
		touch = new Touch(context_, 
			TOUCH_SENSITIVITY);

	CreateScene();

	CreateAvatar(); 

	SubscribeToEvents();

	Sample::InitMouseMode(MM_RELATIVE); 
}

void DemoScene::CreateScene()
{
	auto* cache = GetSubsystem<ResourceCache>();

	scene_ = new Scene(context_); 

	scene_->CreateComponent<Octree>();
	scene_->CreateComponent<PhysicsWorld>();

	cameraNode_ = new Node(context_);
	auto* camera = cameraNode_->
		CreateComponent<Camera>();
	camera->SetFarClip(300.0f); 
	GetSubsystem<Renderer>()->SetViewport(0,
		new Viewport(context_, scene_, camera)); 

	Node* zoneNode = scene_->CreateChild("Zone"); 
	auto* zone = zoneNode->CreateComponent<Zone>();
	zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f)); 
	zone->SetFogColor(Color(0.5f, 0.5f, 0.7f));
	zone->SetFogStart(100.0f); 
	zone->SetFogEnd(300.0f); 
	zone->SetBoundingBox(BoundingBox(-1000.0f,
		1000.0f));

	Node* lightNode = scene_->
		CreateChild("DirectionalLight");
	lightNode->SetDirection(Vector3(0.3f, -0.5f, 
		0.425f));
	auto* light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL); 
	light->SetCastShadows(true); 
	light->SetShadowBias(BiasParameters(0.00025f,
		0.5f));
	light->SetShadowCascade(CascadeParameters(10.0f,
		50.0f, 200.0f, 0.0f, 0.8f)); 
	light->SetSpecularIntensity(0.5f);

	Node* floorNode = scene_->CreateChild("Floor"); 
	floorNode->SetPosition(Vector3(0.0f, -0.5f, 0.0f));
	floorNode->SetScale(Vector3(200.0f, 1.0f, 200.0f));
	auto* object = floorNode->
		CreateComponent<StaticModel>();
	object->SetModel(cache->GetResource<Model>
		("Models/Box.mdl")); 
	object->SetMaterial(cache->GetResource<Material>
		("Materials/Stone.xml"));

	auto* rb = floorNode->CreateComponent<RigidBody>();
	rb->SetCollisionLayer(2); 
	auto* collider = floorNode->
		CreateComponent<CollisionShape>();
	collider->SetBox(Vector3::ONE);
}

void DemoScene::CreateAvatar()
{
	auto* cache = GetSubsystem<ResourceCache>();

	Node* objectNode = scene_->CreateChild("Avatar");
	objectNode->SetPosition(Vector3(0.0f, 1.0f, 0.0f));
	auto* object = objectNode->
		CreateComponent<AnimatedModel>();
	object->SetModel(cache->GetResource<Model>
		("Models/Mutant/Mutant.mdl"));
	object->SetCastShadows(true);

	object->GetSkeleton().GetBone
	("Mutant:Head")->animated_ = false; 

	auto* rb = objectNode->
		CreateComponent<RigidBody>();
	rb->SetCollisionLayer(1); 
	rb->SetMass(1.0f); 

	rb->SetAngularFactor(Vector3::ZERO); 

	rb->SetCollisionEventMode(COLLISION_ALWAYS);

	auto* collider = objectNode->CreateComponent
		<CollisionShape>();
	collider->SetCapsule(0.7f, 1.8f, Vector3(0.0f,
		0.9f, 0.0f)); 

	character = objectNode->CreateComponent
		<CharacterController>();
}
void DemoScene::SubscribeToEvents()
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER
	(DemoScene, HandleUpdate));

	SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER
	(DemoScene, HandlePostUpdate));

	UnsubscribeFromEvent(E_SCENEUPDATE);
}

void DemoScene::HandleUpdate(StringHash eventType,
	VariantMap& eventData)
{
	using namespace Update; 

	auto* input = GetSubsystem<Input>();

	if (character)
	{
		character->_PlayerControls.Set(CTRL_JUMP, 
			false);

		if (touch)
		{
			touch->UpdateTouches(character->
				_PlayerControls);
		}

		auto* ui = GetSubsystem<UI>();
		if (!ui->GetFocusElement())
		{
			character->_PlayerControls.Set(CTRL_JUMP,
				input->GetKeyDown(KEY_SPACE));

			if (touchEnabled_)
			{
				for (unsigned i = 0; i < input->
					GetNumTouches(); ++i)
				{
					TouchState* state = input->
						GetTouch(i);

					if (!state->touchedElement_)
					{
						auto* camera = cameraNode_->
							GetComponent<Camera>();

						if (!camera)
							return; 

						auto* graphics =
							GetSubsystem<Graphics>();

						character->_PlayerControls.yaw_ +=
							TOUCH_SENSITIVITY * camera->
							GetFov() / graphics->GetHeight()
							*state->delta_.x_; 

						character->_PlayerControls.pitch_ +=
							TOUCH_SENSITIVITY * camera->
							GetFov() / graphics->GetHeight()
							*state->delta_.y_; 
					}
				}
			}
			else
			{
				character->_PlayerControls.yaw_ +=
					(float)input->GetMouseMoveX() *
					YAW_SENSITIVITY; 

				character->_PlayerControls.pitch_ +=
					(float)input->GetMouseMoveY() *
					YAW_SENSITIVITY;

			}

			character->_PlayerControls.pitch_ =
				Clamp(character->_PlayerControls.pitch_,
					-80.0f, 80.0f);

			character->GetNode()->SetRotation(
				Quaternion(character->
					_PlayerControls.yaw_, 
					Vector3::UP));
		}

	}
}

void DemoScene::HandlePostUpdate(
	StringHash eventType, VariantMap& eventData)
{
	if (!character)
		return; 

	Node* characterNode = character->GetNode();

	const Quaternion& rot = characterNode->
		GetRotation();

	Quaternion dir = rot * Quaternion(character->
		_PlayerControls.pitch_, Vector3::RIGHT); 

	Vector3 aimPoint = characterNode->GetPosition()
		+ rot * Vector3(0.0f, 1.7f, 0.0f); 

	Vector3 rayDir = dir * Vector3::BACK; 
	float rayRange = touch ? touch->
		cameraDistance_ : CAMERA_INITIAL_DIST;
	PhysicsRaycastResult result; 
	scene_->GetComponent<PhysicsWorld>()->
		RaycastSingle(result, Ray(aimPoint,
			rayDir), rayRange, 2);

	if (result.body_)
	{
		rayRange = Min(rayRange,
			result.distance_); 
	}

	rayRange = Clamp(rayRange, CAMERA_MIN_DIST,
		CAMERA_MAX_DIST); 

	cameraNode_->SetPosition(aimPoint + rayDir *
		rayRange);
	cameraNode_->SetRotation(dir); 
}