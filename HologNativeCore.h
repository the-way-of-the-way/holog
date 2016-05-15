#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Engine/Application.h>

namespace Urho3D {

    class Node;

    class Scene;

    class Sprite;

    class Texture2D;

    class RenderPath;

}

using namespace Urho3D;

class Hologram : public Application {
OBJECT(Hologram);

private:

    // Pause flag.
    bool _paused;

    bool _autoControls;

protected:

    // Main scene with 4 cameras.
    SharedPtr<Scene> _mainScene;
    // Used for plane rendering.
    SharedPtr<Scene> _screenScene;

    // Camera scene node.
    SharedPtr<Node> _mainCameraNode;
    // Camera scene node.
    SharedPtr<Node> _screenCameraNode;

public:

    Hologram(Context *context);

    // Setup before engine initialization. Modifies the engine parameters.
    virtual void Setup();

    // Setup after engine initialization. Creates the logo, console & debug HUD.
    virtual void Start();

    // Cleanup after the main loop. Called by Application.
    virtual void Stop();

private:

    // Construct the scene content.
    void InitMainScene();
    void InitScreenScene();

    // Handle scene update event to control camera's pitch and yaw for all samples.
    void HandleUpdate(StringHash eventType, VariantMap &eventData);

    // Handle key down event to process key controls common to all samples.
    void HandleKeyDown(StringHash eventType, VariantMap &eventData);

    SharedPtr<Texture2D> CreateSurfaceTexture(const Vector3 &direction, const Vector2 &position);

};