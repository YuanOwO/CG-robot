#pragma once

#include "graphics/camera.hpp"
#include "graphics/projection.hpp"
#include "graphics/viewport.hpp"

namespace robot::graphics {

/// 切換到 3D 繪製：以 viewport 為繪圖區域，套用相機與投影，開啟深度測試。
/// 之後的座標為世界座標
/// @param viewport 畫在視窗的哪一塊
/// @param camera 從哪裡看向哪裡
/// @param projection 看得到的範圍；會依 viewport 的寬高比放寬，畫面不會變形
void begin3D(const Viewport& viewport, const Camera& camera, const Projection& projection);

/// 切換到 2D 繪製：以整個視窗為繪圖區域，關閉深度測試。
/// 之後的座標單位為像素，原點在左上角、y 向下
/// @param width 視窗寬度，像素
/// @param height 視窗高度，像素
void begin2D(int width, int height);

}  // namespace robot::graphics
