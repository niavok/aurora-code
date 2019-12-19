#ifndef AURORA_PHYSIC_CONSTANTS_H
#define AURORA_PHYSIC_CONSTANTS_H

#include "aurora_physic_types.h"

namespace aurora {

class PhysicalConstants
{
public:
    static constexpr Scalar molePerN = 1.2e-6; // mol.N-1

    static constexpr Scalar gravity = 50; // m.s-2, Acceleration of the gravity
    static constexpr Scalar gasConstantMole = 8.31446261815325; // Gas contant for mol J.mol-1.K-1
    static constexpr Scalar gasConstantN = molePerN * gasConstantMole; // Pressure . volume . K-1, from PV = NRT
    static constexpr Scalar kineticCoef = 0.001; // acceleration ratio of a fluid due to pressure difference

    static void Init();

    static Quantity EstimateSolidNByVolume(Solid material, Volume volume);

    static Volume GetSolidVolumeByN(Solid material, Quantity N);

    static Quantity EstimateLiquidNByVolume(Liquid material, Volume volume, Scalar pressure, Scalar temperature);

    static Volume GetLiquidVolumeByN(Liquid material, Quantity N, Scalar pressure, Scalar temperature);

    static Energy EstimateThermalEnergy(Gas material, Quantity N, Scalar temperature);
    static Energy EstimateThermalEnergy(Liquid material, Quantity N, Scalar temperature);
    static Energy EstimateThermalEnergy(Solid material, Quantity N, Scalar temperature);

    static Quantity EstimateGasN(Volume volume, Scalar pressure, Scalar temperature);
    //static Material GetDissolvedMaterial(Material material);

    static Scalar GetSpecificHeatByN(Gas material);
    static Scalar GetSpecificHeatByN(Liquid material);
    static Scalar GetSpecificHeatByN(Solid material);

    static Scalar GetMassByN(Gas material);
    static Scalar GetMassByN(Liquid material);
    static Scalar GetMassByN(Solid material);

    static Scalar ComputePressure(Quantity N, Volume volume, Scalar temperature);

};

}

#endif
