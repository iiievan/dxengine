#ifndef __GDIPLUSMANAGER_H
#define __GDIPLUSMANAGER_H

#include "WinDefs.h"


class GDIPlusManager
{
public:
    GDIPlusManager();
    ~GDIPlusManager();

private:
    static ULONG_PTR m_token;
    static int m_refCount;
};

#endif //__GDIPLUSMANAGER_H
