#pragma once

#include <string>

#include "graphics/camera.hpp"
#include "graphics/projection.hpp"
#include "graphics/viewport.hpp"

namespace robot::view {

/// 畫一次場景所需的全部參數：畫在視窗的哪一塊、從哪裡看、用什麼投影
struct SceneView {
    graphics::Viewport viewport;
    graphics::Camera camera;
    graphics::Projection projection;
    std::string label;       ///< 顯示在該區域角落的名稱
    bool showsViewVolume;    ///< 是否疊上透視相機的 view volume；只有平行投影的畫面需要
};

}  // namespace robot::view
