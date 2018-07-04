#include "Global.h"
#include "LinearMath\btGeometryUtil.h"
#include "LinearMath\btVector3.h"
#include "Urho3D\Core\Timer.h"
#include "KinematicCharacterController.h"

class KinematicCharacterScene : public Application
{
	URHO3D_OBJECT(KinematicCharacterScene, Application);

public:
	SharedPtr<Scene> scene_;
	SharedPtr<Node> cameraNode_;
	float yaw_ = 0.0f;
	float pitch_ = 0.0f;
	bool firstPerson_ = false;
	bool zoomEnabled_ = true; 

	float camTransitionTime = 1.0;
	Vector3 cameraPos; 

	Node* charNode_ = nullptr;

	KinematicCharacterScene(Context* context) :
		Application(context)
	{
		SubscribeToEvent(E_BEGINFRAME,
			URHO3D_HANDLER(KinematicCharacterScene, ApplyGameState));

		KinematicCharacterController::RegisterObject(
			context_);
	}

	void Setup()
	{
		engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
		engineParameters_[EP_FULL_SCREEN] = false;
		engineParameters_[EP_WINDOW_WIDTH] = 800;
		engineParameters_[EP_WINDOW_HEIGHT] = 600;
		engineParameters_[EP_RESOURCE_PATHS] = "Data;CoreData";
		input = GetSubsystem<Input>();
	}

	void Start()
	{
		InitGlobal(context_);

		CreateScene(); 
		SetupViewport(); 
		SubscribeToEvents(); 
		
	}

	void SetupViewport()
	{
		Camera* camera = cameraNode_->GetComponent<Camera>();
		SharedPtr<Viewport> viewport(new Viewport(context_,
			scene_, camera));
		renderer->SetViewport(0, viewport);
	}

	void ApplyGameState(StringHash eventType,
		VariantMap& eventData)
	{

	}

	void CreateScene()
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();

		scene_ = new Scene(context_);
		SharedPtr<File> levelFile = cache->GetFile
		("LevelData/Scenes/Level.xml");
		scene_->LoadXML(*levelFile);

		cameraNode_ = scene_->CreateChild("Camera");
		cameraNode_->CreateComponent<Camera>();
		cameraNode_->SetPosition(Vector3(0.0f, 5.0f, 0.0f));

		charNode_ = CreateCharacter();
	}

	void SubscribeToEvents()
	{
		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(
			KinematicCharacterScene, HandleKeyDown));
		SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(
			KinematicCharacterScene, HandleKeyUp));
		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER
		(KinematicCharacterScene, HandleUpdate));
		SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER
		(KinematicCharacterScene, HandlePhysicsPreStep));
		SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER
		(KinematicCharacterScene, HandlePostRenderUpdate));
		SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(
			KinematicCharacterScene, HandlePostUpdate));
	}

	void HandleKeyDown(StringHash eventType, VariantMap& eventData)
	{
		using namespace KeyDown;

		int key = eventData[P_KEY].GetInt();

		if (key == KEY_F)
		{
			if (zoomEnabled_)
			{
				camTransitionTime = 0.0;
				cameraPos = cameraNode_->GetPosition();
				firstPerson_ = !firstPerson_;
				zoomEnabled_ = false; 
			}
		}
	}

	void HandleKeyUp(StringHash eventType, VariantMap& eventData)
	{
		using namespace KeyUp;

		int key = eventData[P_KEY].GetInt();

		zoomEnabled_ = !zoomEnabled_;
	}

	void HandleUpdate(StringHash eventType, VariantMap& eventData)
	{
		using namespace Update;
		float timestep = eventData[P_TIMESTEP].GetFloat();

		IntVector2 mouseMovement = input->GetMouseMove();
		yaw_ += MOUSE_SENSITIVITY * mouseMovement.x_;
		pitch_ += MOUSE_SENSITIVITY * mouseMovement.y_;
		pitch_ = Clamp(pitch_, -90.0f, 90.0f);

		charNode_->SetRotation(Quaternion(0.0f, yaw_, 0.0f));

		CameraMovement(timestep);
	}

	Node*CreateCharacter()
	{
		Node* charNode = scene_->CreateChild();
		charNode->SetPosition(Vector3(0.0f, 4.0f, 20.0f));

		Node* adjustNode = charNode->CreateChild("AdjNone");
		adjustNode->SetRotation(Quaternion(180, Vector3
		(0.0f, 1.0f, 0.0f)));

		AnimatedModel* charObject = adjustNode->CreateComponent
			<AnimatedModel>();

		charObject->SetModel(cache->GetResource<Model>
			("Models/Mutant/Mutant.mdl"));
		charObject->SetCastShadows(true);

		charObject->GetSkeleton().GetBone(
			"Mutant:Head")->animated_ = false;

		KinematicCharacterController* charController =
			charNode->CreateComponent<KinematicCharacterController>();
		charController->CreatePhysComponents(1.9f, 0.5f);
		return charNode;
	}

	const float MOUSE_SENSITIVITY = 0.1f;

	void HandlePhysicsPreStep(StringHash eventType,
		VariantMap& eventData)
	{
	}

	void HandlePostRenderUpdate(StringHash eventType,
		VariantMap& eventData)
	{
		scene_->GetComponent<PhysicsWorld>()->
			DrawDebugGeometry(true);
	}

	void HandlePostUpdate(StringHash eventType,
		VariantMap& eventData)
	{
		using namespace PostUpdate;
		float timestep = eventData[P_TIMESTEP].GetFloat();
		
		CameraMovement(timestep);
	}

	void CameraMovement(float duration)
	{
		Quaternion rot = charNode_->GetRotation();
		Quaternion dir = rot * Quaternion(pitch_,
			Vector3::RIGHT);

		Node* headNode = charNode_->
			GetChild("Mutant:Head", true);

		Vector3 aimPoint = charNode_->GetPosition()
			+ rot * Vector3(0.0f, 1.9f, 0.0f);
		Vector3 rayDir = dir * Vector3::BACK;
		float rayRange = 5.0f;

		Vector3 firstPersonPos = Vector3(headNode->GetPosition() + rot * 
			Vector3(0.0f, 0.15f, 0.2f));

		Vector3 thirdPersonPos = Vector3(0.0f, 5.0f, 0.0f);

		Vector3 towardsFPS = Vector3(firstPersonPos -
			cameraNode_->GetPosition());

		Vector3 towardsTPS = Vector3(thirdPersonPos -
			cameraNode_->GetPosition());

		float transitionRate = 0.5f; 

		Vector3 targetPos;

		if (firstPerson_)
		{
			Vector3 firstPersonPos = Vector3(headNode->
				GetWorldPosition() + rot *
					Vector3(0.0f, 0.15f, 0.2f));

					targetPos = firstPersonPos;
		}

		else
		{
			PhysicsRaycastResult result;
			scene_->GetComponent<PhysicsWorld>()->
				RaycastSingle(result, Ray(aimPoint,
					rayDir), rayRange, 2);

			if (result.body_)
				rayRange = Min(rayRange,
					result.distance_);
			rayRange - Clamp(rayRange, 1.0f, 5.0f);


			targetPos = aimPoint + rayDir * rayRange;
		}

		Vector3 currentCameraPos = cameraNode_->GetPosition();

		if (camTransitionTime >= 1.0) {
			cameraNode_->SetPosition(targetPos);
			
		}
		else {
			camTransitionTime += 5.0 * duration;
			Vector3 lerpedPos = cameraPos.Lerp(targetPos, camTransitionTime);
			cameraNode_->SetPosition(lerpedPos);
		}
		
		cameraNode_->SetRotation(dir);
	}

};
		
		URHO3D_DEFINE_APPLICATION_MAIN(KinematicCharacterScene)
