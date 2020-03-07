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
typedef Scalar Quantity;

//typedef int32_t Mm; // Mm
typedef Scalar Volume; // m3
typedef Scalar Meter; // m

//Volume MmSquareToVolume(Mm size);
//Meter MmToMeter(Mm length);

struct Meter2
{
    Meter2(Meter xy);
    Meter2(Meter iX, Meter iY);

    Meter x;
    Meter y;

    Meter2 operator+(Meter2 const& p_v) const;
    Meter2 operator*(Meter v) const;
    bool operator==(Meter2 const&p_v) const { return x== p_v.x && y == p_v.y; }

    Vector2 ToVector2() const;
};

struct MeterRect
{
    MeterRect();
    MeterRect(Meter2 iPosition, Meter2 iSize);
    MeterRect(Meter x, Meter y, Meter width, Meter height);

    MeterRect Clip(MeterRect const& p_rect) const;
    bool Intersects(MeterRect const&p_rect) const;

    inline bool IsEmpty() const {
        return (size.x <= 0 || size.y <= 0);
    }

    bool operator==(MeterRect const&p_rect) const { return position== p_rect.position && size == p_rect.size; }


    Meter2 position;
    Meter2 size;
};



inline Scalar sqr(Scalar x) { return x*x; }

inline Scalar sign(Scalar val) {
    return (0 < val) - (val < 0);
}

}

#endif
