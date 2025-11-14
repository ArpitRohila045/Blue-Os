#ifndef __BLUE_OS__GUI__WIDGET_H
#define __BLUE_OS__GUI__WIDGET_H

#include <common/types.h>
#include <common/graphicscontext.h>
#include <drivers/keyboard.h>

namespace blueOs {
    namespace gui {

        class Widget : public blueOs::drivers::KeyboardEventHandler {
        protected:
            Widget* parent;
            common::int32_t x, y, w, h;
            common::uint8_t r, g, b;

            bool Focusable;

        public:
            Widget(Widget* parent,
                   common::int32_t x, common::int32_t y,
                   common::int32_t w, common::int32_t h,
                   common::uint8_t r, common::uint8_t g, common::uint8_t b);

            virtual ~Widget();

            virtual void getFocus(Widget* widget);
            virtual void modelToScreen(common::int32_t &x, common::int32_t &y);
            virtual bool containsCoordinate(common::int32_t x, common::int32_t y);

            virtual void draw(common::GraphicsContext* gc);
            virtual void onMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void onMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void onMouseMove(common::int32_t oldx, common::int32_t oldy,
                                     common::int32_t newx, common::int32_t newy);
        };


        class CompositeWidget : public Widget {
        private:
            Widget* children[100];
            int numChildren;
            Widget* focussedChild;

        public:
            CompositeWidget(Widget* parent,
                            common::int32_t x, common::int32_t y,
                            common::int32_t w, common::int32_t h,
                            common::uint8_t r, common::uint8_t g, common::uint8_t b);

            virtual ~CompositeWidget();

            virtual void getFocus(Widget* widget);
            virtual bool addChild(Widget* child);

            virtual void draw(common::GraphicsContext* gc);
            virtual void onMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void onMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void onMouseMove(common::int32_t oldx, common::int32_t oldy,
                                     common::int32_t newx, common::int32_t newy);

            virtual void onKeyLeft(char) override;
            virtual void onKeyRight(char) override;
            virtual void onKeyPress(char) override;
        };

    }
}

#endif
