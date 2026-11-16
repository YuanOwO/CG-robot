#pragma once

namespace robot::graphics {

/// 視窗中的繪圖區域，對應 glViewport：原點在視窗左下角，單位像素
struct Viewport {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    bool empty() const { return width <= 0 || height <= 0; }
    float aspect() const { return empty() ? 1.0f : static_cast<float>(width) / static_cast<float>(height); }
};

}  // namespace robot::graphics
