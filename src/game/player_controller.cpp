#include "game/player_controller.hpp"

namespace robot::game {

namespace {

const float TURN_SPEED = 120.0f;  // 度 / 秒

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

world::entity::brain::Intent PlayerController::read(const engine::Input& input) {
    world::entity::brain::Intent intent;

    intent.surge = axis(input, Key::W, Key::S);
    intent.sway = axis(input, Key::D, Key::A);

    if (input.held(Key::LeftShift)) {
        intent.run = true;
    }
    if (input.held(Key::Space)) {
        intent.jump = true;
    }

    if (input.pressed(Key::Digit1)) {
        intent.stance = world::anatomy::Stance::Standing;
    }
    if (input.pressed(Key::Digit2)) {
        intent.stance = world::anatomy::Stance::Sneaking;
    }
    if (input.pressed(Key::Digit3)) {
        intent.stance = world::anatomy::Stance::Sitting;
    }

    return intent;
}

float PlayerController::turn(float dt, const engine::Input& input) const {
    float direction = 0.0f;
    if (input.held(Key::Q)) {
        direction += 1.0f;
    }
    if (input.held(Key::E)) {
        direction -= 1.0f;
    }
    return direction * TURN_SPEED * dt;
}

}  // namespace robot::game
