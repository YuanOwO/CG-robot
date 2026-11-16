#pragma once

#include <string>

namespace robot::view {

/// 作業要求的五種投影方式
enum class ViewMode {
    OrthoX,       ///< 沿 x 軸的平行投影（側視）
    OrthoY,       ///< 沿 y 軸的平行投影（俯視）
    OrthoZ,       ///< 沿 z 軸的平行投影（前視）
    Perspective,  ///< 透視投影，看到的是使用者操控的相機
    Quad,         ///< 四個 viewport：三個平行投影加一個透視投影
};

std::string toString(ViewMode mode);

}  // namespace robot::view
