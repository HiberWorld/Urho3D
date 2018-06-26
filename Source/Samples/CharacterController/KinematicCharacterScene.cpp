#include "Global.h"
#include "KinematicCharacterController.h"

class KinematicCharacterScene : public Application
{
	URHO3D_OBJECT(KinematicCharacterScene, Application);

public:
	SharedPtr<Scene> scene_;
	SharedPtr<Node> cameraNode_;
	float yaw_ = 0.0f; 
	float pitch_ = 0.0f; 

	Node* charNode_ = nullptr; 

	KinematicCharacterScene(Context* context) :
		Application(context)
	{
		SubscribeToEvent(E_BEGINFRAME,
			URHO3D_HANDLER(KinematicCharacterScene,ApplyGameState)); 

		KinematicCharacterController::RegisterObject(
			context_);
	}

	void Setup()
	{
		engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
		engineParameters_[EP_FULL_SCREEN] = false; 
		engineParameters_[EP_WINDOW_WIDTH] = 800; 
		engineParameters_[EP_WINDOW_HEIGHT] = 600; 
		engineParameters_[EP_RESOURCE_PATHS] = "GameData;Data;CoreData";
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
		/*scene_ = new Scene(context_);
		File levelFile(context_, fileSystem->)*/

		cameraNode_ = scene_->CreateChild("Camera"); 
		cameraNode_->CreateComponent<Camera>();
		cameraNode_->SetPosition(Vector3(0.0f, 5.0f, 0.0f)); 

		charNode_ = CreateCharacter();
	}

	void MoveCamera(float timeStep)
	{
		if (ui->GetFocusElement())
			return;
	}
	
	void SubscribeToEvents()
	{
		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER
		(KinematicCharacterScene, HandleUpdate));
		SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER
		(KinematicCharacterScene, HandlePhysicsPreStep));
		SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER
		(KinematicCharacterScene, HandlePostRenderUpdate));
		SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(
			KinematicCharacterScene, HandlePostUpdate));
	}

	void HandleUpdate(StringHash eventType, VariantMap& eventData)
	{
		using namespace Update; 
		float timestep = eventData[P_TIMESTEP].GetFloat();

		if (input->GetKeyPress(KEY_F1))
			console->Toggle();

		if (input->GetKeyPress(KEY_F2))
			debugHud->ToggleAll();

		IntVector2 mouseMovement = input->GetMouseMove(); 
		yaw_ += MOUSE_SENSITIVITY * mouseMovement.x_;
		pitch_ += MOUSE_SENSITIVITY * mouseMovement.y_;
		pitch_ = Clamp(pitch_, -90.0f, 90.0f); 
	}
	Node*CreateCharacter()
	{

	}
	const float MOUSE_SENSITIVITY = 0.1f;
};