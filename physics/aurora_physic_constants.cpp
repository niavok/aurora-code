#include "aurora_physic_constants.h"
#include <assert.h>

namespace aurora {

// N is a mole

template<typename T, typename I, int C>
class ConstantEnumClassArray
{
public:

    ConstantEnumClassArray()
    {
        for(int i = 0; i < C; i++)
        {
            m_isInit[i] = false;
        }
    }

    T operator[](I index) const
    {
        return m_data[(int) index];
    }

    void Set(I index, T value)
    {
        assert(!m_isInit[(int) index]);
        if(m_isInit[(int) index])
        {
            return;
        }
        m_data[(int) index] = value;
        m_isInit[(int) index] = true;
    }

    void Check() const
    {
        for(int i = 0; i < C; i++)
        {
            assert(m_isInit[i]);
        }
    }

private:
    T m_data[C];
    bool m_isInit[C];
};


ConstantEnumClassArray<Quantity, Solid, SolidCount> solidNByVolume;
ConstantEnumClassArray<Quantity, Liquid, LiquidCount> liquidNByVolume;
ConstantEnumClassArray<Scalar, Liquid, LiquidCount> liquidThermalExpansionCoef;
ConstantEnumClassArray<Scalar, Liquid, LiquidCount> liquidCompressibilityCoef;

ConstantEnumClassArray<Scalar, Gas, GasCount> gasSpecifHeat; // Joules.kg-1.K-1
ConstantEnumClassArray<Scalar, Gas, GasCount> gasMassByMole; // kg.mol-1

/*
static Quantity solidNByVolume[SolidCount] = // TODO
{
    10, // Water
    10, // Salt
    10, // Iron
    10, // Gold
    10, // Limestone
    10, // Lime
    10, // Wood
    10, // Charcoal
    10, // Coal
    10  // Clay
};

static Quantity liquidNByVolume[LiquidCount] = // TODO
{
    10, // Hydrogen
    10, // Nitrogen
    10, // Oxygen
    10, // Water
    10, // Methane
    10, // CarbonDioxide
    10, // Salt
    10, // Iron
    10, // Gold
};

static Scalar liquidThermalExpansionCoef[LiquidCount] = // TODO
{
    0.01, // Hydrogen
    0.01, // Nitrogen
    0.01, // Oxygen
    0.01, // Water
    0.01, // Methane
    0.01, // CarbonDioxide
    0.01, // Salt
    0.01, // Iron
    0.01, // Gold
};

static Scalar liquidCompressibilityCoef[LiquidCount] =
{
    100., // Hydrogen
    100., // Nitrogen
    100., // Oxygen
    100., // Water
    100., // Methane
    100., // CarbonDioxide
    100., // Salt
    100., // Iron
    100., // Gold
};

static Scalar gasSpecifHeat[GasCount] =
{
    14300, // Hydrogen
     1040, // Nitrogen
      918, // Oxygen
     2080, // Water
     2191, // Methane
      839, // CarbonDioxide
      640, // SO2
      392, // U235F6
      392, // U238F6
};*/
/*
static Material dissolvedMaterial[Material::MaterialCount] = // Joules.N-1.K-1
{
    CarbonDioxide, // Hydrogen
    CarbonDioxide, // Nitrogen
    CarbonDioxide, // Oxygen
    CarbonDioxide, // Water
    CarbonDioxide, // Methane
    CarbonDioxide, // CarbonDioxide
    CarbonDioxide, // Salt
    CarbonDioxide, // Iron
    CarbonDioxide, // Gold
    CarbonDioxide, // Limestone
    CarbonDioxide, // Lime
    CarbonDioxide, // Wood
    CarbonDioxide, // Charcoal
    CarbonDioxide, // Coal
    CarbonDioxide  // Clay
};*/
/*

static Scalar gasMassByMole[GasCount] = // kg.mol-1
{
    0.00201588, // Hydrogen
    0.0280134, // Nitrogen
    0.0319988, // Oxygen
    0.0180153, // Water
    0.01604, // Methane
    0.0440095, // CarbonDioxide
    0.064064, // SO2
    0.34903434, // U235F6  18,998403163 * 6 + 235,0439299
    0.35204120, // U238F6  18,998403163 * 6 + 238,0507826
};*/
/*
static Scalar gasSpecifHeat[GasCount] = // Joules.kg-1.K-1
{
    14300, // Hydrogen
     1040, // Nitrogen
      918, // Oxygen
     2080, // Water
     2191, // Methane
      839, // CarbonDioxide
      640, // SO2
      392, // U235F6
      392, // U238F6
};*/

////////////////////////
/// PhysicalConstants
////////////////////////

static bool isInit = false;

void PhysicalConstants::Init()
{
    if(isInit)
    {
        return;
    }

    Gas gas;
    // H2
    gas = Gas::Hydrogen;
    gasSpecifHeat.Set(gas, 14300);
    gasMassByMole.Set(gas, 0.00201588);

    // N2
    gas = Gas::Nitrogen;
    gasSpecifHeat.Set(gas, 1040);
    gasMassByMole.Set(gas, 0.0280134);

    // Oxygen
    gas = Gas::Oxygen;
    gasSpecifHeat.Set(gas, 918);
    gasMassByMole.Set(gas, 0.0319988);

    // H2O
    gas = Gas::Water;
    gasSpecifHeat.Set(gas, 2080);
    gasMassByMole.Set(gas, 0.0180153);

    // CH4
    gas = Gas::Methane;
    gasSpecifHeat.Set(gas, 2191);
    gasMassByMole.Set(gas, 0.01604);

    // CO2
    gas = Gas::CarbonDioxide;
    gasSpecifHeat.Set(gas, 839);
    gasMassByMole.Set(gas, 0.0440095);

    // SO2
    gas = Gas::SulfurDioxide;
    gasSpecifHeat.Set(gas, 640);
    gasMassByMole.Set(gas, 0.064064);

    // UF6 with U235
    gas = Gas::U235F6;
    gasSpecifHeat.Set(gas, 392);
    gasMassByMole.Set(gas, 0.34903434);

    // UF6 with U238
    gas = Gas::U238F6;
    gasSpecifHeat.Set(gas, 392);
    gasMassByMole.Set(gas, 0.35204120);

    //solidNByVolume.Check();
    //liquidNByVolume.Check();
    //liquidThermalExpansionCoef.Check();
    //liquidCompressibilityCoef.Check();
    gasSpecifHeat.Check();
    gasMassByMole.Check();

    isInit = true;
}

Quantity PhysicalConstants::EstimateSolidNByVolume(Solid material, Volume volume)
{
    return Quantity(solidNByVolume[material] * volume);
}


Volume PhysicalConstants::GetSolidVolumeByN(Solid material, Quantity N)
{
    Volume volume = N / solidNByVolume[material];
    return volume;
}

Quantity PhysicalConstants::EstimateLiquidNByVolume(Liquid material, Volume volume, Scalar pressure, Scalar temperature)
{
    Quantity N0 = liquidNByVolume[material];
    Scalar thermalExpansionCoef = liquidThermalExpansionCoef[material]; // Volume multiplier by K
    Scalar compressibilityCoef = liquidCompressibilityCoef[material];

    Scalar ooStateCoef = (compressibilityCoef + pressure) / ((1. + temperature * thermalExpansionCoef) * compressibilityCoef);

    Quantity N = Quantity(volume * N0 * ooStateCoef);

    return N;
}

Volume PhysicalConstants::GetLiquidVolumeByN(Liquid material, Quantity N, Scalar pressure, Scalar temperature)
{
    Scalar V0 = 1./liquidNByVolume[material];
    Scalar thermalExpansionCoef = liquidThermalExpansionCoef[material]; // Volume multiplier by K
    Scalar compressibilityCoef = liquidCompressibilityCoef[material];

    Scalar stateCoef = (1. + temperature * thermalExpansionCoef) * compressibilityCoef / (compressibilityCoef + pressure);

    Volume volume = Volume(N * V0 * stateCoef);

    return volume;
}


Energy PhysicalConstants::EstimateInternalEnergy(Gas material, Quantity N, Scalar temperature, Scalar volume)
{
    Scalar pressure = ComputeGasPressure(N, volume, temperature);
    Energy elasticEnergy = pressure * volume * PhysicalConstants::gasElasticCoef;
    Energy thermalEnergy = gasSpecifHeat[material] * gasMassByMole[material] * N * temperature;
    return thermalEnergy + elasticEnergy;
}

Quantity PhysicalConstants::EstimateGasN(Volume volume, Scalar pressure, Scalar temperature)
{
    // PV = NRT
    // N = PV / RT
    Quantity N = Quantity(pressure * volume / (gasConstant * temperature));
    return N;
}

Scalar PhysicalConstants::ComputeGasPressure(Quantity N, Volume volume, Scalar temperature)
{
    return N * PhysicalConstants::gasConstant * temperature / volume;
}



// Material PhysicalConstants::GetDissolvedMaterial(Material material)
// {
//     return dissolvedMaterial[material];
// }

Scalar PhysicalConstants::GetSpecificHeatByN(Gas material)
{
    return gasSpecifHeat[material] * gasMassByMole[material]; // TODO cache
}

Scalar PhysicalConstants::GetMassByN(Gas material)
{
    return gasMassByMole[material];
}

}