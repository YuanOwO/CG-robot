#pragma once

#include <optional>

#include "graphics/camera.hpp"
#include "graphics/projection.hpp"
#include "view/renderer/chunk_renderer.hpp"
#include "view/renderer/entity_renderer.hpp"
#include "view/scene_view.hpp"
#include "world/world.hpp"

namespace robot::view {

/// 要疊在平行投影畫面上的透視相機：用來畫出它的 view volume
struct ViewVolumeOverlay {
    graphics::Camera camera;
    graphics::ViewVolume volume;
};

// 依照世界的狀態畫出場景，只讀取世界、不修改。
class WorldRenderer {
   public:
    /// 在 view 指定的區域畫出場景，再疊上世界座標軸；
    /// view.showsViewVolume 為真且有 overlay 時，另外畫出透視相機的 view volume
    void render(const world::World& world, const SceneView& view,
                const std::optional<ViewVolumeOverlay>& overlay) const;

   private:
    ChunkRenderer _chunkRenderer;
    EntityRenderer _entityRenderer;
};

}  // namespace robot::view
