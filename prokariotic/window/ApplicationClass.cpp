#include "ApplicationClass.hpp"
#include "utils/Utils.hpp"


ApplicationClass::ApplicationClass() {
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_Model = nullptr;
    m_LightShader = nullptr;
    m_Light = nullptr;
}

ApplicationClass::ApplicationClass(const ApplicationClass &other) {
}

ApplicationClass::~ApplicationClass() {
}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) {

    bool result;

    // Create and initialize the Direct3D object.
    m_Direct3D = new D3DClass;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);
        return false;
    }

    // Create the camera object.
    m_Camera = new CameraClass;

    // Set the initial position of the camera.
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

    // Create and initialize the model object.
    m_Model = new ModelClass;

    std::wstring model_path = findFullPath("Cube.txt");
    std::string modelFilename = WStringToUTF8(model_path);
    std::wstring texture_path = findFullPath("stone01.tga");
    std::string textureFilename = WStringToUTF8(texture_path);

    result = m_Model->Initialize(m_Direct3D->GetDevice(),
                                 m_Direct3D->GetDeviceContext(),
                                 (char *)modelFilename.c_str(),
                                 (char *)textureFilename.c_str());
    if (!result)
    {
        MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
        return false;
    }

    // Create and initialize the light shader object.
    m_LightShader = new LightShaderClass;

    result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, "Could not initialize the light shader object.", "Error", MB_OK);
        return false;
    }

    // Create and initialize the light object.
    m_Light = new LightClass;

    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetDirection(0.0f, 0.0f, 1.0f);

    return true;
}

void ApplicationClass::Shutdown() {

    if(m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    if(m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
    }

    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = nullptr;
    }

    if (m_Camera)
    {
        delete m_Camera;
        m_Camera = nullptr;
    }

    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = nullptr;
    }
}

bool ApplicationClass::Frame() {
    static float rotation = 0.0f;
    bool result;


    // Update the rotation variable each frame.
    rotation -= 0.0174532925f * 0.6f;
    if(rotation < 0.0f)
    {
        rotation += 360.0f;
    }

    result = Render(rotation);
    if (!result)
        return false;

    return true;
}

bool ApplicationClass::Render(float rotation)
{
    XMMATRIX viewMatrix, projectionMatrix;
    bool result;

    float angleOrbit = rotation;
    float angleLocal = angleOrbit * 2.0f;
    const float orbitRadius = 2.5f;
    const float localRadius = 3.0f;
    XMMATRIX WorldMatrixCube1 = XMMatrixIdentity();
    XMMATRIX WorldMatrixCube1_save;


    // Clear the buffers to begin the scene.
    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Get the world, view, and projection matrices from the camera and d3d objects.
    //m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    XMMATRIX scaleMatrixCube1 = XMMatrixScaling(0.5f, 0.5f, 0.5f);
    XMMATRIX orbitRotation = XMMatrixRotationZ(angleOrbit);
    XMMATRIX translateToOrbit = XMMatrixTranslation(orbitRadius, 0.0f, 0.0f);

    // Multiply them together to create the final world transformation matrix.
    WorldMatrixCube1_save = scaleMatrixCube1 * translateToOrbit * orbitRotation;

    XMMATRIX LocalRotationCube1 = XMMatrixRotationZ(angleOrbit*3.0f);
    WorldMatrixCube1 = scaleMatrixCube1 * LocalRotationCube1 * translateToOrbit * orbitRotation;

    // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
    m_Model->Render(m_Direct3D->GetDeviceContext());

    // Render the model using the light shader.
    result = m_LightShader->Render(m_Direct3D->GetDeviceContext(),
                                   m_Model->GetIndexCount(),
                                   WorldMatrixCube1,
                                   viewMatrix,
                                   projectionMatrix,
                                   m_Model->GetTexture(),
                                   m_Light->GetDirection(),
                                   m_Light->GetDiffuseColor());
    if(!result)
        return false;

    XMMATRIX WorldMatrixCube2 = XMMatrixIdentity();
    XMMATRIX scaleMatrixCube2 = XMMatrixScaling(0.5f, 0.5f, 0.5f);
    XMMATRIX localOrbitMatrix = XMMatrixRotationZ(angleLocal);
    XMMATRIX localRotation = XMMatrixRotationY(angleLocal * 4.0f);
    XMMATRIX translateToLocalOrbit = XMMatrixTranslation(localRadius, 0.0f, 0.0f);
    XMMATRIX localTransform = scaleMatrixCube2 * localRotation * translateToLocalOrbit *  localOrbitMatrix;

    // Multiply them together to create the final world transformation matrix.
    WorldMatrixCube2 = localTransform * WorldMatrixCube1_save;

    // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
    m_Model->Render(m_Direct3D->GetDeviceContext());

    // Render the model using the light shader.
    result = m_LightShader->Render(m_Direct3D->GetDeviceContext(),
                                   m_Model->GetIndexCount(),
                                   WorldMatrixCube2,
                                   viewMatrix,
                                   projectionMatrix,
                                   m_Model->GetTexture(),
                                   m_Light->GetDirection(),
                                   m_Light->GetDiffuseColor());
    if(!result)
        return false;

    // Present the rendered scene to the screen.
    m_Direct3D->EndScene();

    return true;
}
