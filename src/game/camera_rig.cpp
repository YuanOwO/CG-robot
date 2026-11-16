#include "game/camera_rig.hpp"

#include <algorithm>

namespace robot::game {

namespace {

const float MIN_ZOOM = 0.5f;
const float MAX_ZOOM = 25.0f;

const float BASE_FOVY = 60.0f;

// 點 point 以 center 為中心、繞 axis 轉 degree：先把 center 移到原點，旋轉後再移回去，即 T(C) R(θ, axis) T(-C)
Vec3 rotateAround(const Vec3& point, const Vec3& center, const Vec3& axis, float degree) {
    return center + rotateAroundAxis(point - center, axis, degree);
}

}  // namespace

void CameraRig::place(const Vec3& eye, const Vec3& focus, const Vec3& up) {
    _eye = eye;
    _focus = focus;
    _up = up;
}

Vec3 CameraRig::w() const {
    return normalize(_eye - _focus);
}

Vec3 CameraRig::u() const {
    return normalize(cross(_up, w()));
}

Vec3 CameraRig::v() const {
    const Vec3 axisW = w();
    return cross(axisW, normalize(cross(_up, axisW)));
}

void CameraRig::move(float surge, float sway, float heave) {
    // E 與 F 一起平移，視線方向不變；Vup 是向量，不受平移影響
    const Vec3 displacement = -w() * surge + u() * sway + v() * heave;
    _eye += displacement;
    _focus += displacement;
}

void CameraRig::rotate(float yaw, float pitch, float roll) {
    // 每個 tick 的角度很小，三種旋轉依序套用的順序不影響結果；每次都用目前的觀察參數重新算軸
    if (pitch != 0.0f) {
        const Vec3 axis = u();
        _focus = rotateAround(_focus, _eye, axis, pitch);
        _up = rotateAroundAxis(_up, axis, pitch);
    }
    if (yaw != 0.0f) {
        const Vec3 axis = v();
        _focus = rotateAround(_focus, _eye, axis, yaw);
        _up = rotateAroundAxis(_up, axis, yaw);
    }
    if (roll != 0.0f) {
        _up = rotateAroundAxis(_up, -w(), roll);
    }

    // 旋轉不改變長度，只是反覆運算會累積浮點誤差
    _up = normalize(_up);
}

void CameraRig::carry(const Vec3& offset, const Vec3& pivot, float yaw) {
    const Vec3 vertical(0.0f, 1.0f, 0.0f);
    _eye = rotateAround(_eye + offset, pivot, vertical, yaw);
    _focus = rotateAround(_focus + offset, pivot, vertical, yaw);
    _up = rotateAroundAxis(_up, vertical, yaw);
}

void CameraRig::zoom(float factor) {
    _zoom = std::clamp(_zoom * factor, MIN_ZOOM, MAX_ZOOM);
}

graphics::Camera CameraRig::camera() const {
    return graphics::Camera{_eye, _focus, _up};
}

graphics::PerspectiveProjection CameraRig::projection() const {
    graphics::PerspectiveProjection projection;
    projection.fovY = BASE_FOVY / _zoom;
    return projection;
}

}  // namespace robot::game
