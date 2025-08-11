#include "ApplicationClass.hpp"
#include "utils/Utils.hpp"

ApplicationClass::ApplicationClass()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    // m_Model = nullptr;
    // m_LightShader = nullptr;
    // m_Light = nullptr;
    m_Arrow = nullptr;
    m_ArrowShader = nullptr;
}

ApplicationClass::ApplicationClass(const ApplicationClass &other) {}

ApplicationClass::~ApplicationClass() {}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result;

    // Create and initialize the Direct3D object.
    m_Direct3D = new D3DClass;

    result =
        m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);
        return false;
    }

    // Create the camera object.
    m_Camera = new CameraClass;

    // Set the initial position of the camera.
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
    /*
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

        m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
        m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_Light->SetDirection(1.0f, 0.0f, 0.0f);
    */
    m_Arrow = new vec4model(XMFLOAT3(-1.0f, -1.0f, 1.0f));
    result = m_Arrow->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());

    if (!result)
    {
        MessageBox(hwnd, "Could not initialize debug vector object.", "Error", MB_OK);
        return false;
    }

    m_ArrowShader = new vec4modelShaderClass();

    result = m_ArrowShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, "Could not initialize the debug vector shader object.", "Error", MB_OK);
        return false;
    }

    return true;
}

void ApplicationClass::Shutdown()
{
    /*
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
    */
    if (m_Arrow)
    {
        delete m_Arrow;
        m_Arrow = nullptr;
    }

    if (m_ArrowShader)
    {
        delete m_ArrowShader;
        m_ArrowShader = nullptr;
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

bool ApplicationClass::Frame()
{
    static float rotation = 0.0f;
    bool         result;

    // Update the rotation variable each frame.
    rotation -= 0.0174532925f * 0.6f;
    if (rotation < 0.0f)
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
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool     result;

    // Clear the buffers to begin the scene.
    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Get the world, view, and projection matrices from the camera and d3d objects.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    // Rotate the world matrix by the rotation value so that the model will spin.
    worldMatrix = XMMatrixRotationY(rotation);

    m_Arrow->setEnd(m_Direct3D->GetDeviceContext(),XMFLOAT3(-1.0f, -1.0f, 1.0f));
    m_Arrow->setColor(m_Direct3D->GetDeviceContext(),DBG_LEMON);
    m_Arrow->Render(m_Direct3D->GetDeviceContext());
    result = m_ArrowShader->Render(m_Direct3D->GetDeviceContext(),m_Arrow->getIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
    if (!result)
        return false;

    // Present the rendered scene to the screen.
    m_Direct3D->EndScene();

    return true;
}
