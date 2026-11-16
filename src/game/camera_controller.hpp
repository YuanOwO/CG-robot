#pragma once

#include <optional>

#include "common/point.hpp"
#include "engine/input.hpp"
#include "view/view_mode.hpp"

namespace robot::game {

/// 把相機相關的按鍵與滑鼠讀成這個 tick 要做的操作
class CameraController {
   public:
    struct CameraUpdate {
        // 平移，公尺，沿相機自己的軸
        float surge = 0.0f;  ///< 正值往前
        float sway = 0.0f;   ///< 正值往右
        float heave = 0.0f;  ///< 正值往上

        // 旋轉，度，繞相機自己的軸
        float yaw = 0.0f;    ///< 正值往左轉
        float pitch = 0.0f;  ///< 正值抬頭
        float roll = 0.0f;   ///< 正值往右傾

        float zoom = 1.0f;  ///< 倍數，>1 放大，<1 縮小

        bool reset = false;             ///< 把相機放回機器人後方
        bool toggleFollow = false;      ///< 切換跟隨 / 自由
        std::optional<view::ViewMode> viewMode;  ///< 這個 tick 選擇的投影方式
    };

    CameraUpdate update(float dt, const engine::Input& input);
    // 停止 update 時呼叫：期間游標的位移不屬於這次拖曳，恢復時要重新記錄起點
    void reset() { _dragFrom.reset(); }

   private:
    // 拖曳視角時上一個 tick 的游標位置；沒有在拖曳時為空，按下的那一刻才記錄起點
    std::optional<Point> _dragFrom;
};

}  // namespace robot::game
