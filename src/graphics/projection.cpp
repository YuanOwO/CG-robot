#include "graphics/projection.hpp"

#include <cmath>
#include <stdexcept>

#include "math/utils.hpp"
#include "platform/glut.hpp"

namespace robot::graphics {

std::string toString(ProjectionMode mode) {
    switch (mode) {
    case ProjectionMode::Ortho:
        return "Ortho";
    case ProjectionMode::Perspective:
        return "Perspective";
    default:
        return "Unknown";
    }
}

ViewVolume ViewVolume::ortho(double left, double right, double bottom, double top, double nearPlane, double farPlane) {
    if (nearPlane >= farPlane) {
        throw std::invalid_argument("nearPlane must be less than farPlane");
    }

    return ViewVolume{ProjectionMode::Ortho, left, right, bottom, top, nearPlane, farPlane};
}

ViewVolume ViewVolume::perspective(double left, double right, double bottom, double top, double nearPlane,
                                   double farPlane) {
    if (nearPlane <= 0.0 || farPlane <= 0.0) {
        throw std::invalid_argument("nearPlane and farPlane must be positive");
    }

    if (nearPlane >= farPlane) {
        throw std::invalid_argument("nearPlane must be less than farPlane");
    }

    return ViewVolume{ProjectionMode::Perspective, left, right, bottom, top, nearPlane, farPlane};
}

void ViewVolume::apply() const {
    if (kind == ProjectionMode::Ortho) {
        glOrtho(left, right, bottom, top, nearPlane, farPlane);
    } else if (kind == ProjectionMode::Perspective) {
        glFrustum(left, right, bottom, top, nearPlane, farPlane);
    } else {
        throw std::runtime_error("Unknown view volume kind");
    }
}

ViewVolume OrthoProjection::volume(double aspect) const {
    double halfWidth = halfHeight * aspect;

    return ViewVolume::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
}

ViewVolume PerspectiveProjection::volume(double aspect) const {
    const double top = nearPlane * std::tan(radians(fovY) / 2.0);
    const double right = top * aspect;

    return ViewVolume::perspective(-right, right, -top, top, nearPlane, farPlane);
}

ViewVolume volume(const Projection& projection, double aspect) {
    return std::visit([aspect](const auto& proj) { return proj.volume(aspect); }, projection);
}

}  // namespace robot::graphics
