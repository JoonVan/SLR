﻿//
//  directional_distribution_functions.cpp
//
//  Created by 渡部 心 on 2015/06/28.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#include "directional_distribution_functions.h"

#include "../MemoryAllocators/ArenaAllocator.h"
#include "../Core/light_path_sampler.h"
#include "distributions.h"

namespace SLR {
    SLR_API const DirectionType DirectionType::LowFreq = DirectionType::IE_LowFreq;
    SLR_API const DirectionType DirectionType::HighFreq = DirectionType::IE_HighFreq;
    SLR_API const DirectionType DirectionType::Delta0D = DirectionType::IE_Delta0D;
    SLR_API const DirectionType DirectionType::Delta1D = DirectionType::IE_Delta1D;
    SLR_API const DirectionType DirectionType::NonDelta = DirectionType::IE_NonDelta;
    SLR_API const DirectionType DirectionType::Delta = DirectionType::IE_Delta;
    SLR_API const DirectionType DirectionType::AllFreq = DirectionType::IE_AllFreq;
    SLR_API const DirectionType DirectionType::Reflection = DirectionType::IE_Reflection;
    SLR_API const DirectionType DirectionType::Transmission = DirectionType::IE_Transmission;
    SLR_API const DirectionType DirectionType::Emission = DirectionType::IE_Emission;
    SLR_API const DirectionType DirectionType::Acquisition = DirectionType::IE_Acquisition;
    SLR_API const DirectionType DirectionType::WholeSphere = DirectionType::IE_WholeSphere;
    SLR_API const DirectionType DirectionType::All = DirectionType::IE_All;
    SLR_API const DirectionType DirectionType::Dispersive = DirectionType::IE_Dispersive;
    SLR_API const DirectionType DirectionType::LowFreqReflection = DirectionType::IE_LowFreqReflection;
    SLR_API const DirectionType DirectionType::LowFreqTransmission = DirectionType::IE_LowFreqTransmission;
    SLR_API const DirectionType DirectionType::LowFreqScattering = DirectionType::IE_LowFreqScattering;
    SLR_API const DirectionType DirectionType::HighFreqReflection = DirectionType::IE_HighFreqReflection;
    SLR_API const DirectionType DirectionType::HighFreqTransmission = DirectionType::IE_HighFreqTransmission;
    SLR_API const DirectionType DirectionType::HighFreqScattering = DirectionType::IE_HighFreqScattering;
    SLR_API const DirectionType DirectionType::Delta0DReflection = DirectionType::IE_Delta0DReflection;
    SLR_API const DirectionType DirectionType::Delta0DTransmission = DirectionType::IE_Delta0DTransmission;
    SLR_API const DirectionType DirectionType::Delta0DScattering = DirectionType::IE_Delta0DScattering;
    
    
    SampledSpectrum BSDF::rho(uint32_t numSamples, BSDFSample* samples, float* uDir0, float* uDir1, float* uWl, DirectionType flags, bool fromUpper) const {
        SampledSpectrumSum ret(SampledSpectrum::Zero);
        uint32_t numFails = 0;
        for (int i = 0; i < numSamples; ++i) {
            Vector3D dir_sn = uniformSampleHemisphere(uDir0[i], uDir1[i]);
            dir_sn.z *= fromUpper ? 1 : -1;
            float dirPDF = 1.0f / (2 * M_PI);
            
            int16_t wlIdx = std::min(int16_t(SampledSpectrum::NumComponents * uWl[i]), int16_t(SampledSpectrum::NumComponents - 1));
            BSDFQuery query{dir_sn, Normal3D(0, 0, 1), wlIdx, flags, false};
            
            BSDFQueryResult fsResult;
            SampledSpectrum fs = sample(query, samples[i], &fsResult);
            if (fsResult.dirPDF == 0.0f) {
                ++numFails;
                continue;
            }
            ret += fs * std::fabs(fsResult.dirLocal.z) * std::fabs(dir_sn.z) / (dirPDF * fsResult.dirPDF);
        }
        return ret.result / (M_PI * (numSamples - numFails));
    }
    
    SampledSpectrum BSDF::sample(const ABDFQuery* query, LightPathSampler &sampler, ArenaAllocator &mem, ABDFQueryResult** result) const {
        BSDFQueryResult* concreteResult = mem.create<BSDFQueryResult>();
        // MEMO: *(const BSDFQuery*)query doesn't seem a good way.
        SampledSpectrum ret = sample(*(const BSDFQuery*)query, sampler.getBSDFSample(), concreteResult);
        *result = concreteResult;
        return ret;
    }
    
    SampledSpectrum BSDF::evaluate(const ABDFQuery* query, const Vector3D &dir, SampledSpectrum* rev_fs) const {
        return evaluate(*(const BSDFQuery*)query, dir, rev_fs);
    }
    
    float BSDF::evaluatePDF(const ABDFQuery* query, const Vector3D &dir, float* revPDF) const {
        return evaluatePDF(*(const BSDFQuery*)query, dir, revPDF);
    }
    
    
    
    SampledSpectrum VolumetricBSDF::sample(const ABDFQuery* query, LightPathSampler &sampler, ArenaAllocator &mem, ABDFQueryResult** result) const {
        PFQueryResult concreteResult;
        PFQuery pfQuery(-query->dirLocal, query->wlHint, query->dirTypeFilter, query->requestReverse);
        SampledSpectrum ret = m_albedo * m_pf->sample(pfQuery, sampler.getPFSample(), &concreteResult);
        *result = mem.create<VolumetricBSDFQueryResult>(concreteResult);
        if (pfQuery.requestReverse) {
            (*result)->reverse.value = ret;
            (*result)->reverse.dirPDF = concreteResult.revDirPDF;
        }
        return ret;
    }
    
    SampledSpectrum VolumetricBSDF::evaluate(const ABDFQuery* query, const Vector3D &dir, SampledSpectrum* rev_fs) const {
        PFQuery pfQuery(-query->dirLocal, query->wlHint, query->dirTypeFilter, query->requestReverse);
        SampledSpectrum ret = m_albedo * m_pf->evaluate(pfQuery, dir);
        // assume phase functions are symmetric.
        if (pfQuery.requestReverse)
            *rev_fs = ret;
        return ret;
    }
    
    float VolumetricBSDF::evaluatePDF(const ABDFQuery* query, const Vector3D &dir, float* revPDF) const {
        PFQuery pfQuery(-query->dirLocal, query->wlHint, query->dirTypeFilter, query->requestReverse);
        return m_pf->evaluatePDF(pfQuery, dir, revPDF);
    }
    
    
    
    SampledSpectrum FresnelNoOp::evaluate(float cosEnter) const {
        return SampledSpectrum::One;
    }
    
    float FresnelNoOp::evaluate(float cosEnter, uint32_t wlIdx) const {
        return 1.0f;
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
    
    float FresnelConductor::evaluate(float cosEnter, uint32_t wlIdx) const {
        cosEnter = std::fabs(cosEnter);
        float cosEnter2 = cosEnter * cosEnter;
        float _2EtaCosEnter = 2.0f * m_eta[wlIdx] * cosEnter;
        float tmp_f = m_eta[wlIdx] * m_eta[wlIdx] + m_k[wlIdx] * m_k[wlIdx];
        float tmp = tmp_f * cosEnter2;
        float Rparl2 = (tmp - _2EtaCosEnter + 1) / (tmp + _2EtaCosEnter + 1);
        float Rperp2 = (tmp_f - _2EtaCosEnter + cosEnter2) / (tmp_f + _2EtaCosEnter + cosEnter2);
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
        
//        cosEnter = std::fabs(std::clamp(cosEnter, -1.0f, 1.0f));
//        
//        bool entering = cosEnter > 0.0f;
//        const SampledSpectrum &eEnter = entering ? m_etaExt : m_etaInt;
//        const SampledSpectrum &eExit = entering ? m_etaInt : m_etaExt;
//        
//        SampledSpectrum relIOR = eExit / eEnter;
//        SampledSpectrum g2 = relIOR * relIOR - SampledSpectrum::One + cosEnter * cosEnter;
//        
//        SampledSpectrum ret = SampledSpectrum::Zero;
//        for (int i = 0; i < SampledSpectrum::NumComponents; ++i) {
//            if (g2[i] < 0) {
//                ret[i] = 1.0f;
//            }
//            else {
//                float g_minus_c = std::sqrt(g2[i]) - cosEnter;
//                float g_plus_c = std::sqrt(g2[i]) + cosEnter;
//                float termN = cosEnter * g_plus_c - 1;
//                float termD = cosEnter * g_minus_c + 1;
//                
//                ret[i] = 0.5f * (g_minus_c * g_minus_c) / (g_plus_c * g_plus_c) * (1 + (termN * termN) / (termD * termD));
//            }
//        }
//        return ret;
    }
    
    float FresnelDielectric::evaluate(float cosEnter, uint32_t wlIdx) const {
        cosEnter = std::clamp(cosEnter, -1.0f, 1.0f);
        
        bool entering = cosEnter > 0.0f;
        const float &eEnter = entering ? m_etaExt[wlIdx] : m_etaInt[wlIdx];
        const float &eExit = entering ? m_etaInt[wlIdx] : m_etaExt[wlIdx];
        
        float sinExit = eEnter / eExit * std::sqrt(std::fmax(0.0f, 1.0f - cosEnter * cosEnter));
        cosEnter = std::fabs(cosEnter);
        if (sinExit >= 1.0f) {
            return 1.0f;
        }
        else {
            float cosExit = std::sqrt(std::fmax(0.0f, 1.0f - sinExit * sinExit));
            return evalF(eEnter, eExit, cosEnter, cosExit);
        }
    }
    
    float FresnelDielectric::evalF(float etaEnter, float etaExit, float cosEnter, float cosExit) {
        float Rparl = ((etaExit * cosEnter) - (etaEnter * cosExit)) / ((etaExit * cosEnter) + (etaEnter * cosExit));
        float Rperp = ((etaEnter * cosEnter) - (etaExit * cosExit)) / ((etaEnter * cosEnter) + (etaExit * cosExit));
        return (Rparl * Rparl + Rperp * Rperp) / 2.0f;
    }
    
    
    
    float BerryMicrofacetDistribution::sample(float u0, float u1, Normal3D *m, float *normalPDF) const {
        float alpha2 = m_alpha_g * m_alpha_g;
        float cosTheta_m = std::sqrt((1 - std::pow(alpha2, 1 - u0)) / (1 - alpha2));
        float theta_m = std::acos(cosTheta_m);
        float phi_m = 2 * M_PI * u1;
        *m = Normal3D(std::sin(theta_m) * std::cos(phi_m), std::sin(theta_m) * std::sin(phi_m), std::cos(theta_m));
        
        float ret = (alpha2 - 1) / (M_PI * std::log(alpha2)) / (1 + (alpha2 - 1) * cosTheta_m * cosTheta_m);
        *normalPDF = ret * m->z;
        
        return ret;
    }
    
    float BerryMicrofacetDistribution::evaluate(const Normal3D &m) const {
        if (m.z <= 0)
            return 0.0f;
        float alpha2 = m_alpha_g * m_alpha_g;
        float cosTheta_m = m.z;
        return (alpha2 - 1) / (M_PI * std::log(alpha2)) / (1 + (alpha2 - 1) * cosTheta_m * cosTheta_m);
    }
    
    float BerryMicrofacetDistribution::evaluatePDF(const Normal3D &m) const {
        return evaluate(m) * m.z;
    }
    
    float BerryMicrofacetDistribution::evaluateSmithG1(const Vector3D &v, const Normal3D &m) const {
        SLRAssert_NotImplemented();
        return 0.0f;
    }
    
    
    
    float GGXMicrofacetDistribution::sample(float u0, float u1, Normal3D* m, float* normalPDF) const {
        // sample slopes
        float tiltFactor = std::sqrt(u1 / (1 - u1));
        float slope_x = m_alpha_gx * tiltFactor * std::cos(2 * M_PI * u0);
        float slope_y = m_alpha_gy * tiltFactor * std::sin(2 * M_PI * u0);

        *m = Normal3D(-slope_x, -slope_y, 1.0f).normalize();
        
        float temp = m->x * m->x / (m_alpha_gx * m_alpha_gx) + m->y * m->y / (m_alpha_gy * m_alpha_gy) + m->z * m->z;
        float ret = 1.0f / (M_PI * m_alpha_gx * m_alpha_gy * temp * temp);
        *normalPDF = ret * m->z;
        
        return ret;
    }
    
    float GGXMicrofacetDistribution::evaluate(const Normal3D &m) const {
        if (m.z <= 0)
            return 0.0f;
        float temp = m.x * m.x / (m_alpha_gx * m_alpha_gx) + m.y * m.y / (m_alpha_gy * m_alpha_gy) + m.z * m.z;
        return 1.0f / (M_PI * m_alpha_gx * m_alpha_gy * temp * temp);
    }
    
    float GGXMicrofacetDistribution::evaluatePDF(const Normal3D &m) const {
        return evaluate(m) * m.z;
    }
    
    // References
    // Importance Sampling Microfacet-Based BSDFs using the Distribution of Visible Normals
    // A Simpler and Exact Sampling Routine for the GGX Distribution of Visible Normals
    float GGXMicrofacetDistribution::sample(const Vector3D &v, float u0, float u1, Normal3D* m, float* normalPDF) const {        
        // stretch view
        Vector3D sv = normalize(Vector3D(m_alpha_gx * v.x, m_alpha_gy * v.y, v.z));
        
        // orthonormal basis
//        Vector3D T1 = (sv.z < 0.9999f) ? normalize(cross(sv, Vector3D::Ez)) : Vector3D::Ex;
//        Vector3D T2 = cross(T1, sv);
        float distIn2D = std::sqrt(sv.x * sv.x + sv.y * sv.y);
        float recDistIn2D = 1.0f / distIn2D;
        Vector3D T1 = (sv.z < 0.9999f) ? Vector3D(sv.y * recDistIn2D, -sv.x * recDistIn2D, 0) : Vector3D::Ex;
        Vector3D T2 = Vector3D(T1.y * sv.z, -T1.x * sv.z, distIn2D);
        
        // sample point with polar coordinates (r, phi)
        float a = 1.0f / (1.0f + sv.z);
        float r = std::sqrt(u0);
        float phi = M_PI * ((u1 < a) ? u1 / a : 1 + (u1 - a) / (1.0f - a));
        float P1 = r * std::cos(phi);
        float P2 = r * std::sin(phi) * ((u1 < a) ? 1.0 : sv.z);
        
        // compute normal
        *m = P1 * T1 + P2 * T2 + std::sqrt(1.0f - P1 * P1 - P2 * P2) * sv;
        
        // unstretch
        *m = normalize(Normal3D(m_alpha_gx * m->x, m_alpha_gy * m->y, m->z));
        
        float D = evaluate(*m);
        *normalPDF = evaluateSmithG1(v, *m) * absDot(v, *m) * D / std::abs(v.z);
        
        return D;
    }
    
    float GGXMicrofacetDistribution::evaluatePDF(const Vector3D &v, const Normal3D &m) const {
        return evaluateSmithG1(v, m) * absDot(v, m) * evaluate(m) / std::abs(v.z);
    }
    
    float GGXMicrofacetDistribution::evaluateSmithG1(const Vector3D &v, const Normal3D &m) const {
        float chi = (dot(v, m) / v.z) > 0 ? 1 : 0;
        float tanTheta_v_alpha_go_2 = (v.x * v.x * m_alpha_gx * m_alpha_gx + v.y * v.y * m_alpha_gy * m_alpha_gy) / (v.z * v.z);
        return chi * 2 / (1 + std::sqrt(1 + tanTheta_v_alpha_go_2));
    }
    
    
    
    float GTRMicrofacetDistribution::sample(float u0, float u1, Normal3D *m, float *normalPDF) const {
        float alpha2 = m_alpha_g * m_alpha_g;
        float alpha2Powered = std::pow(alpha2, 1 - m_gamma);
        float cosTheta_m = std::sqrt((1 - std::pow(alpha2Powered * (1 - u0) + u0, 1 / (1 - m_gamma))) / (1 - alpha2));
        float theta_m = std::acos(cosTheta_m);
        float phi_m = 2 * M_PI * u1;
        *m = Normal3D(std::sin(theta_m) * std::cos(phi_m), std::sin(theta_m) * std::sin(phi_m), std::cos(theta_m));
        
        float ret = (m_gamma - 1) * (alpha2 - 1) / (M_PI * (1 - alpha2Powered)) / std::pow(1 + (alpha2 - 1) * cosTheta_m * cosTheta_m, m_gamma);
        *normalPDF = ret * m->z;
        
        return ret;
    }
    
    float GTRMicrofacetDistribution::evaluate(const Normal3D &m) const {
        if (m.z <= 0)
            return 0.0f;
        float alpha2 = m_alpha_g * m_alpha_g;
        float alpha2Powered = std::pow(alpha2, 1 - m_gamma);
        float cosTheta_m = m.z;
        return (m_gamma - 1) * (alpha2 - 1) / (M_PI * (1 - alpha2Powered)) / std::pow(1 + (alpha2 - 1) * cosTheta_m * cosTheta_m, m_gamma);
    }
    
    float GTRMicrofacetDistribution::evaluatePDF(const Normal3D &m) const {
        return evaluate(m) * m.z;
    }
    
    float GTRMicrofacetDistribution::evaluateSmithG1(const Vector3D &v, const Normal3D &m) const {
        SLRAssert_NotImplemented();
        return 0.0f;
    }
}
