#include "HologNativeCore.h"
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Engine/EngineEvents.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/RenderPath.h>

DEFINE_APPLICATION_MAIN(Hologram)

Hologram::Hologram(Context *context) :
        Application(context), _paused(false) {
}

void Hologram::Setup() {
    // Modify engine startup parameters.
    engineParameters_["WindowTitle"] = GetTypeName();
    engineParameters_["LogName"] =
            GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_["FullScreen"] = false;
    engineParameters_["Headless"] = false;

    _autoControls = true;
}

void Hologram::Start() {
    // Create the scene content.
    InitMainScene();
    InitScreenScene();

    // Setup the viewport for displaying the scene
    Renderer *renderer = GetSubsystem<Renderer>();

    Zone *zone = renderer->GetDefaultZone();
    zone->SetFogColor(Color::BLACK); // Set background color for the scene

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, _screenScene, _screenCameraNode->GetComponent<Camera>()));

    renderer->SetViewport(0, viewport);

    // Subscribe function for processing update events
    SubscribeToEvent(E_UPDATE, HANDLER(Hologram, HandleUpdate));
    // Subscribe key down event
    SubscribeToEvent(E_KEYDOWN, HANDLER(Hologram, HandleKeyDown));
}

SharedPtr<Texture2D> Hologram::CreateSurfaceTexture(const Vector3 &direction, const Vector2 &position) {
// Create a renderable texture (1024x768, RGB format), enable bilinear filtering on it
    SharedPtr<Texture2D> renderTexture(new Texture2D(context_));
    renderTexture->SetSize(768, 768, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
    renderTexture->SetFilterMode(FILTER_BILINEAR);

    Node *cameraNode = _mainCameraNode->CreateChild("Camera");
    // Set camera's position
    cameraNode->SetPosition(Vector3(position.x_, 0.0f, position.y_));
    cameraNode->SetDirection(-direction);
    Camera *camera = cameraNode->CreateComponent<Camera>();
    camera->SetAspectRatio(1.f);
    camera->SetNearClip(0.1f);
    camera->SetFarClip(20.f);

    // Get the texture's RenderSurface object (exists when the texture has been created in rendertarget mode)
    // and define the viewport for rendering the second scene, similarly as how backbuffer viewports are defined
    // to the Renderer subsystem. By default the texture viewport will be updated when the texture is visible
    // in the main view
    RenderSurface *surface = renderTexture->GetRenderSurface();
    SharedPtr<Viewport> rttViewport(new Viewport(context_, _mainScene, camera));
    surface->SetViewport(0, rttViewport);

    return renderTexture;
}

void Hologram::Stop() {
    engine_->DumpResources(true);
}

void Hologram::InitMainScene() {
    _mainScene = new Scene(context_);
    _mainScene->CreateComponent<Octree>();

    // Create camera node.
    _mainCameraNode = _mainScene->CreateChild("Camera");
    _mainCameraNode->SetPosition(Vector3::ZERO);

    ResourceCache *cache = GetSubsystem<ResourceCache>();

    Node *lightNodes = _mainScene->CreateChild("Light");

    Node *lightNode1 = lightNodes->CreateChild("DirectionalLight1");
    lightNode1->SetDirection(Vector3(1.f, -1.f, 1.f));
    Light *light1 = lightNode1->CreateComponent<Light>();
    light1->SetLightType(LIGHT_DIRECTIONAL);
    light1->SetColor(Color(.5f, .5f, .5f));

    Node *lightNode2 = lightNodes->CreateChild("DirectionalLight2");
    lightNode2->SetDirection(Vector3(-1.f, -1.f, 1.f));
    Light *light2 = lightNode2->CreateComponent<Light>();
    light2->SetLightType(LIGHT_DIRECTIONAL);
    light2->SetColor(Color(.5f, .5f, .5f));

    Node *lightNode3 = lightNodes->CreateChild("DirectionalLight3");
    lightNode3->SetDirection(Vector3(1.f, -1.f, -1.f));
    Light *light3 = lightNode3->CreateComponent<Light>();
    light3->SetLightType(LIGHT_DIRECTIONAL);
    light3->SetColor(Color(.5f, .5f, .5f));

    Node *lightNode4 = lightNodes->CreateChild("DirectionalLight4");
    lightNode4->SetDirection(Vector3(-1.f, -1.f, -1.f));
    Light *light4 = lightNode4->CreateComponent<Light>();
    light4->SetLightType(LIGHT_DIRECTIONAL);
    light4->SetColor(Color(.5f, .5f, .5f));

    Node *ninjaNode = _mainScene->CreateChild("Ninja");
    ninjaNode->SetPosition(Vector3::ZERO);
    StaticModel *ninjaObject = ninjaNode->CreateComponent<StaticModel>();
    ninjaObject->SetModel(cache->GetResource<Model>("Models/Ninja.mdl"));
    ninjaObject->SetMaterial(cache->GetResource<Material>("Materials/Ninja.xml"));
}

void Hologram::InitScreenScene() {
    _screenScene = new Scene(context_);
    _screenScene->CreateComponent<Octree>();

    // Create camera node
    _screenCameraNode = _screenScene->CreateChild("Camera");
    // Set camera's position
    _screenCameraNode->SetPosition(Vector3(.0f, 1.f, .0f));
    _screenCameraNode->SetDirection(Vector3::DOWN);
    Camera *camera = _screenCameraNode->CreateComponent<Camera>();
    camera->SetOrthographic(true);
    camera->SetOrthoSize(1.f);

    ResourceCache *cache = GetSubsystem<ResourceCache>();

    Node *screenNode = _screenScene->CreateChild("Screen");
    screenNode->SetPosition(Vector3::ZERO);
    StaticModel *screenObject = screenNode->CreateComponent<StaticModel>();
    screenObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));

    SharedPtr<Material> renderMaterial(new Material(context_));
    renderMaterial->SetTechnique(0, cache->GetResource<Technique>("Techniques/DividedScreen.xml"));
    // Bottom
    renderMaterial->SetTexture(TU_DIFFUSE, CreateSurfaceTexture(Vector3::FORWARD, Vector2(0.f, 8.f)));
    // Left
    renderMaterial->SetTexture(TU_NORMAL, CreateSurfaceTexture(Vector3::RIGHT, Vector2(8.f, 0.f)));
    // Top
    renderMaterial->SetTexture(TU_SPECULAR, CreateSurfaceTexture(Vector3::BACK, Vector2(0.f, -8.f)));
    // Right
    renderMaterial->SetTexture(TU_EMISSIVE, CreateSurfaceTexture(Vector3::LEFT, Vector2(-8.f, 0.f)));

    screenObject->SetMaterial(renderMaterial);

    _mainCameraNode->SetPosition(Vector3(0.f, .5f, 0.f));
}

void Hologram::HandleKeyDown(StringHash eventType, VariantMap &eventData) {
    int key = eventData[KeyDown::P_KEY].GetInt();
    // Close console (if open) or exit when ESC is pressed
    if (key == KEY_ESC) {
        Console *console = GetSubsystem<Console>();
        if (console->IsVisible()) {
            console->SetVisible(false);
        } else {
            engine_->Exit();
        }
    }
}

void Hologram::HandleUpdate(StringHash eventType, VariantMap &eventData) {
    using namespace Update;

    // Movement speed as world units per second.
    const float MOVE_SPEED = 4.f;
    const float ROTATE_SPEED = 40.f;

    // Take the frame time step, which is stored as a float.
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    if (_autoControls) {
        _mainCameraNode->Rotate(Quaternion(ROTATE_SPEED * timeStep, Vector3::DOWN));
    } else {
        Vector3 translation = Vector3::ZERO;
        Quaternion rotation = Quaternion::IDENTITY;

        Input *input = GetSubsystem<Input>();
        // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
        if (input->GetKeyDown('W')) {
            translation = Vector3::UP * MOVE_SPEED * timeStep;
        }
        if (input->GetKeyDown('S')) {
            translation = Vector3::DOWN * MOVE_SPEED * timeStep;
        }
        if (input->GetKeyDown('A')) {
            rotation = Quaternion(ROTATE_SPEED * timeStep, Vector3::DOWN);
        }
        if (input->GetKeyDown('D')) {
            rotation = Quaternion(ROTATE_SPEED * timeStep, Vector3::UP);
        }

        if (input->GetKeyDown('Q')) {
            Vector<SharedPtr<Node> > childCameras = _mainCameraNode->GetChildren();
            for (int i = 0; i < childCameras.Size(); ++i) {
                childCameras[i]->Translate(-childCameras[i]->GetDirection() * MOVE_SPEED * timeStep, TS_PARENT);
            }
        }
        if (input->GetKeyDown('E')) {
            Vector<SharedPtr<Node> > childCameras = _mainCameraNode->GetChildren();
            for (int i = 0; i < childCameras.Size(); ++i) {
                childCameras[i]->Translate(childCameras[i]->GetDirection() * MOVE_SPEED * timeStep, TS_PARENT);
            }
        }

        _mainCameraNode->Translate(translation);
        _mainCameraNode->Rotate(rotation);
    }
}