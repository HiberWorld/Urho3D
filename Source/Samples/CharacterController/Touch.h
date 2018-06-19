
#pragma once

#include <Urho3D/Core/Object.h>

using namespace Urho3D;

namespace Urho3D
{
    class Controls;
}

const float CAMERA_MIN_DIST = 1.0f;
const float CAMERA_INITIAL_DIST = 5.0f;
const float CAMERA_MAX_DIST = 20.0f;

class Touch : public Object
{
    URHO3D_OBJECT(Touch, Object);

public:

    Touch(Context* context, float touchSensitivity);

    ~Touch() override;


    void UpdateTouches(Controls& controls);


    float touchSensitivity_;

    float cameraDistance_;

    bool zoom_;

    bool useGyroscope_;
};

