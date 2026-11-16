#include "view/hud.hpp"

#include <cmath>
#include <cstdio>
#include <string>

#include "common/color.hpp"
#include "font/font.hpp"

namespace robot::view {

namespace {

const FontStyle HUD_FONT = GlyphFontStyle{GlyphFont::UNIFONT, 16};
const ColorRGBA PANEL_COLOR{0.0f, 0.0f, 0.0f, 0.6f};
const ColorRGBA SEPARATOR_COLOR{1.0f, 1.0f, 1.0f, 0.9f};
const int SEPARATOR_WIDTH = 2;

// 文字加上半透明黑底，position 為底框的左上角
void drawPanel(graphics::Canvas& canvas, const std::string& text, int x, int y) {
    const int textWidth = static_cast<int>(std::ceil(getFontWidth(HUD_FONT, text)));
    const int textHeight = static_cast<int>(std::ceil(getFontHeight(HUD_FONT, text)));

    canvas.fillRect(x, y, textWidth + 12, textHeight + 8, PANEL_COLOR);
    canvas.drawText(text, HUD_FONT, Color::White, Point(x + 6, y + 4));
}

void drawViewLabels(graphics::Canvas& canvas, const HudInfo& info) {
    if (info.views.size() > 1) {
        // 四格畫面之間的十字分隔線
        const int midX = info.windowWidth / 2;
        const int midY = info.windowHeight / 2;
        canvas.fillRect(midX - SEPARATOR_WIDTH / 2, 0, SEPARATOR_WIDTH, info.windowHeight, SEPARATOR_COLOR);
        canvas.fillRect(0, midY - SEPARATOR_WIDTH / 2, info.windowWidth, SEPARATOR_WIDTH, SEPARATOR_COLOR);
    }

    for (const auto& view : info.views) {
        // viewport 的原點在左下角、canvas 在左上角，換算這一塊的底邊位置；標籤放在左下角
        const int bottom = info.windowHeight - view.viewport.y;
        const int labelHeight = static_cast<int>(std::ceil(getFontHeight(HUD_FONT, view.label))) + 8;
        drawPanel(canvas, view.label, view.viewport.x + 8, bottom - labelHeight - 8);
    }
}

}  // namespace

void Hud::render(graphics::Canvas& canvas, const HudInfo& info) const {
    drawViewLabels(canvas, info);

    char buffer[1024];
    std::snprintf(buffer, sizeof(buffer),
                  "FPS %.2f · TPS %.2f\n"
                  "View: %s (F1–F5) · Camera: %s (Tab)\n"
                  "E: %.2f, %.2f, %.2f\n"
                  "F: %.2f, %.2f, %.2f\n"
                  "Vup: %.2f, %.2f, %.2f\n"
                  "FOV %.1f° · Ortho half height %.1f m\n"
                  "Robot yaw %.1f°\n"
                  "X: %.2f · Y: %.2f · Z: %.2f\n"
                  "VX: %.2f · VY: %.2f · VZ: %.2f\n",
                  info.framesPerSecond, info.ticksPerSecond, toString(info.viewMode).c_str(), info.cameraMode.c_str(),
                  info.cameraEye.x, info.cameraEye.y, info.cameraEye.z, info.cameraFocus.x, info.cameraFocus.y,
                  info.cameraFocus.z, info.cameraUp.x, info.cameraUp.y, info.cameraUp.z, info.fovY,
                  info.orthoHalfHeight, info.playerYaw, info.playerPosition.x, info.playerPosition.y,
                  info.playerPosition.z, info.playerVelocity.x, info.playerVelocity.y, info.playerVelocity.z);

    drawPanel(canvas, buffer, 8, 8);
}

}  // namespace robot::view
