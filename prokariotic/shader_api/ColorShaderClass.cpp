#include "ColorShaderClass.h"
#include "utils/Utils.hpp"

ColorShaderClass::ColorShaderClass()
{
    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
    m_layout = nullptr;
    m_matrixBuffer = nullptr;
}


ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}


ColorShaderClass::~ColorShaderClass()
{
}


bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result;
    wchar_t vsFilename[128];
    wchar_t psFilename[128];
    int error;

    std::wstring vertex_path = findFullPath("vertex.hlsl");
    std::wstring pixel_path = findFullPath( "pixel.hlsl");

    // Set the filename of the vertex shader.
    error = wcscpy_s(vsFilename, 128, vertex_path.c_str());
    if(error != 0)
        return false;

    // Set the filename of the pixel shader.
    error = wcscpy_s(psFilename, 128, pixel_path.c_str());
    if(error != 0)
        return false;


    // Initialize the vertex and pixel shaders.
    result = InitializeShader(device, hwnd, vsFilename, psFilename);
    if(!result)
        return false;

    return true;
}

void ColorShaderClass::Shutdown()
{
    // Shutdown the vertex and pixel shaders as well as the related objects.
    ShutdownShader();

    return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, 
                                              int  indexCount, 
                                         XMMATRIX  worldMatrix, 
                                         XMMATRIX  viewMatrix,
                                         XMMATRIX  projectionMatrix)
{
    bool result;

    // Set the shader parameters that it will use for rendering.
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
    if(!result)
        return false;

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount);

    return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
    HRESULT result;
    ID3DBlob* errorMessage = nullptr;
    ID3DBlob* vertexShaderBuffer = nullptr;
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

    // Compile the vertex shader code.
    result = D3DCompileFromFile(vsFilename,
                                NULL,
                                NULL,
                                "ColorVertexShader",
                                "vs_5_0",
                                dwShaderFlags,
                                NULL,
                                &vertexShaderBuffer,
                                &errorMessage);

    if(FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
            ReleaseCOM(errorMessage);
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else
            MessageBox(hwnd, (LPCSTR)vsFilename, "Missing vertex shader file", MB_OK);

        return false;
    }

    ID3DBlob* pixelShaderBuffer = nullptr;

    // Compile the pixel shader code.
    result = D3DCompileFromFile(psFilename,
                                NULL,
                                NULL,
                                "ColorPixelShader",
                                "ps_5_0",
                                dwShaderFlags,
                                NULL,
                                &pixelShaderBuffer,
                                &errorMessage);

    if(FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
            ReleaseCOM(errorMessage);
        }
        // If there was nothing in the error message then it simply could not find the file itself.
        else
            MessageBox(hwnd, (LPCSTR)psFilename, "Missing pixel shader file", MB_OK);

        return false;
    }

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
                                        vertexShaderBuffer->GetBufferSize(),
                                        NULL,
                                        &m_vertexShader);

    if(FAILED(result))
        return false;

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
                                       pixelShaderBuffer->GetBufferSize(),
                                       NULL,
                                       &m_pixelShader);

    if(FAILED(result))
        return false;



    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

    // Create the vertex input layout description.
    // This setup needs to match the VertexType stucture in the ModelClass and in the shader.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = device->CreateInputLayout(polygonLayout,
                                       numElements,
                                       vertexShaderBuffer->GetBufferPointer(),
                                       vertexShaderBuffer->GetBufferSize(),
                                       &m_layout);
    if(FAILED(result))
        return false;

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    ReleaseCOM(vertexShaderBuffer);
    ReleaseCOM(pixelShaderBuffer);



    D3D11_BUFFER_DESC matrixBufferDesc;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if(FAILED(result))
        return false;

    return true;
}

void ColorShaderClass::ShutdownShader()
{
    if(m_matrixBuffer)
        ReleaseCOM(m_matrixBuffer);

    if(m_layout)
        ReleaseCOM(m_layout);

    if(m_pixelShader)
        ReleaseCOM(m_pixelShader);

    if(m_vertexShader)
        ReleaseCOM(m_vertexShader);

    return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    ofstream fout;
    char*  compileErrors = (char*)(errorMessage->GetBufferPointer());
    unsigned long long bufferSize = errorMessage->GetBufferSize();


    fout.open("shader-error.txt");

    // Write out the error message.
    for(unsigned long long i = 0; i < bufferSize; i++)
        fout << compileErrors[i];

    fout.close();

    ReleaseCOM(errorMessage);

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", (LPCSTR)shaderFilename, MB_OK);

    return;
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
                                           XMMATRIX worldMatrix,
                                           XMMATRIX viewMatrix,
                                           XMMATRIX projectionMatrix)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;


    // Transpose the matrices to prepare them for the shader.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // Lock the constant buffer so it can be written to.
    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if(FAILED(result))
        return false;

    // Get a pointer to the data in the constant buffer.
    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // Copy the matrices into the constant buffer.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // Set the position of the constant buffer in the vertex shader.
    unsigned int bufferNumber = 0;

    // Finanly set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // Render the triangle.
    deviceContext->DrawIndexed(indexCount, 0, 0);

    return;
}