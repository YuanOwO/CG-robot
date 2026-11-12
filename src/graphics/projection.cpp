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

ViewVolume ViewVolume::ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    if (nearPlane >= farPlane) {
        throw std::invalid_argument("nearPlane must be less than farPlane");
    }

    return ViewVolume{ProjectionMode::Ortho, left, right, bottom, top, nearPlane, farPlane};
}

ViewVolume ViewVolume::perspective(float left, float right, float bottom, float top, float nearPlane,
                                   float farPlane) {
    if (nearPlane <= 0.0f || farPlane <= 0.0f) {
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

ViewVolume OrthoProjection::volume(float aspect) const {
    const float halfWidth = halfHeight * aspect;

    return ViewVolume::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
}

ViewVolume PerspectiveProjection::volume(float aspect) const {
    const float top = nearPlane * std::tan(radians(fovY) / 2.0f);
    const float right = top * aspect;

    return ViewVolume::perspective(-right, right, -top, top, nearPlane, farPlane);
}

ViewVolume volume(const Projection& projection, float aspect) {
    return std::visit([aspect](const auto& proj) { return proj.volume(aspect); }, projection);
}

}  // namespace robot::graphics
