#ifndef __DXGIINFOMANAGER_H
#define __DXGIINFOMANAGER_H

#include "WinDefs.h"
#include <vector>
#include <string>

class DxgiInfoManager
{
public:
    DxgiInfoManager();
    ~DxgiInfoManager();
    DxgiInfoManager(const DxgiInfoManager &) = delete;
    DxgiInfoManager &operator=(const DxgiInfoManager &) = delete;

    void Set() noexcept;
    std::vector<std::string> GetMessages() const;

private:
    unsigned long long m_next {0u};
    struct IDXGIInfoQueue* m_pDxgiInfoQueue {nullptr};

};



#endif //__DXGIINFOMANAGER_H
