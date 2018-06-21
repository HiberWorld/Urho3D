#pragma once

#include "Sample.h"

namespace Urho3D
{
	class Node; 
	class Scene; 
}

class CharacterController; 
class Touch; 

class DemoScene : public Sample
{
	URHO3D_OBJECT(DemoScene, Sample); 

public: 

	explicit DemoScene(Context* context); 
	
	~DemoScene() override; 

	void Start() override; 


private: 
	void CreateScene(); 

	void CreateAvatar(); 

	void SubscribeToEvents(); 

	void HandleUpdate(StringHash eventType,
		VariantMap& eventData); 

	void HandlePostUpdate(StringHash eventType,
		VariantMap& eventData); 

	SharedPtr<Touch> touch; 

	WeakPtr<CharacterController> character; 

	bool firstPerson_; 
};