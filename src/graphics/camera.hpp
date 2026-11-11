#pragma once

#include "math/vec3.hpp"

namespace robot::graphics {

/// 觀察參數：從 eye 看向 focus，up 決定畫面的上方。由這三者建立眼睛座標系，對應 gluLookAt 的三組參數
struct Camera {
    Vec3 eye{45.0f, 70.0f, 55.0f};
    Vec3 focus{25.0f, 0.0f, 0.0f};
    Vec3 up{0.0f, 1.0f, 0.0f};
};

}  // namespace robot::graphics
