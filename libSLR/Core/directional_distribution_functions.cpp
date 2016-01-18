//
//  directional_distribution_functions.cpp
//
//  Created by 渡部 心 on 2015/06/28.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#include "directional_distribution_functions.h"

namespace SLR {
    const DirectionType DirectionType::LowFreq = DirectionType::IE_LowFreq;
    const DirectionType DirectionType::HighFreq = DirectionType::IE_HighFreq;
    const DirectionType DirectionType::Delta0D = DirectionType::IE_Delta0D;
    const DirectionType DirectionType::Delta1D = DirectionType::IE_Delta1D;
    const DirectionType DirectionType::Dispersive = DirectionType::IE_Dispersive;
    const DirectionType DirectionType::NonDelta = DirectionType::IE_NonDelta;
    const DirectionType DirectionType::Delta = DirectionType::IE_Delta;
    const DirectionType DirectionType::AllFreq = DirectionType::IE_AllFreq;
    const DirectionType DirectionType::Reflection = DirectionType::IE_Reflection;
    const DirectionType DirectionType::Transmission = DirectionType::IE_Transmission;
    const DirectionType DirectionType::Emission = DirectionType::IE_Emission;
    const DirectionType DirectionType::Acquisition = DirectionType::IE_Acquisition;
    const DirectionType DirectionType::WholeSphere = DirectionType::IE_WholeSphere;
    const DirectionType DirectionType::All = DirectionType::IE_All;
    const DirectionType DirectionType::LowFreqReflection = DirectionType::IE_LowFreqReflection;
    const DirectionType DirectionType::LowFreqTransmission = DirectionType::IE_LowFreqTransmission;
    const DirectionType DirectionType::LowFreqScattering = DirectionType::IE_LowFreqScattering;
    const DirectionType DirectionType::HighFreqReflection = DirectionType::IE_HighFreqReflection;
    const DirectionType DirectionType::HighFreqTransmission = DirectionType::IE_HighFreqTransmission;
    const DirectionType DirectionType::HighFreqScattering = DirectionType::IE_HighFreqScattering;
    const DirectionType DirectionType::Delta0DReflection = DirectionType::IE_Delta0DReflection;
    const DirectionType DirectionType::Delta0DTransmission = DirectionType::IE_Delta0DTransmission;
    const DirectionType DirectionType::Delta0DScattering = DirectionType::IE_Delta0DScattering;
    
    SampledSpectrum FresnelNoOp::evaluate(float cosEnter) const {
        return SampledSpectrum::One;
    }
    
    SampledSpectrum FresnelConductor::evaluate(float cosEnter) const {
        cosEnter = std::fabs(cosEnter);
        float cosEnter2 = cosEnter * cosEnter;
        SampledSpectrum _2EtaCosEnter = 2.0f * m_eta * cosEnter;
        SampledSpectrum tmp_f = m_eta * m_eta + m_k * m_k;
        SampledSpectrum tmp = tmp_f * cosEnter2;
        SampledSpectrum Rparl2 = (tmp - _2EtaCosEnter + 1) / (tmp + _2EtaCosEnter + 1);
        SampledSpectrum Rperp2 = (tmp_f - _2EtaCosEnter + cosEnter2) / (tmp_f + _2EtaCosEnter + cosEnter2);
        return (Rparl2 + Rperp2) / 2.0f;
    }
    
    SampledSpectrum FresnelDielectric::evaluate(float cosEnter) const {
        cosEnter = std::clamp(cosEnter, -1.0f, 1.0f);
        
        bool entering = cosEnter > 0.0f;
        const SampledSpectrum &eEnter = entering ? m_etaExt : m_etaInt;
        const SampledSpectrum &eExit = entering ? m_etaInt : m_etaExt;
        
        SampledSpectrum sinExit = eEnter / eExit * std::sqrt(std::fmax(0.0f, 1.0f - cosEnter * cosEnter));
        SampledSpectrum ret = SampledSpectrum::Zero;
        cosEnter = std::fabs(cosEnter);
        for (int i = 0; i < SampledSpectrum::NumComponents; ++i) {
            if (sinExit[i] >= 1.0f) {
                ret[i] = 1.0f;
            }
            else {
                float cosExit = std::sqrt(std::fmax(0.0f, 1.0f - sinExit[i] * sinExit[i]));
                ret[i] = evalF(eEnter[i], eExit[i], cosEnter, cosExit);
            }
        }
        return ret;
    }
    
    float FresnelDielectric::evalF(float etaEnter, float etaExit, float cosEnter, float cosExit) {
        float Rparl = ((etaExit * cosEnter) - (etaEnter * cosExit)) / ((etaExit * cosEnter) + (etaEnter * cosExit));
        float Rperp = ((etaEnter * cosEnter) - (etaExit * cosExit)) / ((etaEnter * cosEnter) + (etaExit * cosExit));
        return (Rparl * Rparl + Rperp * Rperp) / 2.0f;
    }    
}
