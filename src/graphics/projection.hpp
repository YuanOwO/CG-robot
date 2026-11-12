#pragma once

#include <string>
#include <variant>

namespace robot::graphics {

enum class ProjectionMode { Ortho, Perspective };

std::string toString(ProjectionMode mode);

struct ViewVolume {
    ProjectionMode kind;
    float left, right, bottom, top;
    float nearPlane, farPlane;

    static ViewVolume ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    static ViewVolume perspective(float left, float right, float bottom, float top, float nearPlane, float farPlane);

    void apply() const;
};

struct OrthoProjection {
    float halfHeight = 20.0f;
    float nearPlane = -200.0f;
    float farPlane = 200.0f;

    ViewVolume volume(float aspect) const;
};

struct PerspectiveProjection {
    float fovY = 60.0f;  // 垂直方向的視角，單位度
    float nearPlane = 0.1f;
    // 世界以玩家為中心載入半徑 12 個 chunk（192 公尺）的範圍，相機又在玩家後方，
    // 留一些餘裕才看得到最遠處的地形
    float farPlane = 250.0f;

    ViewVolume volume(float aspect) const;
};

using Projection = std::variant<OrthoProjection, PerspectiveProjection>;

ViewVolume volume(const Projection& projection, float aspect);

}  // namespace robot::graphics
