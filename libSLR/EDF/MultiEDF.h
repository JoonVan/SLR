﻿//
//  MultiEDF.h
//
//  Created by 渡部 心 on 2015/09/05.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#ifndef __SLR_MultiEDF__
#define __SLR_MultiEDF__

#include "../defines.h"
#include "../declarations.h"
#include "../Core/directional_distribution_functions.h"

namespace SLR {
    class SLR_API MultiEDF : public EDF {
#define maxNumEDFElems 4
        uint32_t m_numComponents;
        EDF* m_EDFs[maxNumEDFElems];
        
    public:
        MultiEDF() : EDF(DirectionType()) { }
        
        void add(EDF* edf);
        SampledSpectrum sample(const EDFQuery &query, const EDFSample &smp, EDFQueryResult* result) const override;
        SampledSpectrum evaluate(const EDFQuery &query, const Vector3D &dirOut) const override;
        float evaluatePDF(const EDFQuery &query, const Vector3D &dirOut) const override;
        float weight(const EDFQuery &query) const override;
    };
}

#endif /* __SLR_MultiEDF__ */
