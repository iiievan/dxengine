#ifndef __GRAPHICSTHROWMACROSES_H
#define __GRAPHICSTHROWMACROSES_H

// Attention!!!! HRESULT hr should exist in the local scope for these macros to work.

#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) \
if (FAILED(hr = (hrcall)))   \
throw Graphics::HrException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr), m_infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) \
m_infoManager.Set();         \
if (FAILED(hr = (hrcall))) \
throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) \
Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), m_infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call)                                 \
m_infoManager.Set();                                            \
(call);                                                       \
{                                                             \
auto v = m_infoManager.GetMessages();                       \
if (!v.empty())                                           \
{                                                         \
throw Graphics::InfoException(__LINE__, __FILE__, v); \
}                                                         \
}
#else
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// macro for importing infomanager into local scope
// this.GetInfoManager() must exist
#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr
#else
#define INFOMAN(gfx) HRESULT hr; DxgiInfoManager& m_infoManager = GetInfoManager(gfx)
#endif

#endif //__GRAPHICSTHROWMACROSES_H
