#pragma once

#include <string>
#include <variant>

namespace robot::graphics {

enum class ProjectionMode { Ortho, Perspective };

std::string toString(ProjectionMode mode);

struct ViewVolume {
    ProjectionMode kind;
    double left, right, bottom, top;
    double nearPlane, farPlane;

    static ViewVolume ortho(double left, double right, double bottom, double top, double nearPlane, double farPlane);
    static ViewVolume perspective(double left, double right, double bottom, double top, double nearPlane,
                                  double farPlane);

    void apply() const;
};

struct OrthoProjection {
    double halfHeight = 20.0;
    double nearPlane = -200.0;
    double farPlane = 200.0;

    ViewVolume volume(double aspect) const;
};

struct PerspectiveProjection {
    double fovY = 60.0;  // 垂直方向的視角，單位度
    double nearPlane = 0.1;
    double farPlane = 200.0;

    ViewVolume volume(double aspect) const;
};

using Projection = std::variant<OrthoProjection, PerspectiveProjection>;

ViewVolume volume(const Projection& projection, double aspect);

}  // namespace robot::graphics
