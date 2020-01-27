#ifndef AURORA_TYPES_H
#define AURORA_TYPES_H

#include <stdint.h>
#include <core/math/rect2.h>

namespace aurora {


typedef double Scalar;


struct int2
{
	int x;
	int y;
};

typedef Scalar Energy;
typedef int64_t Quantity;

typedef int32_t Mm; // Mm
typedef Scalar Volume; // m2
typedef Scalar Meter; // m

Volume MmSquareToVolume(Mm size);
Meter MmToMeter(Mm length);

struct Mm2
{
    Mm2(Mm xy);
    Mm2(Mm iX, Mm iY);

    Mm x;
    Mm y;

    Mm2 operator+(Mm2 const& p_v) const;
    Mm2 operator*(Mm v) const;
    bool operator==(Mm2 const&p_v) const { return x== p_v.x && y == p_v.y; }

    Vector2 ToVector2() const;
};



struct MmRect
{
    MmRect();
    MmRect(Mm2 iPosition, Mm2 iSize);
    MmRect(Mm x, Mm y, Mm width, Mm height);

    MmRect Clip(MmRect const& p_rect) const;
    bool Intersects(MmRect const&p_rect) const;

    inline bool IsEmpty() const {
        return (size.x <= 0 || size.y <= 0);
    }

    bool operator==(MmRect const&p_rect) const { return position== p_rect.position && size == p_rect.size; }


    Mm2 position;
    Mm2 size;
};



inline Scalar sqr(Scalar x) { return x*x; }

}


#endif
