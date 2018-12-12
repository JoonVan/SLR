﻿//
//  MultiBSDF.cpp
//
//  Created by 渡部 心 on 2015/09/05.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#include "MultiBSDF.h"

#include "../Core/distributions.h"

namespace SLR {
    void MultiBSDF::add(const BSDF *bsdf) {
        if (!bsdf)
            return;
        SLRAssert(m_numComponents < MaxNumElems, "Number of MultiBSDF elements exceeds the limit: %u", MaxNumElems);
        m_type |= bsdf->m_type;
        m_BSDFs[m_numComponents++] = bsdf;
    }
    
    SampledSpectrum MultiBSDF::sampleInternalNoRev(const BSDFQuery &query, float uComponent, const float uDir[2], BSDFQueryResult *result) const {
        float weights[MaxNumElems];
        for (int i = 0; i < m_numComponents; ++i)
            weights[i] = m_BSDFs[i]->weight(query);
        
        // JP: 各BSDFのウェイトに基づいて方向のサンプルを行うBSDFを選択する。
        // EN: Based on the weight of each BSDF, select a BSDF from which direction sampling.
        float tempProb;
        float sumWeights;
        uint32_t idx = sampleDiscrete(weights, m_numComponents, uComponent, &tempProb, &sumWeights, &uComponent);
        const BSDF* selectedBSDF = m_BSDFs[idx];
        if (sumWeights == 0.0f) {
            result->dirPDF = 0.0f;
            return SampledSpectrum::Zero;
        }
        
        // JP: 選択したBSDFから方向をサンプリングする。
        // EN: sample a direction from the selected BSDF.
        SampledSpectrum value = selectedBSDF->sampleInternal(query, uComponent, uDir, result);
        result->dirPDF *= weights[idx];
        if (result->dirPDF == 0.0f) {
            result->dirPDF = 0.0f;
            return SampledSpectrum::Zero;
        }
        
        // JP: サンプルした方向に関するBSDFの値の合計と、single-sample model MISに基づいた確率密度を計算する。
        // EN: calculate the total of BSDF values and a PDF based on the single-sample model MIS for the sampled direction.
        if (!result->sampledType.isDelta()) {
            for (int i = 0; i < m_numComponents; ++i) {
                if (i != idx && m_BSDFs[i]->matches(query.dirTypeFilter))
                    result->dirPDF += m_BSDFs[i]->evaluatePDFInternal(query, result->dirLocal, nullptr) * weights[i];
            }
            
            BSDFQuery mQuery = query;
            mQuery.dirTypeFilter &= sideTest(query.gNormalLocal, query.dirLocal, result->dirLocal);
            value = SampledSpectrum::Zero;
            for (int i = 0; i < m_numComponents; ++i) {
                if (!m_BSDFs[i]->matches(mQuery.dirTypeFilter))
                    continue;
                value += m_BSDFs[i]->evaluateInternal(mQuery, result->dirLocal, nullptr);
            }
        }
        result->dirPDF /= sumWeights;
        
        return value;
    }
    
    SampledSpectrum MultiBSDF::sampleInternalWithRev(const BSDFQuery &query, float uComponent, const float uDir[2], BSDFQueryResult *result) const {
        float weights[MaxNumElems];
        for (int i = 0; i < m_numComponents; ++i)
            weights[i] = m_BSDFs[i]->weight(query);
        
        // JP: 各BSDFのウェイトに基づいて方向のサンプルを行うBSDFを選択する。
        // EN: Based on the weight of each BSDF, select a BSDF from which direction sampling.
        float tempProb;
        float sumWeights;
        uint32_t idx = sampleDiscrete(weights, m_numComponents, uComponent, &tempProb, &sumWeights, &uComponent);
        const BSDF* selectedBSDF = m_BSDFs[idx];
        if (sumWeights == 0.0f) {
            result->dirPDF = 0.0f;
            return SampledSpectrum::Zero;
        }
        
        // JP: 選択したBSDFから方向をサンプリングする。
        // EN: sample a direction from the selected BSDF.
        SampledSpectrum value = selectedBSDF->sampleInternal(query, uComponent, uDir, result);
        if (result->dirPDF == 0.0f) {
            result->dirPDF = 0.0f;
            return SampledSpectrum::Zero;
        }
        
        // JP: 逆方向の確率密度を求めるための諸量を計算する。
        // EN: calculate quantities for reverse probability density.
        BSDFQuery revQuery = query;// mQuery?
        Vector3D revDirIn = result->dirLocal;
        std::swap(revQuery.dirLocal, revDirIn);
        revQuery.adjoint ^= true;
        float revWeights[MaxNumElems];
        FloatSum sumRevWeights = 0;
        for (int i = 0; i < m_numComponents; ++i) {
            revWeights[i] = m_BSDFs[i]->weight(revQuery);
            sumRevWeights += revWeights[i];
        }
        
        result->dirPDF *= weights[idx];
        result->reverse.dirPDF *= revWeights[idx];
        
        // JP: サンプルした方向に関するBSDFの値の合計と、single-sample model MISに基づいた確率密度を計算する。
        // EN: calculate the total of BSDF values and a PDF based on the single-sample model MIS for the sampled direction.
        if (!result->sampledType.isDelta()) {
            for (int i = 0; i < m_numComponents; ++i) {
                float revPDF;
                if (i != idx && m_BSDFs[i]->matches(query.dirTypeFilter)) {
                    result->dirPDF += m_BSDFs[i]->evaluatePDFInternal(query, result->dirLocal, &revPDF) * weights[i];
                    result->reverse.dirPDF += revPDF * revWeights[i];
                }
            }
            
            BSDFQuery mQuery = query;
            mQuery.dirTypeFilter &= sideTest(query.gNormalLocal, query.dirLocal, result->dirLocal);
            value = SampledSpectrum::Zero;
            result->reverse.value = SampledSpectrum::Zero;
            
            for (int i = 0; i < m_numComponents; ++i) {
                if (!m_BSDFs[i]->matches(mQuery.dirTypeFilter))
                    continue;
                SampledSpectrum eRev_fs;
                value += m_BSDFs[i]->evaluateInternal(mQuery, result->dirLocal, &eRev_fs);
                result->reverse.value += eRev_fs;
            }
        }
        result->dirPDF /= sumWeights;
        result->reverse.dirPDF /= sumRevWeights;
        
        return value;
    }
    
    SampledSpectrum MultiBSDF::evaluateInternal(const BSDFQuery &query, const Vector3D &dirOut, SampledSpectrum* rev_fs) const {
        if (query.requestReverse) {
            SampledSpectrum retValue = SampledSpectrum::Zero;
            *rev_fs = SampledSpectrum::Zero;
            for (int i = 0; i < m_numComponents; ++i) {
                if (!m_BSDFs[i]->matches(query.dirTypeFilter))
                    continue;
                SampledSpectrum eRev_fs;
                retValue += m_BSDFs[i]->evaluateInternal(query, dirOut, &eRev_fs);
                *rev_fs += eRev_fs;
            }
            return retValue;
        }
        else {
            SampledSpectrum retValue = SampledSpectrum::Zero;
            for (int i = 0; i < m_numComponents; ++i) {
                if (!m_BSDFs[i]->matches(query.dirTypeFilter))
                    continue;
                retValue += m_BSDFs[i]->evaluateInternal(query, dirOut, nullptr);
            }
            return retValue;
        }
    }
    
    float MultiBSDF::evaluatePDFInternalNoRev(const BSDFQuery &query, const Vector3D &dirOut, float* revPDF) const {
        FloatSum sumWeights = 0.0f;
        float weights[MaxNumElems];
        for (int i = 0; i < m_numComponents; ++i) {
            weights[i] = m_BSDFs[i]->weight(query);
            sumWeights += weights[i];
        }
        if (sumWeights == 0.0f)
            return 0.0f;
        
        float retPDF = 0.0f;
        for (int i = 0; i < m_numComponents; ++i) {
            if (weights[i] > 0)
                retPDF += m_BSDFs[i]->evaluatePDFInternal(query, dirOut, nullptr) * weights[i];
        }
        retPDF /= sumWeights;
        
        return retPDF;
    }
    
    float MultiBSDF::evaluatePDFInternalWithRev(const BSDFQuery &query, const Vector3D &dirOut, float* revPDF) const {
        BSDFQuery revQuery = query;// mQuery?
        Vector3D revDirIn = dirOut;
        std::swap(revQuery.dirLocal, revDirIn);
        revQuery.adjoint ^= true;
        
        FloatSum sumWeights = 0.0f;
        FloatSum sumRevWeights = 0.0f;
        float weights[MaxNumElems];
        float revWeights[MaxNumElems];
        for (int i = 0; i < m_numComponents; ++i) {
            weights[i] = m_BSDFs[i]->weight(query);
            sumWeights += weights[i];
            revWeights[i] = m_BSDFs[i]->weight(revQuery);
            sumRevWeights += revWeights[i];
        }
        if (sumWeights == 0.0f) {
            *revPDF = 0.0f;
            return 0.0f;
        }
        
        float retPDF = 0.0f;
        *revPDF = 0.0f;
        for (int i = 0; i < m_numComponents; ++i) {
            if (weights[i] > 0) {
                float eRevPDF;
                retPDF += m_BSDFs[i]->evaluatePDFInternal(query, dirOut, &eRevPDF) * weights[i];
                *revPDF += eRevPDF * revWeights[i];
            }
        }
        retPDF /= sumWeights;
        *revPDF /= sumRevWeights;
        
        return retPDF;
    }
    
    float MultiBSDF::weightInternal(const BSDFQuery &query) const {
        FloatSum sumWeights = 0.0f;
        for (int i = 0; i < m_numComponents; ++i)
            sumWeights += m_BSDFs[i]->weight(query);
        return sumWeights;
    }
    
    SampledSpectrum MultiBSDF::getBaseColorInternal(DirectionType flags) const {
        SampledSpectrum baseColor = SampledSpectrum::Zero;
        for (int i = 0; i < m_numComponents; ++i) {
            baseColor = m_BSDFs[i]->getBaseColor(flags);
            if (baseColor.hasNonZero())
                break;
        }
        return baseColor;
    }
}
