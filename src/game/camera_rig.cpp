#include "game/camera_rig.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "math/utils.hpp"

namespace robot::game {

namespace {

// 視線與上方向 (0, 1, 0) 平行時 gluLookAt 無法決定畫面的左右，因此不能剛好朝正上下方
const float MIN_PITCH = -89.9f;
const float MAX_PITCH = 89.9f;

const float MIN_ZOOM = 0.5f;
const float MAX_ZOOM = 25.0f;

const float BASE_FOVY = 60.0f;

}  // namespace

void CameraRig::setYaw(float degree) {
    _yaw = wrapDegrees(degree);
}

void CameraRig::setPitch(float degree) {
    _pitch = std::clamp(degree, MIN_PITCH, MAX_PITCH);
}

void CameraRig::setDistance(float distance) {
    _distance = std::max(distance, 0.0f);
}

void CameraRig::turn(float degree) {
    _yaw = wrapDegrees(_yaw + degree);
}

void CameraRig::tilt(float degree) {
    _pitch = std::clamp(_pitch + degree, MIN_PITCH, MAX_PITCH);
}

void CameraRig::zoom(float factor) {
    _zoom = std::clamp(_zoom * factor, MIN_ZOOM, MAX_ZOOM);
}

void CameraRig::setProjectionMode(graphics::ProjectionMode mode) {
    _mode = mode;
}

void CameraRig::changeProjectionMode() {
    if (_mode == graphics::ProjectionMode::Perspective) {
        _mode = graphics::ProjectionMode::Ortho;
    } else {
        _mode = graphics::ProjectionMode::Perspective;
    }
}

graphics::Camera CameraRig::camera() const {
    // (0, 0, -1) 先依 pitch 抬頭或低頭，再繞 y 軸轉 yaw
    const Vec3 forward = rotateAroundY(rotateAroundX(Vec3(0.0f, 0.0f, -1.0f), _pitch), _yaw);

    graphics::Camera camera;
    camera.focus = _target;
    camera.eye = camera.focus - forward * _distance;
    return camera;
}

graphics::Projection CameraRig::projection() const {
    float fovY = BASE_FOVY / _zoom;

    if (_mode == graphics::ProjectionMode::Ortho) {
        graphics::OrthoProjection projection;
        projection.halfHeight = _distance * std::tan(radians(fovY) / 2.0f);
        return projection;
    } else if (_mode == graphics::ProjectionMode::Perspective) {
        graphics::PerspectiveProjection projection;
        projection.fovY = fovY;
        return projection;
    } else {
        throw std::runtime_error("Unknown projection mode");
    }
}

}  // namespace robot::game
