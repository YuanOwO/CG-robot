#pragma once

#include "graphics/camera.hpp"
#include "graphics/projection.hpp"
#include "math/vec3.hpp"

namespace robot::game {

/// 六個自由度的相機，照課本（Camera Control: Changing View）的做法：
/// 只保存觀察參數 eye（E）、focus（F）、view up（Vup），每次移動前再由它們算出眼睛座標系的 u、v、w 軸，
/// 在眼睛座標系中修改觀察參數，最後交給 gluLookAt。
/// 相機是遊玩中的操作狀態，不屬於世界；繪製時只取它算出的觀察參數與投影
class CameraRig {
   public:
    /// 直接設定觀察參數；up 不必與視線垂直，但不可與視線平行
    void place(const Vec3& eye, const Vec3& focus, const Vec3& up);

    /// 沿眼睛座標系的軸平移 E 與 F，Vup 不變；單位公尺
    /// @param surge 沿 -w，正值往前
    /// @param sway 沿 u，正值往右
    /// @param heave 沿 v，正值往上
    void move(float surge, float sway, float heave);

    /// 以 E 為中心旋轉，單位度
    /// @param yaw 繞 v 軸轉 F 與 Vup，正值往左轉
    /// @param pitch 繞 u 軸轉 F 與 Vup，正值抬頭
    /// @param roll 繞 -w 軸轉 Vup，正值往右傾（畫面逆時針轉）
    void rotate(float yaw, float pitch, float roll);

    /// 跟隨移動的物體：E、F 一起平移 offset，再與 Vup 一起繞通過 pivot 的鉛直軸轉 yaw 度
    void carry(const Vec3& offset, const Vec3& pivot, float yaw);

    /// @param factor 大於 1 拉近（視角變窄）、小於 1 拉遠
    void zoom(float factor);

    const Vec3& eye() const { return _eye; }
    const Vec3& focus() const { return _focus; }
    const Vec3& up() const { return _up; }
    float zoomFactor() const { return _zoom; }

    /// 眼睛座標系的三個軸（世界座標的單位向量），與 gluLookAt 相同：
    /// w 從 F 指向 E（視線是 -w），u = Vup × w 朝右，v = w × u 朝上
    Vec3 u() const;
    Vec3 v() const;
    Vec3 w() const;

    graphics::Camera camera() const;
    graphics::PerspectiveProjection projection() const;

   private:
    Vec3 _eye{0.0f, 2.0f, 3.0f};
    Vec3 _focus{0.0f, 2.0f, 0.0f};
    Vec3 _up{0.0f, 1.0f, 0.0f};

    // 縮放倍率：縮小視角，觀察參數不變
    float _zoom = 1.0f;
};

}  // namespace robot::game
