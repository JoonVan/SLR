﻿//
//  camera.h
//
//  Created by 渡部 心 on 2015/05/30.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#ifndef __SLR_camera__
#define __SLR_camera__

#include "../defines.h"
#include "../declarations.h"
#include "geometry.h"
#include "directional_distribution_functions.h"

namespace SLR {
    struct SLR_API LensPosQuery {
        float time;
        WavelengthSamples wls;
        LensPosQuery(float t, const WavelengthSamples &lambdas) : time(t), wls(lambdas) { };
    };
    
    struct SLR_API LensPosSample {
        float uPos[2];
        LensPosSample(float up0, float up1) : uPos{up0, up1} { };
    };
    
    struct SLR_API LensPosQueryResult {
        SurfacePoint surfPt;
        float areaPDF;
        DirectionType posType;
    };
    
    
    
    class SLR_API Camera {
    protected:
        const Transform* m_transform;
    public:
        Camera(const Transform* l2w) : m_transform(l2w) { };
        virtual ~Camera() { };
        
        void setTransform(const Transform* t);
        
        virtual ImageSensor* getSensor() const = 0;
        
        virtual SampledSpectrum sample(const LensPosQuery &query, const LensPosSample &smp, LensPosQueryResult* result) const = 0;
        virtual IDF* createIDF(const SurfacePoint &surfPt, const WavelengthSamples &wls, ArenaAllocator &mem) const = 0;
        virtual void sampleRay(const LensPosQuery &lensQuery, const LensPosSample &lensSample,
                               const IDFSample &WeSample, 
                               ArenaAllocator &mem, 
                               LensPosQueryResult* lensResult, SampledSpectrum* We0, IDF** idf, 
                               IDFQueryResult* WeResult, SampledSpectrum* We1, Ray* ray, float* epsilon) const {
            // JP: レンズ面上の位置(We0, 位置に関するimportance)をサンプルする。
            // EN: sample a position (We0, spatial importance) on the lens surface of the camera.
            *We0 = sample(lensQuery, lensSample, lensResult);
            *idf = createIDF(lensResult->surfPt, lensQuery.wls, mem);
            SLRAssert(std::isfinite(lensResult->areaPDF), "areaPDF: unexpected value detected: %f", lensResult->areaPDF);

            // JP: IDFから方向(方向に関するimportance)をサンプルする。
            // EN: sample a direction (directional importance) from IDF.
            *We1 = (*idf)->sample(WeSample, WeResult);
            *ray = Ray(lensResult->surfPt.getPosition(), lensResult->surfPt.fromLocal(WeResult->dirLocal), lensQuery.time);
            *epsilon = 0.0f;
        }
    };    
}

#endif /* __SLR_camera__ */
