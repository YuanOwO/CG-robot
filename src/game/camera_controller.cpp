#include "game/camera_controller.hpp"

#include <cmath>

namespace robot::game {

namespace {

const float MOVE_SPEED = 4.0f;        // 公尺 / 秒
const float TURN_SPEED = 90.0f;       // 度 / 秒
const float DRAG_SENSITIVITY = 0.3f;  // 度 / 像素
const float ZOOM_SPEED = 4.0f;        // 按住一秒放大或縮小的倍數

// 兩個相反方向的鍵合成 -1、0 或 1；同時按住時互相抵消
float axis(const engine::Input& input, Key positive, Key negative) {
    float value = 0.0f;
    if (input.held(positive)) {
        value += 1.0f;
    }
    if (input.held(negative)) {
        value -= 1.0f;
    }
    return value;
}

}  // namespace

CameraController::CameraUpdate CameraController::update(float dt, const engine::Input& input) {
    CameraUpdate result;

    // 用按住的狀態而不是按下事件：按住多久就動多少，速度與幀率無關
    result.surge = axis(input, Key::I, Key::K) * MOVE_SPEED * dt;
    result.sway = axis(input, Key::L, Key::J) * MOVE_SPEED * dt;
    result.heave = axis(input, Key::U, Key::O) * MOVE_SPEED * dt;

    result.yaw = axis(input, Key::Left, Key::Right) * TURN_SPEED * dt;
    result.pitch = axis(input, Key::Up, Key::Down) * TURN_SPEED * dt;
    result.roll = axis(input, Key::Period, Key::Comma) * TURN_SPEED * dt;

    // 拖曳方向與視線方向相同：往右拖往右看、往上拖往上看
    const auto& mouse = input.mouse();
    if (!mouse.isDown(MouseButton::MouseLeft)) {
        _dragFrom.reset();
    } else {
        if (_dragFrom) {
            const auto delta = mouse.position() - *_dragFrom;
            result.yaw -= static_cast<float>(delta.x) * DRAG_SENSITIVITY;
            result.pitch -= static_cast<float>(delta.y) * DRAG_SENSITIVITY;
        }
        _dragFrom = mouse.position();
    }

    // 以指數變化，不論目前的倍率，縮放的手感都一樣
    result.zoom = std::pow(ZOOM_SPEED, axis(input, Key::Equal, Key::Minus) * dt);

    result.reset = input.pressed(Key::R);
    result.toggleFollow = input.pressed(Key::Tab);

    if (input.pressed(Key::F1)) {
        result.viewMode = view::ViewMode::OrthoX;
    } else if (input.pressed(Key::F2)) {
        result.viewMode = view::ViewMode::OrthoY;
    } else if (input.pressed(Key::F3)) {
        result.viewMode = view::ViewMode::OrthoZ;
    } else if (input.pressed(Key::F4)) {
        result.viewMode = view::ViewMode::Perspective;
    } else if (input.pressed(Key::F5)) {
        result.viewMode = view::ViewMode::Quad;
    }

    return result;
}

}  // namespace robot::game
