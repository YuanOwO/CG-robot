#pragma once

#include "graphics/camera.hpp"
#include "graphics/projection.hpp"
#include "math/vec3.hpp"

namespace robot::game {

/// 第三人稱相機：從目標點沿視線往後退一段距離，轉向時繞著目標點環繞。
/// 相機是遊玩中的操作狀態，不屬於世界；繪製時只取它算出的觀察參數與投影
class CameraRig {
   public:
    void setTarget(const Vec3& target) { _target = target; }

    void setYaw(float degree);
    void setPitch(float degree);
    void setDistance(float distance);

    /// 水平轉向，正值為從上方看逆時針
    void turn(float degree);
    /// 抬頭或低頭，正值抬頭；會停在接近正上方或正下方的地方
    void tilt(float degree);

    /// @param factor 大於 1 拉近、小於 1 拉遠
    void zoom(float factor);

    void setProjectionMode(graphics::ProjectionMode mode);

    void changeProjectionMode();

    /// 度，[0, 360)；為 0 時看向 -z
    float yaw() const { return _yaw; }
    /// 度，正值抬頭
    float pitch() const { return _pitch; }

    graphics::Camera camera() const;
    graphics::Projection projection() const;
    graphics::ProjectionMode projectionMode() const { return _mode; }

   private:
    Vec3 _target;

    // yaw 為 0 時看向 -z，與 OpenGL 眼睛座標的預設方向一致
    float _yaw = 45.0f;
    float _pitch = -20.0f;
    // 公尺，眼睛到目標點的距離。透視投影時越遠畫面越小；平行投影的 view volume 也依它換算，
    // 讓兩種投影在目標點處看到的範圍相同
    float _distance = 3.0f;

    graphics::ProjectionMode _mode = graphics::ProjectionMode::Perspective;

    // 縮放倍率：透視投影縮小視角，平行投影縮小 view volume，眼睛的位置都不變
    float _zoom = 1.0f;
};

}  // namespace robot::game
