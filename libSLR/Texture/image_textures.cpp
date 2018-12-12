﻿//
//  image_texture.cpp
//
//  Created by 渡部 心 on 2015/09/06.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#include "image_textures.h"

#include "../Core/distributions.h"
#include "../Core/image_2d.h"

namespace SLR {
    SampledSpectrum ImageSpectrumTexture::evaluate(const Point3D &p, const WavelengthSamples &wls) const {
        float u = std::fmod(p.x, 1.0f);
        float v = std::fmod(p.y, 1.0f);
        u += u < 0 ? 1.0f : 0.0f;
        v += v < 0 ? 1.0f : 0.0f;
        uint32_t px = std::min((uint32_t)(m_data->width() * u), m_data->width() - 1);
        uint32_t py = std::min((uint32_t)(m_data->height() * v), m_data->height() - 1);
        SampledSpectrum ret;
        switch (m_data->format()) {
#ifdef SLR_Use_Spectral_Representation
            case ColorFormat::uvs16Fx3: {
                const uvs16Fx3 &data = m_data->get<uvs16Fx3>(px, py);
                ret = UpsampledContinuousSpectrum(data.u, data.v, data.s / UPSAMPLED_CONTINOUS_SPECTRUM_SCALE_FACTOR).evaluate(wls);
                break;
            }
            case ColorFormat::uvsA16Fx4: {
                const uvsA16Fx4 &data = m_data->get<uvsA16Fx4>(px, py);
                ret = UpsampledContinuousSpectrum(data.u, data.v, data.s / UPSAMPLED_CONTINOUS_SPECTRUM_SCALE_FACTOR).evaluate(wls);
                break;
            }
            case ColorFormat::Gray8: {
                const Gray8 &data = m_data->get<Gray8>(px, py);
                ret = SampledSpectrum(data.v / 255.0f);
                break;
            }
#else
            case ColorFormat::RGB8x3: {
                const RGB8x3 &data = m_data->get<RGB8x3>(px, py);
                ret.r = data.r / 255.0f;
                ret.g = data.g / 255.0f;
                ret.b = data.b / 255.0f;
                break;
            }
            case ColorFormat::RGB_8x4: {
                const RGB_8x4 &data = m_data->get<RGB_8x4>(px, py);
                ret.r = data.r / 255.0f;
                ret.g = data.g / 255.0f;
                ret.b = data.b / 255.0f;
                break;
            }
            case ColorFormat::RGBA8x4: {
                const RGBA8x4 &data = m_data->get<RGBA8x4>(px, py);
                ret.r = data.r / 255.0f;
                ret.g = data.g / 255.0f;
                ret.b = data.b / 255.0f;
                break;
            }
            case ColorFormat::RGBA16Fx4: {
                const RGBA16Fx4 &data = m_data->get<RGBA16Fx4>(px, py);
                ret.r = data.r;
                ret.g = data.g;
                ret.b = data.b;
                break;
            }
            case ColorFormat::Gray8: {
                const Gray8 &data = m_data->get<Gray8>(px, py);
                ret.r = ret.g = ret.b = data.v / 255.0f;
                break;
            }
#endif
            default:
                SLRAssert(false, "Image data format is unknown.");
                break;
        }
        return ret;
    }
    
    float ImageSpectrumTexture::evaluateLuminance(const Point3D &p) const {
        float u = std::fmod(p.x, 1.0f);
        float v = std::fmod(p.y, 1.0f);
        u += u < 0 ? 1.0f : 0.0f;
        v += v < 0 ? 1.0f : 0.0f;
        uint32_t px = std::min((uint32_t)(m_data->width() * u), m_data->width() - 1);
        uint32_t py = std::min((uint32_t)(m_data->height() * v), m_data->height() - 1);
        
        float ret = 0.0f;
        switch (m_data->format()) {
#ifdef SLR_Use_Spectral_Representation
            case ColorFormat::uvs16Fx3: {
                const uvs16Fx3 &data = m_data->get<uvs16Fx3>(px, py);
                float uvs[] = {data.u, data.v, data.s / (float)UPSAMPLED_CONTINOUS_SPECTRUM_SCALE_FACTOR};
                ret = UpsampledContinuousSpectrum::uvs_to_luminance(uvs);
            }
            case ColorFormat::uvsA16Fx4: {
                const uvsA16Fx4 &data = m_data->get<uvsA16Fx4>(px, py);
                float uvs[] = {data.u, data.v, data.s / (float)UPSAMPLED_CONTINOUS_SPECTRUM_SCALE_FACTOR};
                ret = UpsampledContinuousSpectrum::uvs_to_luminance(uvs);
                break;
            }
            case ColorFormat::Gray8: {
                const Gray8 &data = m_data->get<Gray8>(px, py);
                ret = data.v / 255.0f;
                break;
            }
#else
            case ColorFormat::RGB8x3: {
                const RGB8x3 &data = m_data->get<RGB8x3>(px, py);
                return sRGB_to_Luminance(data.r / 255.0f, data.g / 255.0f, data.b / 255.0f);
                break;
            }
            case ColorFormat::RGB_8x4: {
                const RGB_8x4 &data = m_data->get<RGB_8x4>(px, py);
                return sRGB_to_Luminance(data.r / 255.0f, data.g / 255.0f, data.b / 255.0f);
                break;
            }
            case ColorFormat::RGBA8x4: {
                const RGBA8x4 &data = m_data->get<RGBA8x4>(px, py);
                return sRGB_to_Luminance(data.r / 255.0f, data.g / 255.0f, data.b / 255.0f);
                break;
            }
            case ColorFormat::RGBA16Fx4: {
                const RGBA16Fx4 &data = m_data->get<RGBA16Fx4>(px, py);
                return sRGB_to_Luminance(data.r, data.g, data.b);
                break;
            }
            case ColorFormat::Gray8: {
                const Gray8 &data = m_data->get<Gray8>(px, py);
                ret = data.v / 255.0f;
                break;
            }
#endif
            default:
                SLRAssert(false, "Image data format is unknown.");
                break;
        }
        return ret;
    }
    
    const ContinuousDistribution2D* ImageSpectrumTexture::createIBLImportanceMap() const {
        uint32_t mapWidth = m_data->width() / 4;
        uint32_t mapHeight = m_data->height() / 4;
        float deltaX = m_data->width() / mapWidth;
        float deltaY = m_data->height() / mapHeight;
        std::function<float(uint32_t, uint32_t)> pickFunc = [this, &deltaX, &deltaY, &mapHeight](uint32_t x, uint32_t y) -> float {
            uint8_t data[16];
            m_data->areaAverage(x * deltaX, (x + 1) * deltaX, y * deltaY, (y + 1) * deltaY, data);
            float luminance;
            switch (m_data->format()) {
                case ColorFormat::RGB8x3: {
                    RGB8x3 avg = *(RGB8x3*)data;
                    luminance = sRGB_to_Luminance(avg.r, avg.g, avg.b);
                    break;
                }
                case ColorFormat::RGB_8x4: {
                    RGB_8x4 avg = *(RGB_8x4*)data;
                    luminance = sRGB_to_Luminance(avg.r, avg.g, avg.b);
                    break;
                }
                case ColorFormat::RGBA8x4: {
                    RGBA8x4 avg = *(RGBA8x4*)data;
                    luminance = sRGB_to_Luminance(avg.r, avg.g, avg.b);
                    break;
                }
                case ColorFormat::RGBA16Fx4: {
                    RGBA16Fx4 avg = *(RGBA16Fx4*)data;
                    luminance = sRGB_to_Luminance(avg.r, avg.g, avg.b);
                    break;
                }
#ifdef SLR_Use_Spectral_Representation
                case ColorFormat::uvs16Fx3: {
                    uvs16Fx3 avg = *(uvs16Fx3*)data;
                    float uvs[3] = {avg.u, avg.v, avg.s};
                    float rgb[3];
                    UpsampledContinuousSpectrum::uvs_to_sRGB(m_data->spectrumType(), uvs, rgb);
                    luminance = sRGB_to_Luminance(rgb[0], rgb[1], rgb[2]);
                    break;
                }
                case ColorFormat::uvsA16Fx4: {
                    uvsA16Fx4 avg = *(uvsA16Fx4*)data;
                    float uvs[3] = {avg.u, avg.v, avg.s};
                    float rgb[3];
                    UpsampledContinuousSpectrum::uvs_to_sRGB(m_data->spectrumType(), uvs, rgb);
                    luminance = sRGB_to_Luminance(rgb[0], rgb[1], rgb[2]);
                    break;
                }
#endif
                default:
                    return 0.0f;
            }
            SLRAssert(std::isfinite(luminance), "Invalid area average value.");
            return std::sin(M_PI * (y + 0.5f) / mapHeight) * luminance;
        };
        return new RegularConstantContinuousDistribution2D(mapWidth, mapHeight, pickFunc);
    }
    
    
    
    Normal3D ImageNormalTexture::evaluate(const Point3D &p) const {
        float u = std::fmod(p.x, 1.0f);
        float v = std::fmod(p.y, 1.0f);
        u += u < 0 ? 1.0f : 0.0f;
        v += v < 0 ? 1.0f : 0.0f;
        uint32_t px = std::min((uint32_t)(m_data->width() * u), m_data->width() - 1);
        uint32_t py = std::min((uint32_t)(m_data->height() * v), m_data->height() - 1);
        Normal3D ret;
        switch (m_data->format()) {
            case ColorFormat::RGB8x3: {
                const RGB8x3 &data = m_data->get<RGB8x3>(px, py);
                ret = normalize(Normal3D(data.r / 255.0f - 0.5f, data.g / 255.0f - 0.5f, data.b / 255.0f - 0.5f));
                break;
            }
            case ColorFormat::RGB_8x4: {
                const RGB_8x4 &data = m_data->get<RGB_8x4>(px, py);
                ret = normalize(Normal3D(data.r / 255.0f - 0.5f, data.g / 255.0f - 0.5f, data.b / 255.0f - 0.5f));
                break;
            }
            case ColorFormat::RGBA8x4: {
                const RGBA8x4 &data = m_data->get<RGBA8x4>(px, py);
                ret = normalize(Normal3D(data.r / 255.0f - 0.5f, data.g / 255.0f - 0.5f, data.b / 255.0f - 0.5f));
                break;
            }
            case ColorFormat::RGBA16Fx4: {
                break;
            }
            case ColorFormat::Gray8: {
                break;
            }
            default:
                break;
        }
        return ret;
    }
    
    
    
    float ImageFloatTexture::evaluate(const Point3D &p) const {
        float u = std::fmod(p.x, 1.0f);
        float v = std::fmod(p.y, 1.0f);
        u += u < 0 ? 1.0f : 0.0f;
        v += v < 0 ? 1.0f : 0.0f;
        uint32_t px = std::min((uint32_t)(m_data->width() * u), m_data->width() - 1);
        uint32_t py = std::min((uint32_t)(m_data->height() * v), m_data->height() - 1);
        float ret = 0.0f;
        switch (m_data->format()) {
            case ColorFormat::RGB8x3: {
                break;
            }
            case ColorFormat::RGB_8x4: {
                break;
            }
            case ColorFormat::RGBA8x4: {
                break;
            }
            case ColorFormat::RGBA16Fx4: {
                break;
            }
            case ColorFormat::Gray8: {
                const Gray8 &data = m_data->get<Gray8>(px, py);
                ret = data.v / 255.0f;
                break;
            }
#ifdef SLR_Use_Spectral_Representation
            case ColorFormat::uvsA16Fx4: {
                const uvsA16Fx4 &data = m_data->get<uvsA16Fx4>(px, py);
                ret = data.a;
                break;
            }
#endif
            default:
                break;
        }
        return ret;
    }    
}
