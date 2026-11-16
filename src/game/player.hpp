#pragma once

#include <vector>

#include "engine/input.hpp"
#include "game/camera_controller.hpp"
#include "game/camera_rig.hpp"
#include "game/player_controller.hpp"
#include "graphics/projection.hpp"
#include "math/vec3.hpp"
#include "view/scene_view.hpp"
#include "view/view_mode.hpp"
#include "world/entity/brain/player_brain.hpp"
#include "world/entity/entity.hpp"
#include "world/world.hpp"

namespace robot::game {

struct PlayerInfo {
    float yaw;
    float pitch;
    Vec3 velocity;
    Vec3 position;
};

/// 使用者在遊玩中操控的一切：機器人（WASD、QE）、六自由度相機（IJKL 等），以及投影方式（F1–F5）。
/// 相機與機器人互相獨立：機器人只依自己的朝向移動，相機只由相機的按鍵控制
class Player {
   public:
    enum class CameraMode {
        Follow,  ///< 相機與機器人的相對位置固定，機器人移動、轉向時相機跟著走
        Free,    ///< 相機留在世界座標中原地，只由相機的按鍵控制
    };

    explicit Player(world::World& world, Vec3 position);
    ~Player() = default;

    PlayerInfo info() const;
    const CameraRig& cameraRig() const { return _cameraRig; }
    CameraMode cameraMode() const { return _cameraMode; }
    view::ViewMode viewMode() const { return _viewMode; }
    /// 平行投影目前看到的半高，公尺；由透視投影的 fovY 與 eye 到 focus 的距離換算
    float orthoHalfHeight() const;

    /// 依目前的投影方式，把視窗分成一或四塊，每塊各自的觀察參數與投影
    std::vector<view::SceneView> views(int width, int height) const;
    /// 透視相機的 view volume；疊在平行投影的畫面上時使用
    graphics::ViewVolume perspectiveVolume(int width, int height) const;

    void update(float dt, const engine::Input& input);
    /// 世界前進一個 tick 之後呼叫：跟隨模式下讓相機跟著機器人移動與轉向
    void follow();
    /// 停止 update 時呼叫，清掉停止期間會失效的操作狀態
    void cancelInput();

   private:
    // 機器人的視線；移動方向以它為準。Q、E 只改 yaw，pitch 維持水平
    float _yaw = 0.0f;
    float _pitch = 0.0f;

    CameraMode _cameraMode = CameraMode::Follow;
    view::ViewMode _viewMode = view::ViewMode::Perspective;

    // 上一次 follow() 時機器人的位置與視線，用來算出這個 tick 的位移與轉向
    Vec3 _lastPosition;
    float _lastYaw = 0.0f;

    CameraRig _cameraRig;
    CameraController _cameraController;
    PlayerController _playerController;

    world::entity::brain::PlayerBrain* _brain = nullptr;
    world::entity::Entity* _entity = nullptr;

    void placeCameraBehind();

    Vec3 orthoCenter() const;
    view::SceneView orthoView(view::ViewMode mode, const graphics::Viewport& viewport) const;
    view::SceneView perspectiveView(const graphics::Viewport& viewport) const;
};

std::string toString(Player::CameraMode mode);

}  // namespace robot::game
