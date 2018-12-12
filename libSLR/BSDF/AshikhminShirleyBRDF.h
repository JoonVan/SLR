﻿//
//  AshikhminShirleyBRDF.h
//
//  Created by 渡部 心 on 2015/09/05.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#ifndef __SLR_AshikhminShirleyBRDF__
#define __SLR_AshikhminShirleyBRDF__

#include "../defines.h"
#include "../declarations.h"
#include "../Core/directional_distribution_functions.h"

namespace SLR {
    // References
    // An Anisotropic Phong BRDF Model
    class SLR_API AshikhminShirleyBRDF : public BSDF {
        SampledSpectrum m_Rs;
        SampledSpectrum m_Rd;
        float m_nu, m_nv;
        
        SampledSpectrum sampleInternal(const BSDFQuery &query, float uComponent, const float uDir[2], BSDFQueryResult* result) const override;
        SampledSpectrum evaluateInternal(const BSDFQuery &query, const Vector3D &dir, SampledSpectrum* rev_fs) const override;
        float evaluatePDFInternal(const BSDFQuery &query, const Vector3D &dir, float* revPDF) const override;
        float weightInternal(const BSDFQuery &query) const override;
        SampledSpectrum getBaseColorInternal(DirectionType flags) const override;
    public:
        AshikhminShirleyBRDF(const SampledSpectrum &Rs, const SampledSpectrum &Rd, float nu, float nv) : BSDF(DirectionType::Reflection | DirectionType::HighFreq | DirectionType::LowFreq),
        m_Rs(Rs), m_Rd(Rd), m_nu(nu), m_nv(nv) { }
    };
}

#endif /* __SLR_AshikhminShirleyBRDF__ */
