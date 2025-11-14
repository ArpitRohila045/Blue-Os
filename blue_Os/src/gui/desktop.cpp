#include <gui/desktop.h>

using namespace blueOs;
using namespace blueOs::common;
using namespace blueOs::gui;
using namespace blueOs::drivers;

namespace blueOs {
namespace gui {

Desktop::Desktop(int32_t w, int32_t h,
                 uint8_t r, uint8_t g, uint8_t b)
: CompositeWidget(0, 0, 0, w, h, r, g, b),
  MouseEventHandler()
{
    mouseX = w / 2;
    mouseY = h / 2;
}

Desktop::~Desktop() {}


void Desktop::draw(GraphicsContext* gc)
{
    // draw all widgets
    CompositeWidget::draw(gc);

    // draw mouse cursor (simple crosshair)
    for(int i = 0; i < 4; i++)
    {
        gc->PutPixel(mouseX - i, mouseY,     0xFF, 0xFF, 0xFF);
        gc->PutPixel(mouseX + i, mouseY,     0xFF, 0xFF, 0xFF);
        gc->PutPixel(mouseX,     mouseY - i, 0xFF, 0xFF, 0xFF);
        gc->PutPixel(mouseX,     mouseY + i, 0xFF, 0xFF, 0xFF);
    }
}


void Desktop::onMouseDown(uint8_t button)
{
    CompositeWidget::onMouseDown(mouseX, mouseY, button);
}

void Desktop::onMouseUp(uint8_t button)
{
    CompositeWidget::onMouseUp(mouseX, mouseY, button);
}

void Desktop::onMouseMove(int32_t x, int32_t y)
{
    // reduce sensitivity
    x /= 4;
    y /= 4;

    int32_t w = this->w;
    int32_t h = this->h;

    int32_t newMouseX = mouseX + x;
    if(newMouseX < 0) newMouseX = 0;
    if(newMouseX >= w) newMouseX = w - 1;

    int32_t newMouseY = mouseY + y;
    if(newMouseY < 0) newMouseY = 0;
    if(newMouseY >= h) newMouseY = h - 1;

    CompositeWidget::onMouseMove(mouseX, mouseY, newMouseX, newMouseY);

    mouseX = newMouseX;
    mouseY = newMouseY;
}

} // namespace gui
} // namespace blueOs
