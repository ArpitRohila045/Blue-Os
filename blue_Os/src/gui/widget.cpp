#include <gui/widget.h>

using namespace blueOs::gui;
using namespace blueOs::common;


/* ------------------ WIDGET ------------------ */

Widget::Widget(Widget* parent, int32_t x, int32_t y,
               int32_t w, int32_t h,
               uint8_t r, uint8_t g, uint8_t b)
    : KeyboardEventHandler()
{
    this->parent = parent;
    this->x = x; this->y = y;
    this->w = w; this->h = h;
    this->r = r; this->g = g; this->b = b;
    this->Focusable = true;
}

Widget::~Widget() {}

void Widget::getFocus(Widget* widget) {
    if(parent != 0)
        parent->getFocus(widget);
}

void Widget::modelToScreen(int32_t &x, int32_t &y) {
    if(parent != 0)
        parent->modelToScreen(x, y);

    x += this->x;
    y += this->y;
}

bool Widget::containsCoordinate(int32_t x, int32_t y) {
    return (x >= this->x && x < this->x + this->w &&
            y >= this->y && y < this->y + this->h);
}

void Widget::draw(GraphicsContext* gc) {
    int32_t X = 0, Y = 0;
    modelToScreen(X, Y);
    gc->fillRectangle(X, Y, w, h, r, g, b);
}

void Widget::onMouseDown(int32_t x, int32_t y, uint8_t button) {
    if(Focusable)
        getFocus(this);
}

void Widget::onMouseUp(int32_t x, int32_t y, uint8_t button) {}
void Widget::onMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy) {}

// void Widget::onKeyLeft(char)  {}
// void Widget::onKeyRight(char) {}
// void Widget::onKeyPress(char) {}


/* ------------------ COMPOSITE ------------------ */

CompositeWidget::CompositeWidget(Widget* parent,
                                 int32_t x, int32_t y,
                                 int32_t w, int32_t h,
                                 uint8_t r, uint8_t g, uint8_t b)
    : Widget(parent, x, y, w, h, r, g, b)
{
    numChildren = 0;
    focussedChild = 0;
}

CompositeWidget::~CompositeWidget() {}

void CompositeWidget::getFocus(Widget* widget) {
    focussedChild = widget;
    if(parent != 0)
        parent->getFocus(this);
}

bool CompositeWidget::addChild(Widget* child) {
    if(numChildren >= 100)
        return false;

    children[numChildren++] = child;
    return true;
}

void CompositeWidget::draw(GraphicsContext* gc) {
    Widget::draw(gc);

    for(int i = 0; i < numChildren; i++)
        children[i]->draw(gc);
}

void CompositeWidget::onMouseDown(int32_t x, int32_t y, uint8_t button) {
    for(int i = 0; i < numChildren; i++) {
        if(children[i]->containsCoordinate(x - this->x, y - this->y)) {
            children[i]->onMouseDown(x - this->x, y - this->y, button);
            break;
        }
    }
}

void CompositeWidget::onMouseUp(int32_t x, int32_t y, uint8_t button) {
    for(int i = 0; i < numChildren; i++) {
        if(children[i]->containsCoordinate(x - this->x, y - this->y)) {
            children[i]->onMouseUp(x - this->x, y - this->y, button);
            break;
        }
    }
}

void CompositeWidget::onMouseMove(int32_t oldx, int32_t oldy,
                                  int32_t newx, int32_t newy) {

    int leaving = -1;

    for(int i = 0; i < numChildren; i++) {
        if(children[i]->containsCoordinate(oldx - x, oldy - y)) {
            children[i]->onMouseMove(oldx - x, oldy - y, newx - x, newy - y);
            leaving = i;
            break;
        }
    }

    for(int i = 0; i < numChildren; i++) {
        if(children[i]->containsCoordinate(newx - x, newy - y)) {
            if(leaving != i)
                children[i]->onMouseMove(oldx - x, oldy - y, newx - x, newy - y);
            break;
        }
    }
}

void CompositeWidget::onKeyLeft(char s) {
    if(focussedChild != 0)
        focussedChild->onKeyLeft(s);
}

void CompositeWidget::onKeyRight(char s) {
    if(focussedChild != 0)
        focussedChild->onKeyRight(s);
}

void CompositeWidget::onKeyPress(char s) {
    if(focussedChild != 0)
        focussedChild->onKeyPress(s);
}
