#pragma once

#include "engine/input.hpp"
#include "world/entity/brain/brain.hpp"

namespace robot::game {

// 把按鍵轉成玩家指令；world 不必知道鍵盤的存在。相機另由 CameraController 控制，與機器人無關
class PlayerController {
   public:
    /// 移動、跳躍與姿勢；視線由 Player 依 turn() 的結果另外填入
    world::entity::brain::Intent read(const engine::Input& input);

    /// 機器人這個 tick 要原地轉多少度，正值往左轉
    float turn(float dt, const engine::Input& input) const;
};

}  // namespace robot::game
