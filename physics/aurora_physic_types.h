#ifndef AURORA_PHYSIC_TYPES_H
#define AURORA_PHYSIC_TYPES_H

//#include "core/math/vector2.h"
//#include "core/math/rect2.h"
#include "../tools/aurora_types.h"
//#include <vector>

namespace aurora {

enum class Gas
{
    Hydrogen, //H2
    Nitrogen, // N2
    Oxygen, // O2
    Water,  // H2O
    Methane,// CH4
    CarbonDioxide, // CO2
    SulfurDioxide, // SO2
    U235F6, // UF6 with U235
    U238F6, // UF6 with U238

    Count
};

enum class Liquid
{
    Hydrogen, //H2
    Nitrogen, // N2
    Oxygen, // O2
    Water,  // H2O
    Methane,// CH4
    CarbonDioxide, // CO2
    Salt, // NaCl
    Iron, // Fe
    Gold, // Au

    Count
};

enum class Solid
{
    Water,  // H2O
    Salt, // NaCl
    Iron, // Fe
    Gold, // Au

    Limestone, // CaCO3 -> Decay in CaO + CO2 at 800 C
    Lime, // CaO, used to do concrete ?
    Wood, // Decay at 400 C into 25% of Charcoal, 25% of Methane and 50 % of water
    Charcoal, // C
    Coal, // C + Sulfur, decay as coke at  1200C
    Clay, // Al2O3 2SiO2 2H2O

    Count
};

size_t constexpr GasCount = (size_t) Gas::Count;
size_t constexpr LiquidCount = (size_t) Liquid::Count;
size_t constexpr SolidCount = (size_t) Solid::Count;

template<typename T, typename I, int C>
class EnumClassArray
{
public:
    T& operator[](I index)
    {
        return m_data[(int) index];
    }

    T const& operator[](I index) const
    {
        return m_data[(int) index];
    }
private:
    T m_data[C];
};

template< typename T, int C>
class Enum
{
public:
   class Iterator
   {
   public:
      Iterator( int value ) :
         m_value( value )
      { }

      T operator*( void ) const
      {
         return (T)m_value;
      }

      void operator++( void )
      {
         ++m_value;
      }

      bool operator!=( Iterator rhs )
      {
         return m_value != rhs.m_value;
      }

   private:
      int m_value;
   };

};

template< typename T, int C>
typename Enum<T, C>::Iterator begin( Enum<T, C> )
{
   return typename Enum<T, C>::Iterator(0 );
}

template< typename T, int C>
typename Enum<T, C>::Iterator end( Enum<T, C> )
{
   return typename Enum<T, C>::Iterator(C);
}


using GasComposition = EnumClassArray<Quantity, Gas, GasCount>;

using AllGas = Enum<Gas, GasCount>;

}

#endif
