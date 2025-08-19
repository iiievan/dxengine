#ifndef __DXGIINFOMANAGER_H
#define __DXGIINFOMANAGER_H

#include "WinDefs.h"
#include <wrl.h>
#include <vector>
#include <string>
#include <dxgidebug.h>

class DxgiInfoManager
{
public:
    DxgiInfoManager();
    ~DxgiInfoManager() = default;
    DxgiInfoManager(const DxgiInfoManager &) = delete;
    DxgiInfoManager &operator=(const DxgiInfoManager &) = delete;

    void Set() noexcept;
    std::vector<std::string> GetMessages() const;

private:
    unsigned long long m_next {0u};
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> m_pDxgiInfoQueue;
};

#endif //__DXGIINFOMANAGER_H
