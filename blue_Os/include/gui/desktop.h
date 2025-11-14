#ifndef __BLUE_OS__GUI__DESKTOP_H
#define __BLUE_OS__GUI__DESKTOP_H

#include <gui/widget.h>
#include <drivers/mouse.h>

namespace blueOs {
namespace gui {

    class Desktop : public CompositeWidget,
                    public blueOs::drivers::MouseEventHandler
    {
    protected:
        common::uint32_t mouseX;
        common::uint32_t mouseY;

    public:
        Desktop(common::int32_t w, common::int32_t h,
                common::uint8_t r, common::uint8_t g, common::uint8_t b);

        virtual ~Desktop();
        

        virtual void draw(common::GraphicsContext* gc) override;

        // Mouse event overrides
        virtual void onMouseDown(common::uint8_t button) override;
        virtual void onMouseUp(common::uint8_t button) override;
        virtual void onMouseMove(common::int32_t x, common::int32_t y) override;
    };

}
}

#endif
