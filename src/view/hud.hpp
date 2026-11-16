#pragma once

#include <string>
#include <vector>

#include "graphics/2d/canvas.hpp"
#include "math/vec3.hpp"
#include "view/scene_view.hpp"
#include "view/view_mode.hpp"

namespace robot::view {

// HUD 要顯示的數值，由呼叫的一方填好
struct HudInfo {
    double framesPerSecond;
    double ticksPerSecond;
    ViewMode viewMode;
    std::string cameraMode;
    Vec3 cameraEye;
    Vec3 cameraFocus;
    Vec3 cameraUp;
    float fovY;             ///< 透視投影的垂直視角，度
    float orthoHalfHeight;  ///< 平行投影看到的半高，公尺
    float playerYaw;
    Vec3 playerPosition;
    Vec3 playerVelocity;

    // 每一塊畫面的位置與名稱，用來畫分隔線與標籤
    std::vector<SceneView> views;
    int windowWidth;
    int windowHeight;
};

// 遊玩時疊在場景上的資訊。
class Hud {
   public:
    void render(graphics::Canvas& canvas, const HudInfo& info) const;
};

}  // namespace robot::view
