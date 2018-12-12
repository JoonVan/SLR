﻿//
//  AshikhminShirleyBRDF.cpp
//
//  Created by 渡部 心 on 2015/09/05.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#include "AshikhminShirleyBRDF.h"

#include "../Core/distributions.h"

namespace SLR {
    SampledSpectrum AshikhminShirleyBRDF::sampleInternal(const BSDFQuery &query, float uComponent, const float uDir[2], BSDFQueryResult *result) const {
        float iRs = m_Rs.importance(query.wlHint);
        float iRd = m_Rd.importance(query.wlHint);
        float vDotHV = std::fabs(query.dirLocal.z);
        float specularWeight = iRs + (1 - iRs) * std::pow(1.0f - vDotHV, 5);
        float transmissionTerm = 1 - std::pow(1 - vDotHV * 0.5f, 5);
        float diffuseWeght = 28 * iRd / 23 * (1 - iRs) * transmissionTerm * transmissionTerm;
        float sumWeights = specularWeight + diffuseWeght;
        
        float specularDirPDF, diffuseDirPDF;
        float revSpecularDirPDF, revDiffuseDirPDF;
        float dotHV;
        float commonTerm;
        SampledSpectrum fs;
        if (uComponent * sumWeights < specularWeight) {
            result->sampledType = DirectionType::Reflection | DirectionType::HighFreq;
            
            // JP: ハーフベクトルをサンプルして、最終的な方向サンプルを生成する。
            // EN: sample a half vector, then generate a resulting direction sample based on it.
            float quad = 2 * M_PI * uDir[1];
            float phi_h = std::atan2(std::sqrt(m_nu + 1) * std::sin(quad), std::sqrt(m_nv + 1) * std::cos(quad));
            float cosphi = std::cos(phi_h);
            float sinphi = std::sin(phi_h);
            float theta_h = std::acos(std::pow(1 - uDir[0], 1.0f / (m_nu * cosphi * cosphi + m_nv * sinphi * sinphi + 1)));
            if (query.dirLocal.z < 0)
                theta_h = M_PI - theta_h;
            Vector3D halfv = Vector3D(std::sin(theta_h) * std::cos(phi_h), std::sin(theta_h) * std::sin(phi_h), std::cos(theta_h));
            result->dirLocal = 2 * dot(query.dirLocal, halfv) * halfv - query.dirLocal;
            if (result->dirLocal.z * query.dirLocal.z <= 0) {
                result->dirPDF = 0.0f;
                return SampledSpectrum::Zero;
            }
            
            dotHV = dot(halfv, query.dirLocal);
            float exp = (m_nu * halfv.x * halfv.x + m_nv * halfv.y * halfv.y) / (1 - halfv.z * halfv.z);
            commonTerm = std::sqrt((m_nu + 1) * (m_nv + 1)) / (8 * M_PI * dotHV) * std::pow(std::fabs(halfv.z), exp);
            specularDirPDF = commonTerm;
            revSpecularDirPDF = specularDirPDF;
            
            // JP: ディフューズ要素からサンプル方向を生成する確率密度を計算する。
            // EN: calculate PDF to generate the result direction by sampling from diffuse component.
            diffuseDirPDF = std::fabs(result->dirLocal.z) / M_PI;
            revDiffuseDirPDF = std::fabs(query.dirLocal.z) / M_PI;
        }
        else {
            result->sampledType = DirectionType::Reflection | DirectionType::LowFreq;
            
            // JP: コサイン分布から方向サンプルを生成する。
            // EN: generate a direction sample from the cosine distribution.
            result->dirLocal = cosineSampleHemisphere(uDir[0], uDir[1]);
            diffuseDirPDF = result->dirLocal.z / M_PI;
            revDiffuseDirPDF = std::fabs(query.dirLocal.z) / M_PI;
            result->dirLocal.z *= dot(query.dirLocal, query.gNormalLocal) > 0 ? 1 : -1;
            
            Vector3D halfv = halfVector(query.dirLocal, result->dirLocal);
            
            // JP: スペキュラー要素からサンプル方向を生成する確率密度を計算する。
            // EN: calculate PDF to generate the result direction by sampling from specular component.
            dotHV = dot(halfv, query.dirLocal);
            float exp = (m_nu * halfv.x * halfv.x + m_nv * halfv.y * halfv.y) / (1 - halfv.z * halfv.z);
            commonTerm = std::sqrt((m_nu + 1) * (m_nv + 1)) / (8 * M_PI * dotHV) * std::pow(std::fabs(halfv.z), exp);
            specularDirPDF = commonTerm;
            revSpecularDirPDF = specularDirPDF;
        }
        SampledSpectrum diffuse_fs = (28 * m_Rd / (23 * M_PI) * (SampledSpectrum::One - m_Rs) *
                                      (1.0f - std::pow(1.0f - std::fabs(query.dirLocal.z) / 2, 5)) *
                                      (1.0f - std::pow(1.0f - std::fabs(result->dirLocal.z) / 2, 5))
                                      );
        SampledSpectrum F = m_Rs + (SampledSpectrum::One - m_Rs) * std::pow(1.0f - dotHV, 5);
        SampledSpectrum specular_fs = commonTerm / std::fmax(std::fabs(query.dirLocal.z), std::fabs(result->dirLocal.z)) * F;
        
        fs = specular_fs + diffuse_fs;
        
        // PDF based on the single-sample model MIS.
        result->dirPDF = (specularDirPDF * specularWeight + diffuseDirPDF * diffuseWeght) / sumWeights;
        if (query.requestReverse) {
            float revVDotHV = std::fabs(result->dirLocal.z);
            float revSpecularWeight = iRs + (1 - iRs) * std::pow(1.0f - revVDotHV, 5);
            float revTransmissionTerm = 1 - std::pow(1 - revVDotHV * 0.5f, 5);
            float revDiffuseWeight = 28 * iRd / 23 * (1 - iRs) * revTransmissionTerm * revTransmissionTerm;
            float revSumWeights = revSpecularWeight + revDiffuseWeight;
            
            result->reverse.value = fs;
            result->reverse.dirPDF = (revSpecularDirPDF * revSpecularWeight + revDiffuseDirPDF * revDiffuseWeight) / revSumWeights;
        }
        return fs;
    }
    
    SampledSpectrum AshikhminShirleyBRDF::evaluateInternal(const BSDFQuery &query, const Vector3D &dir, SampledSpectrum* rev_fs) const {
        if (dir.z * query.dirLocal.z <= 0) {
            if (query.requestReverse)
                *rev_fs = SampledSpectrum::Zero;
            return SampledSpectrum::Zero;
        }
        Vector3D halfv = halfVector(query.dirLocal, dir);
        float dotHV = dot(halfv, query.dirLocal);
        float exp = (m_nu * halfv.x * halfv.x + m_nv * halfv.y * halfv.y) / (1 - halfv.z * halfv.z);
        float commonTerm = std::sqrt((m_nu + 1) * (m_nv + 1)) / (8 * M_PI * dotHV) * std::pow(std::fabs(halfv.z), exp);
        SampledSpectrum F = m_Rs + (SampledSpectrum::One - m_Rs) * std::pow(1.0f - dotHV, 5);
        SampledSpectrum specular_fs = commonTerm / std::fmax(std::fabs(query.dirLocal.z), std::fabs(dir.z)) * F;
        SampledSpectrum diffuse_fs = (28 * m_Rd / (23 * M_PI) * (SampledSpectrum::One - m_Rs) *
                                      (1.0f - std::pow(1.0f - std::fabs(query.dirLocal.z) / 2, 5)) *
                                      (1.0f - std::pow(1.0f - std::fabs(dir.z) / 2, 5))
                                      );
        SampledSpectrum fs = specular_fs + diffuse_fs;
        if (query.requestReverse)
            *rev_fs = fs;
        return fs;
    }
    
    float AshikhminShirleyBRDF::evaluatePDFInternal(const BSDFQuery &query, const Vector3D &dir, float* revPDF) const {
        if (dir.z * query.dirLocal.z <= 0) {
            if (query.requestReverse)
                *revPDF = 0.0f;
            return 0.0f;
        }
        
        Vector3D halfv = halfVector(query.dirLocal, dir);
        float dotHV = dot(halfv, query.dirLocal);
        float exp = (m_nu * halfv.x * halfv.x + m_nv * halfv.y * halfv.y) / (1 - halfv.z * halfv.z);
        float specularDirPDF = std::sqrt((m_nu + 1) * (m_nv + 1)) / (8 * M_PI * dotHV) * std::pow(std::fabs(halfv.z), exp);
        float revSpecularDirPDF = specularDirPDF;
        
        float diffuseDirPDF = std::fabs(dir.z) / M_PI;
        float revDiffuseDirPDF = std::fabs(query.dirLocal.z) / M_PI;
        
        float iRs = m_Rs.importance(query.wlHint);
        float iRd = m_Rd.importance(query.wlHint);
        float vDotHV = std::fabs(query.dirLocal.z);
        float specularWeight = iRs + (1 - iRs) * std::pow(1.0f - vDotHV, 5);
        float transmissionTerm = 1 - std::pow(1 - vDotHV * 0.5f, 5);
        float diffuseWeght = 28 * iRd / 23 * (1 - iRs) * transmissionTerm * transmissionTerm;
        float sumWeights = specularWeight + diffuseWeght;
        
        if (query.requestReverse) {
            float revVDotHV = std::fabs(dir.z);
            float revSpecularWeight = iRs + (1 - iRs) * std::pow(1.0f - revVDotHV, 5);
            float revTransmissionTerm = 1 - std::pow(1 - revVDotHV * 0.5f, 5);
            float revDiffuseWeght = 28 * iRd / 23 * (1 - iRs) * revTransmissionTerm * revTransmissionTerm;
            float revSumWeights = revSpecularWeight + revDiffuseWeght;
            
            *revPDF = (revSpecularDirPDF * revSpecularWeight + revDiffuseDirPDF * revDiffuseWeght) / revSumWeights;
        }
        return (specularDirPDF * specularWeight + diffuseDirPDF * diffuseWeght) / sumWeights;
    }
    
    float AshikhminShirleyBRDF::weightInternal(const BSDFQuery &query) const {
        // assume the half vector is the same to the normal.
        float iRs = m_Rs.importance(query.wlHint);
        float iRd = m_Rd.importance(query.wlHint);
        float dotHV = std::fabs(query.dirLocal.z);
        float specularWeight = iRs + (1 - iRs) * std::pow(1.0f - dotHV, 5);
        float transmissionTerm = 1 - std::pow(1 - dotHV * 0.5f, 5);
        float diffuseWeght = 28 * iRd / 23 * (1 - iRs) * transmissionTerm * transmissionTerm;
        return specularWeight + diffuseWeght;
    }
    
    SampledSpectrum AshikhminShirleyBRDF::getBaseColorInternal(DirectionType flags) const {
        return m_Rs + (SampledSpectrum::One - m_Rs) * m_Rd;
    }
}
