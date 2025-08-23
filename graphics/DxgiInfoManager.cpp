#include "DxgiInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include <dxgidebug.h>
#include <memory>
#include "Utils.hpp"
#include "GraphicsThrowMacroses.h"
#include "WindowThrowMacroses.h"

#pragma comment(lib, "dxguid.lib")

DxgiInfoManager::DxgiInfoManager()
{
    // define function signature of DXGIGetDebugInterface
    typedef HRESULT(WINAPI * DXGIGetDebugInterface)(REFIID, void **);

    // load the dll that contains the function DXGIGetDebugInterface
    const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hModDxgiDebug == nullptr)
        throw CHWND_LAST_EXCEPT();

    // get address of DXGIGetDebugInterface in dll
    const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
        reinterpret_cast<void *>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));
    if (DxgiGetDebugInterface == nullptr)
        throw CHWND_LAST_EXCEPT();

    HRESULT hr;
    GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &m_pDxgiInfoQueue));
}


void DxgiInfoManager::Set() noexcept
{
    // set the index (next) so that the next all to GetMessages()
    // will only get errors generated after this call
    m_next = m_pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::GetMessages() const
{
    std::vector<std::string> messages;
    const auto end = m_pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

    for (auto i = m_next; i < end; i++)
    {
        HRESULT hr;
        SIZE_T  messageLength;
        // get the size of message i in bytes
        GFX_THROW_NOINFO(m_pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));

        // allocate memory for message
        auto bytes = std::make_unique<byte[]>(messageLength);
        auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE *>(bytes.get());

        // get the message and push its description into the vector
        GFX_THROW_NOINFO(m_pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));

        messages.emplace_back(pMessage->pDescription);
    }

    return messages;
}
