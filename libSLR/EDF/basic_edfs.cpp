﻿//
//  basic_edfs.cpp
//
//  Created by 渡部 心 on 2015/09/05.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#include "basic_edfs.h"

#include "../Core/distributions.h"

namespace SLR {
    SampledSpectrum DiffuseEDF::sample(const EDFQuery &query, const EDFSample &smp, EDFQueryResult* result) const {
        result->dir_sn = cosineSampleHemisphere(smp.uDir[0], smp.uDir[1]);
        result->dirPDF = result->dir_sn.z / M_PI;
        result->dirType = m_type;
        return 1.0f / M_PI;
    }
    
    SampledSpectrum DiffuseEDF::evaluate(const EDFQuery &query, const Vector3D &dir) const {
        if (!matches(query.flags))
            return SampledSpectrum::Zero;
        return dir.z > 0.0f ? 1.0f / M_PI : 0.0f;
    }
    
    float DiffuseEDF::evaluatePDF(const EDFQuery &query, const Vector3D &dir) const {
        if (!matches(query.flags))
            return 0;
        return dir.z > 0.0f ? dir.z / M_PI : 0.0f;
    }
    
    
    
    SampledSpectrum IdealDirectionalEDF::sample(const EDFQuery &query, const EDFSample &smp, EDFQueryResult *result) const {
        result->dir_sn = m_direction;
        result->dirPDF = 1.0f;
        result->dirType = m_type;
        return SampledSpectrum::One / m_direction.z;
    }
}
