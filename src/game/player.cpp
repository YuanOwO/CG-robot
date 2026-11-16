#include "game/player.hpp"

#include <cmath>
#include <memory>
#include <stdexcept>
#include <utility>

#include "math/utils.hpp"
#include "world/entity/robot.hpp"

namespace robot::game {

namespace {

// 重設時相機在機器人眼睛後方多遠、往下看幾度
const float CAMERA_DISTANCE = 3.0f;
const float CAMERA_PITCH = 20.0f;

// 平行投影只畫畫面中心（相機的 focus）前後這個距離內的東西；太遠的地形會擋住機器人，所以切掉，畫面就像剖面圖
const float ORTHO_DEPTH = 40.0f;

}  // namespace

std::string toString(Player::CameraMode mode) {
    switch (mode) {
    case Player::CameraMode::Follow:
        return "Follow";
    case Player::CameraMode::Free:
        return "Free";
    default:
        return "Unknown";
    }
}

Player::Player(world::World& world, Vec3 position) {
    auto brain = std::make_unique<world::entity::brain::PlayerBrain>();
    _brain = brain.get();
    _entity = &world.entities().spawn<world::entity::Robot>(position, std::move(brain));

    _yaw = _entity->look().yaw;
    _pitch = _entity->look().pitch;
    _lastPosition = _entity->position();
    _lastYaw = _yaw;
    placeCameraBehind();
}

PlayerInfo Player::info() const {
    return PlayerInfo{_entity->look().yaw, _entity->look().pitch, _entity->velocity(), _entity->position()};
}

float Player::orthoHalfHeight() const {
    // 與透視投影共用 fovY：平行投影的半高取透視投影在 focus 處看到的半高，兩種投影在 focus 處看到的範圍相同，
    // 縮放時也一起變化
    const float focusDistance = distance(_cameraRig.eye(), _cameraRig.focus());
    return focusDistance * std::tan(radians(_cameraRig.projection().fovY) / 2.0f);
}

void Player::update(float dt, const engine::Input& input) {
    const auto camera = _cameraController.update(dt, input);

    if (camera.viewMode) {
        _viewMode = *camera.viewMode;
    }
    if (camera.toggleFollow) {
        _cameraMode = _cameraMode == CameraMode::Follow ? CameraMode::Free : CameraMode::Follow;
    }

    if (camera.reset) {
        placeCameraBehind();
    } else {
        _cameraRig.move(camera.surge, camera.sway, camera.heave);
        _cameraRig.rotate(camera.yaw, camera.pitch, camera.roll);
    }
    _cameraRig.zoom(camera.zoom);

    _yaw = wrapDegrees(_yaw + _playerController.turn(dt, input));

    auto intent = _playerController.read(input);
    intent.look = {_yaw, _pitch};
    if (_brain) {
        _brain->setIntent(intent);
    }
}

void Player::follow() {
    const Vec3 position = _entity->position();
    const float yaw = _entity->look().yaw;

    if (_cameraMode == CameraMode::Follow) {
        _cameraRig.carry(position - _lastPosition, position, wrapSignedDegrees(yaw - _lastYaw));
    }

    _lastPosition = position;
    _lastYaw = yaw;
}

void Player::cancelInput() {
    _cameraController.reset();
}

void Player::placeCameraBehind() {
    const Vec3 ahead = rotateAroundY(Vec3(0.0f, 0.0f, 1.0f), _entity->look().yaw);
    const float pitch = radians(CAMERA_PITCH);
    const Vec3 sight = ahead * std::cos(pitch) - Vec3(0.0f, std::sin(pitch), 0.0f);

    // F 放在機器人的眼睛，E 沿視線往後退；Vup 取世界的上方
    const Vec3 focus = _entity->eyePosition();
    _cameraRig.place(focus - sight * CAMERA_DISTANCE, focus, Vec3(0.0f, 1.0f, 0.0f));
}

std::vector<view::SceneView> Player::views(int width, int height) const {
    const graphics::Viewport full{0, 0, width, height};

    switch (_viewMode) {
    case view::ViewMode::OrthoX:
    case view::ViewMode::OrthoY:
    case view::ViewMode::OrthoZ:
        return {orthoView(_viewMode, full)};
    case view::ViewMode::Perspective:
        return {perspectiveView(full)};
    case view::ViewMode::Quad: {
        // 照工程三視圖排列：俯視在前視上方，側視在前視右方，剩下的右上角放透視
        const int halfWidth = width / 2;
        const int halfHeight = height / 2;
        const graphics::Viewport topLeft{0, halfHeight, halfWidth, height - halfHeight};
        const graphics::Viewport topRight{halfWidth, halfHeight, width - halfWidth, height - halfHeight};
        const graphics::Viewport bottomLeft{0, 0, halfWidth, halfHeight};
        const graphics::Viewport bottomRight{halfWidth, 0, width - halfWidth, halfHeight};
        return {
            orthoView(view::ViewMode::OrthoY, topLeft),
            perspectiveView(topRight),
            orthoView(view::ViewMode::OrthoZ, bottomLeft),
            orthoView(view::ViewMode::OrthoX, bottomRight),
        };
    }
    default:
        throw std::runtime_error("Unknown view mode");
    }
}

graphics::ViewVolume Player::perspectiveVolume(int width, int height) const {
    // 四格時透視那一格的寬高比與整個視窗幾乎相同，直接用視窗的
    const graphics::Viewport full{0, 0, width, height};
    return _cameraRig.projection().volume(full.aspect());
}

Vec3 Player::orthoCenter() const {
    // 跟著相機的 focus：平移、旋轉相機時三個平行投影一起移動；按 R 之後 focus 正好是機器人的眼睛
    return _cameraRig.camera().focus;
}

view::SceneView Player::orthoView(view::ViewMode mode, const graphics::Viewport& viewport) const {
    // 沿某一軸看時，從該軸的正方向看向負方向
    Vec3 axis;
    Vec3 up(0.0f, 1.0f, 0.0f);
    std::string label;
    switch (mode) {
    case view::ViewMode::OrthoX:
        axis = Vec3(1.0f, 0.0f, 0.0f);
        label = "Ortho X";
        break;
    case view::ViewMode::OrthoY:
        axis = Vec3(0.0f, 1.0f, 0.0f);
        up = Vec3(0.0f, 0.0f, -1.0f);  // 從上往下看時 y 軸與視線平行，改以 -z 為畫面上方，x 朝右
        label = "Ortho Y";
        break;
    case view::ViewMode::OrthoZ:
        axis = Vec3(0.0f, 0.0f, 1.0f);
        label = "Ortho Z";
        break;
    default:
        throw std::invalid_argument("Not an orthographic view mode");
    }

    graphics::Camera camera;
    camera.focus = orthoCenter();
    camera.eye = camera.focus + axis;
    camera.up = up;

    graphics::OrthoProjection projection;
    projection.halfHeight = orthoHalfHeight();
    projection.nearPlane = 1.0f - ORTHO_DEPTH;  // 眼睛在中心前方 1 公尺，前後範圍以中心為準
    projection.farPlane = 1.0f + ORTHO_DEPTH;

    return view::SceneView{viewport, camera, projection, label, true};
}

view::SceneView Player::perspectiveView(const graphics::Viewport& viewport) const {
    return view::SceneView{viewport, _cameraRig.camera(), _cameraRig.projection(), "F4 Perspective", false};
}

}  // namespace robot::game
