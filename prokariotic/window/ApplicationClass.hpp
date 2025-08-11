#ifndef __APPLICATIONCLASS_H_
#define __APPLICATIONCLASS_H_


#include <windows.h>
#include "d3d_api/D3DClass.hpp"
#include "camera/CameraClass.h"
#include "models/ModelClass.h"
#include "shader_api/ColorShaderClass.h"
#include "shader_api/TextureShaderClass.h"
#include "shader_api/LightShaderClass.hpp"
#include "light/LightClass.hpp"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


class ApplicationClass {
public:
    ApplicationClass();

    ApplicationClass(const ApplicationClass &);

    ~ApplicationClass();

    bool Initialize(int, int, HWND);

    void Shutdown();

    bool Frame();

private:
    bool Render(float);

    D3DClass* m_Direct3D;
    CameraClass* m_Camera;
    ModelClass* m_Model;
    LightShaderClass* m_LightShader;
    LightClass* m_Light;
};

#endif	//__APPLICATIONCLASS_H_
