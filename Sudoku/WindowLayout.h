#ifndef WINDOW_LAYOUT_H
#define WINDOW_LAYOUT_H

#include "Constants.h"
#include <algorithm>

namespace WindowLayout {
struct Size { unsigned width; unsigned height; };
struct Viewport { float left; float top; float width; float height; };

inline Size initialSize(unsigned desktopWidth, unsigned desktopHeight) {
    // Leave room for window decorations and the desktop taskbar/dock.
    const double scale = std::min({1.0,
        desktopWidth * 0.9 / LAYOUT_WINDOW_WIDTH,
        desktopHeight * 0.9 / LAYOUT_WINDOW_HEIGHT});
    return {std::max(1u, static_cast<unsigned>(LAYOUT_WINDOW_WIDTH * scale)),
            std::max(1u, static_cast<unsigned>(LAYOUT_WINDOW_HEIGHT * scale))};
}

inline Viewport viewport(unsigned width, unsigned height) {
    if (width == 0 || height == 0)
        return {0.f, 0.f, 1.f, 1.f};
    const float windowRatio = static_cast<float>(width) / height;
    const float layoutRatio = static_cast<float>(LAYOUT_WINDOW_WIDTH) / LAYOUT_WINDOW_HEIGHT;
    if (windowRatio > layoutRatio) {
        const float fraction = layoutRatio / windowRatio;
        return {(1.f - fraction) / 2.f, 0.f, fraction, 1.f};
    }
    const float fraction = windowRatio / layoutRatio;
    return {0.f, (1.f - fraction) / 2.f, 1.f, fraction};
}
}

#endif
