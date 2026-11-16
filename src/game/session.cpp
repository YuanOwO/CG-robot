#include "game/session.hpp"

namespace robot::game {

Session::Session() : _world(_seed), _player(_world, _world.spawnPosition()) {
    _world.setFocus(_player.info().position);
}

void Session::tick(float dt, const engine::Input& input) {
    _player.update(dt, input);
    _world.tick(dt);
    _player.follow();
    _world.setFocus(_player.info().position);
}

void Session::pause() {
    _player.cancelInput();
}

void Session::renderWorld(int width, int height) const {
    const view::ViewVolumeOverlay overlay{_player.cameraRig().camera(), _player.perspectiveVolume(width, height)};

    for (const auto& view : _player.views(width, height)) {
        _worldRenderer.render(_world, view, overlay);
    }
}

void Session::renderHud(graphics::Canvas& canvas, double framesPerSecond, double ticksPerSecond, int width,
                        int height) const {
    const auto player = _player.info();
    const auto& rig = _player.cameraRig();

    view::HudInfo info{framesPerSecond,
                       ticksPerSecond,
                       _player.viewMode(),
                       toString(_player.cameraMode()),
                       rig.eye(),
                       rig.focus(),
                       rig.up(),
                       rig.projection().fovY,
                       _player.orthoHalfHeight(),
                       player.yaw,
                       player.position,
                       player.velocity,
                       _player.views(width, height),
                       width,
                       height};

    _hud.render(canvas, info);
}

}  // namespace robot::game
