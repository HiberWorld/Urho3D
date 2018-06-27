#pragma once 
#include <Urho3D\Urho3DAll.h>

class btPairCachingGhostObject; 
class btKinematicCharacterController; 

class KinematicCharacterController : public LogicComponent
{
	URHO3D_OBJECT(KinematicCharacterController, LogicComponent);

public:
	KinematicCharacterController(Context* context);
	virtual ~KinematicCharacterController() override; 
	static void RegisterObject(Context* context); 

	Controls playerControls_; 

	void CreatePhysComponents(float height, float diameter);
	void HandleKeyDown(StringHash, VariantMap & eventData);

	btPairCachingGhostObject* ghostObject_; 
	btKinematicCharacterController* bulletController_; 

	float height_; 
	float diameter_; 

private:
	virtual void Start() override; 
	virtual void FixedUpdate(float timestep) override; 
	virtual void Update(float timestep) override; 
};