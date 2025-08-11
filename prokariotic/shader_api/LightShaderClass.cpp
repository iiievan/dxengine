#include "LightShaderClass.hpp"
#include "utils/Utils.hpp"

LightShaderClass::LightShaderClass()
{
    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
    m_layout = nullptr;
    m_sampleState = nullptr;
    m_matrixBuffer = nullptr;
    m_lightColorBuffer = nullptr;
    m_lightPositionBuffer = nullptr;
}

LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}

LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    wchar_t vsFilename[128];
    wchar_t psFilename[128];
    int error;
    bool result;

    std::wstring vertex_path = findFullPath("light_vs.hlsl");
    std::wstring pixel_path = findFullPath( "light_ps.hlsl");

    // Set the filename of the vertex shader.
    error = wcscpy_s(vsFilename, 128, vertex_path.c_str());
    if (error != 0)
        return false;

    // Set the filename of the pixel shader.
    error = wcscpy_s(psFilename, 128, pixel_path.c_str());
    if (error != 0)
        return false;

    // Initialize the vertex and pixel shaders.
    result = InitializeShader(device, hwnd, vsFilename, psFilename);
    if(!result)
        return false;

    return true;
}


void LightShaderClass::Shutdown()
{
    // Shutdown the vertex and pixel shaders as well as the related objects.
    ShutdownShader();

    return;
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext,
                              int indexCount,
                              XMMATRIX worldMatrix,
                              XMMATRIX viewMatrix,
                              XMMATRIX projectionMatrix,
                              ID3D11ShaderResourceView* texture,
                              XMFLOAT4 diffuseColor[],
                              XMFLOAT4 lightPosition[])
{
    bool result;

    // Set the shader parameters that it will use for rendering.
    result = SetShaderParameters(deviceContext,
                                 worldMatrix,
                                 viewMatrix,
                                 projectionMatrix,
                                 texture,
                                 diffuseColor,
                                 lightPosition);
    if(!result)
        return false;

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount);

    return true;
} 


bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
    HRESULT result;
    ID3DBlob* errorMessage = nullptr;
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;


    // Compile the vertex shader code.
    ID3DBlob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename,
                        NULL,
                        NULL,
                     "LightVertexShader",
                        "vs_5_0",
                         dwShaderFlags,
                         NULL,
                        &vertexShaderBuffer,
                     &errorMessage);
    if(FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if(errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
            ReleaseCOM(errorMessage);
        }
        // If there was nothing in the error message then it simply could not find the shader file itself.
        else
            MessageBox(hwnd, (LPCSTR)vsFilename, "Missing vertex shader file", MB_OK);

        return false;
    }

    // Compile the pixel shader code.
    ID3DBlob* pixelShaderBuffer = nullptr;
    result = D3DCompileFromFile(psFilename,
                        NULL,
                        NULL,
                      "LightPixelShader",
                         "ps_5_0",
                          dwShaderFlags,
                          NULL,
                         &pixelShaderBuffer,
                      &errorMessage);
    if(FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if(errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
            ReleaseCOM(errorMessage);
        }
        // If there was nothing in the error message then it simply could not find the file itself.
        else
            MessageBox(hwnd, (LPCSTR)psFilename, "Missing Shader File", MB_OK);

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


    // Create the vertex input layout description.
    // This setup needs to match the VertexType stucture in the ModelClass and in the shader.
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

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


    // Create a texture sampler state description.
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    if(FAILED(result))
        return false;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    D3D11_BUFFER_DESC matrixBufferDesc;
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

    // Setup the description of the dynamic constant buffer that is in the pixel shader.
    D3D11_BUFFER_DESC lightColorBufferDesc;
    lightColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightColorBufferDesc.ByteWidth = sizeof(LightColorBufferType);
    lightColorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightColorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightColorBufferDesc.MiscFlags = 0;
    lightColorBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
    result = device->CreateBuffer(&lightColorBufferDesc, NULL, &m_lightColorBuffer);
    if(FAILED(result))
        return false;

    // Setup the description of the dynamic constant buffer that is in the vertex shader.
    D3D11_BUFFER_DESC lightPositionBufferDesc;
    lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBufferType);
    lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightPositionBufferDesc.MiscFlags = 0;
    lightPositionBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&lightPositionBufferDesc, NULL, &m_lightPositionBuffer);
    if(FAILED(result))
        return false;

    return true;
}


void LightShaderClass::ShutdownShader()
{
    // Release the light constant buffers.
    if(m_lightColorBuffer)
        ReleaseCOM(m_lightColorBuffer);

    if(m_lightPositionBuffer)
        ReleaseCOM(m_lightPositionBuffer);

    if(m_matrixBuffer)
        ReleaseCOM(m_matrixBuffer);

    if(m_sampleState)
        ReleaseCOM(m_sampleState);

    if(m_layout)
        ReleaseCOM(m_layout);

    if(m_pixelShader)
        ReleaseCOM(m_pixelShader);

    if(m_vertexShader)
        ReleaseCOM(m_vertexShader);

    return;
}

void LightShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    ofstream fout;

     char* compileErrors = (char*)(errorMessage->GetBufferPointer());

    unsigned __int64 bufferSize = errorMessage->GetBufferSize();

    fout.open("shader-error.txt");

    // Write out the error message.
    for(unsigned __int64 i=0; i < bufferSize; i++)
    {
        fout << compileErrors[i];
    }

    fout.close();
    ReleaseCOM(errorMessage);

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", (LPCSTR)shaderFilename, MB_OK);

    return;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
                                           XMMATRIX worldMatrix,
                                           XMMATRIX viewMatrix,
                                           XMMATRIX projectionMatrix,
                                           ID3D11ShaderResourceView* texture,
                                           XMFLOAT4 diffuseColor[],
                                           XMFLOAT4 lightPosition[])
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    unsigned int bufferNumber;

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
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // Set the position of the constant buffer in the vertex shader.
    bufferNumber = 0;

    // Now set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    // Lock the light position constant buffer so it can be written to.
    result = deviceContext->Map(m_lightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
        return false;

    // Get a pointer to the data in the constant buffer.
    LightPositionBufferType *dataPtr2 = (LightPositionBufferType*)mappedResource.pData;

    // Copy the light position variables into the constant buffer.
    dataPtr2->lightPosition[0] = lightPosition[0];
    dataPtr2->lightPosition[1] = lightPosition[1];
    dataPtr2->lightPosition[2] = lightPosition[2];
    dataPtr2->lightPosition[3] = lightPosition[3];

    // Unlock the constant buffer.
    deviceContext->Unmap(m_lightPositionBuffer, 0);

    // Set the position of the constant buffer in the vertex shader.
    bufferNumber = 1;

    // Finally set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightPositionBuffer);

    // Set shader texture resource in the pixel shader.
    deviceContext->PSSetShaderResources(0, 1, &texture);

    // Lock the light color constant buffer so it can be written to.
    result = deviceContext->Map(m_lightColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
        return false;

    // Get a pointer to the data in the constant buffer.
    LightColorBufferType *dataPtr3 = (LightColorBufferType*)mappedResource.pData;

    // Copy the light color variables into the constant buffer.
    dataPtr3->diffuseColor[0] = diffuseColor[0];
    dataPtr3->diffuseColor[1] = diffuseColor[1];
    dataPtr3->diffuseColor[2] = diffuseColor[2];
    dataPtr3->diffuseColor[3] = diffuseColor[3];

    // Unlock the constant buffer.
    deviceContext->Unmap(m_lightColorBuffer, 0);

    // Set the position of the constant buffer in the pixel shader.
    bufferNumber = 0;

    // Finally set the constant buffer in the pixel shader with the updated values.
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightColorBuffer);

    return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // Set the sampler state in the pixel shader.
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);

    // Render the triangle.
    deviceContext->DrawIndexed(indexCount, 0, 0);

    return;
}
