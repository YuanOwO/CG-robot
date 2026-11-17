#include "view/renderer/world_renderer.hpp"

#include <array>
#include <cstddef>
#include <vector>

#include "graphics/3d/primitives.hpp"
#include "graphics/render_mode.hpp"
#include "platform/glut.hpp"

namespace robot::view {

namespace {

const float AXIS_LENGTH = 8.0f;

// 平行投影畫疊加的線條時，前後範圍放寬到這個距離：
// 平行投影的畫面位置與深度無關，放寬只是讓遠處的線不被裁掉
const float OVERLAY_DEPTH = 1000.0f;

const ColorRGBA VOLUME_EDGE_COLOR{1.0f, 0.85f, 0.1f, 1.0f};
const ColorRGBA VOLUME_FACE_COLOR{1.0f, 0.85f, 0.1f, 0.12f};
const ColorRGBA CAMERA_UP_COLOR{1.0f, 0.3f, 0.9f, 1.0f};
const float CAMERA_UP_LENGTH = 1.0f;

// 疊加的線條不受深度測試影響，被地形擋住時仍看得到；
// 平行投影另外放寬前後範圍，避免被畫面的前後範圍裁掉
void beginOverlay(const SceneView& view) {
    glDisable(GL_DEPTH_TEST);

    if (view.viewport.empty()) {
        return;
    }

    auto volume = graphics::volume(view.projection, view.viewport.aspect());
    if (volume.kind == graphics::ProjectionMode::Ortho) {
        volume.nearPlane = -OVERLAY_DEPTH;
        volume.farPlane = OVERLAY_DEPTH;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        volume.apply();
        glMatrixMode(GL_MODELVIEW);
    }
}

void drawWorldAxes() {
    graphics::drawAxes(AXIS_LENGTH);
    graphics::drawPoints({Vec3()}, ColorRGBA{1.0f, 1.0f, 1.0f, 1.0f}, 8.0f);
}

// 從眼睛到 far 平面的四角錐：半透明的側面加上邊線，再標出眼睛與相機的上方向
void drawViewVolume(const ViewVolumeOverlay& overlay) {
    const auto corners = overlay.volume.corners(overlay.camera);
    const Vec3& eye = overlay.camera.eye;

    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(VOLUME_FACE_COLOR.r, VOLUME_FACE_COLOR.g, VOLUME_FACE_COLOR.b, VOLUME_FACE_COLOR.a);
    glBegin(GL_TRIANGLES);
    for (std::size_t i = 0; i < 4; i++) {
        const Vec3& a = corners[4 + i];
        const Vec3& b = corners[4 + (i + 1) % 4];
        glVertex3f(eye.x, eye.y, eye.z);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
    }
    glEnd();
    glPopAttrib();

    std::vector<geometry::LineSegment3D> edges;
    for (std::size_t i = 0; i < 4; i++) {
        const std::size_t next = (i + 1) % 4;
        edges.emplace_back(eye, corners[4 + i]);                 // 從眼睛出發的四條稜
        edges.emplace_back(corners[4 + i], corners[4 + next]);  // far 平面的四條邊
    }
    graphics::drawLines(edges, VOLUME_EDGE_COLOR, 2.0f);

    const Vec3 up = normalize(overlay.camera.up) * CAMERA_UP_LENGTH;
    graphics::drawLines({geometry::LineSegment3D(eye, eye + up)}, CAMERA_UP_COLOR, 3.0f);
    graphics::drawPoints({eye}, VOLUME_EDGE_COLOR, 8.0f);
}

}  // namespace

void WorldRenderer::render(const world::World& world, const SceneView& view,
                           const std::optional<ViewVolumeOverlay>& overlay) const {
    graphics::begin3D(view.viewport, view.camera, view.projection);

    _chunkRenderer.render(world.chunks());

    world.entities().forEach([this](const world::entity::Entity& e) { _entityRenderer.render(e); });

    beginOverlay(view);
    drawWorldAxes();
    if (view.showsViewVolume && overlay) {
        drawViewVolume(*overlay);
    }
}

}  // namespace robot::view
