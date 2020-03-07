#include "aurora_types.h"
#include <core/typedefs.h>

namespace aurora {

Meter2::Meter2(Meter iX, Meter iY)
    : x(iX)
    , y(iY)
{
}

Meter2::Meter2(Meter xy)
    : x(xy)
    , y(xy)
{
}

Meter2 Meter2::operator+(Meter2 const&p_v) const
{
    return Meter2(x + p_v.x, y + p_v.y);
}

Meter2 Meter2::operator*(Meter v) const
{
    return Meter2(x * v, y * v);
}

Vector2 Meter2::ToVector2() const
{
    return Vector2(x, y);
}

MeterRect::MeterRect()
    : position(0)
    , size(0)
{
}

MeterRect::MeterRect(Meter2 iPosition, Meter2 iSize)
    : position(iPosition)
    , size(iSize)
{
}

MeterRect::MeterRect(Meter x, Meter y, Meter width, Meter height)
    : position(x, y)
    , size(width, height)
{

}

MeterRect MeterRect::Clip(MeterRect const& p_rect) const {

    MeterRect new_rect = p_rect;

    if (!Intersects(new_rect))
        return MeterRect();

    new_rect.position.x = MAX(p_rect.position.x, position.x);
    new_rect.position.y = MAX(p_rect.position.y, position.y);

    Meter2 p_rect_end = p_rect.position + p_rect.size;
    Meter2 end = position + size;

    new_rect.size.x = MIN(p_rect_end.x, end.x) - new_rect.position.x;
    new_rect.size.y = MIN(p_rect_end.y, end.y) - new_rect.position.y;

    return new_rect;
}

bool MeterRect::Intersects(MeterRect const&p_rect) const {
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
