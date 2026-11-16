#include "view/view_mode.hpp"

namespace robot::view {

std::string toString(ViewMode mode) {
    switch (mode) {
    case ViewMode::OrthoX:
        return "Ortho X";
    case ViewMode::OrthoY:
        return "Ortho Y";
    case ViewMode::OrthoZ:
        return "Ortho Z";
    case ViewMode::Perspective:
        return "Perspective";
    case ViewMode::Quad:
        return "Quad";
    default:
        return "Unknown";
    }
}

}  // namespace robot::view
