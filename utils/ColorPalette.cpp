#include <directxmath.h>
#include "Utils.hpp"

using namespace DirectX;

ColorPalette::ColorPalette()
    : m_Palette {
          XMFLOAT4 {1.0f, 1.0f, 1.0f, 1.0f},    // White
          XMFLOAT4 {0.8f, 0.07f, 0.3f, 1.0f},   // Scarlet
          XMFLOAT4 {0.78f, 0.31f, 0.59f, 1.0f}, // Pink
          XMFLOAT4 {0.63f, 0.31f, 0.78f, 1.0f}, // Purple
          XMFLOAT4 {0.3f, 0.31f, 0.81f, 1.0f},  // Light blue
          XMFLOAT4 {0.3f, 0.57f, 0.81f, 1.0f},  // Blue
          XMFLOAT4 {0.3f, 0.81f, 0.78f, 1.0f},  // Azure
          XMFLOAT4 {0.14f, 0.8f, 0.42f, 1.0f},  // Green
          XMFLOAT4 {0.05f, 0.43f, 0.05f, 1.0f}, // Dark green
          XMFLOAT4 {0.98f, 0.97f, 0.43f, 1.0f}, // Lemon
          XMFLOAT4 {0.86f, 0.74f, 0.12f, 1.0f}, // Amber
          XMFLOAT4 {0.96f, 0.63, 0.01f, 1.0f}   // Orange
      }
{
}

XMFLOAT4 ColorPalette::GetColor(DEBUG_COLORS color)
{
    XMFLOAT4 result = m_Palette[color];
    return result;
}

ColorPalette color_palette;
