#include "aurora_types.h"
#include <core/typedefs.h>

namespace aurora {

Volume MmSquareToVolume(Mm size) {
    return Volume(size) * Volume(size) * 1e-6;
}

Meter MmToMeter(Mm length) {
    return length * 1e-3;
}

Mm2::Mm2(Mm iX, Mm iY)
    : x(iX)
    , y(iY)
{
}

Mm2::Mm2(Mm xy)
    : x(xy)
    , y(xy)
{
}

Mm2 Mm2::operator+(Mm2 const&p_v) const
{
    return Mm2(x + p_v.x, y + p_v.y);
}


Mm2 Mm2::operator*(Mm v) const
{
    return Mm2(x * v, y * v);
}


Vector2 Mm2::ToVector2() const
{
    return Vector2(x, y);
}



MmRect::MmRect()
    : position(0)
    , size(0)
{
}

MmRect::MmRect(Mm2 iPosition, Mm2 iSize)
    : position(iPosition)
    , size(iSize)
{
}

MmRect::MmRect(Mm x, Mm y, Mm width, Mm height)
    : position(x, y)
    , size(width, height)
{

}


MmRect MmRect::Clip(MmRect const& p_rect) const {

    MmRect new_rect = p_rect;

    if (!Intersects(new_rect))
        return MmRect();

    new_rect.position.x = MAX(p_rect.position.x, position.x);
    new_rect.position.y = MAX(p_rect.position.y, position.y);

    Mm2 p_rect_end = p_rect.position + p_rect.size;
    Mm2 end = position + size;

    new_rect.size.x = MIN(p_rect_end.x, end.x) - new_rect.position.x;
    new_rect.size.y = MIN(p_rect_end.y, end.y) - new_rect.position.y;

    return new_rect;
}

bool MmRect::Intersects(MmRect const&p_rect) const {
    if (position.x >= (p_rect.position.x + p_rect.size.x))
        return false;
    if ((position.x + size.x) <= p_rect.position.x)
        return false;
    if (position.y >= (p_rect.position.y + p_rect.size.y))
        return false;
    if ((position.y + size.y) <= p_rect.position.y)
        return false;

    return true;
}

}
