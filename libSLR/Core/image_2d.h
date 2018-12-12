﻿//
//  image_2d.h
//
//  Created by 渡部 心 on 2015/07/11.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#ifndef __SLR_image_2d__
#define __SLR_image_2d__

#include "../defines.h"
#include "../declarations.h"
#include "../MemoryAllocators/Allocator.h"
#include "../BasicTypes/spectrum_base.h"
#include "../BasicTypes/spectrum_types.h"
#include "../External/half.hpp"

namespace SLR {
    using half = half_float::half;

    enum class ColorFormat {
        RGB8x3 = 0,
        RGB_8x4,
        RGBA8x4,
        RGBA16Fx4,
        Gray8,
#ifdef SLR_Use_Spectral_Representation
        uvs16Fx3,
        uvsA16Fx4,
#endif
        Num
    };
    
    enum class ImageStoreMode {
        AsIs = 0,
        NormalTexture,
        AlphaTexture,
    };
    
    struct SLR_API RGB8x3 { uint8_t r, g, b; };
    struct SLR_API RGB_8x4 { uint8_t r, g, b, dummy; };
    struct SLR_API RGBA8x4  { uint8_t r, g, b, a; };
    struct SLR_API RGBA16Fx4 { half r, g, b, a; };
#ifdef SLR_Use_Spectral_Representation
    struct SLR_API uvs16Fx3 { half u, v, s; };
    struct SLR_API uvsA16Fx4 { half u, v, s, a; };
#endif
    struct SLR_API Gray8 { uint8_t v; };
    
    extern SLR_API const size_t sizesOfColorFormats[(uint32_t)ColorFormat::Num];
    
    
    
    class SLR_API Image2D {
    protected:
        uint32_t m_width, m_height;
        ColorFormat m_colorFormat;
        SpectrumType m_spType;
        
        virtual const void* getInternal(uint32_t x, uint32_t y) const = 0;
        virtual void setInternal(uint32_t x, uint32_t y, const void* data, size_t size) = 0;
    public:
        Image2D() { }
        Image2D(uint32_t w, uint32_t h, ColorFormat fmt, SpectrumType spType) : m_width(w), m_height(h), m_colorFormat(fmt), m_spType(spType) { }
        virtual ~Image2D() { }
        
        template <typename ColFmt>
        const ColFmt &get(uint32_t x, uint32_t y) const { return *(const ColFmt*)getInternal(x, y); }
        template <typename ColFmt>
        void set(uint32_t x, uint32_t y, const ColFmt &data) { setInternal(x, y, &data, sizeof(data)); }
        
        void areaAverage(float xLeft, float xRight, float yTop, float yBottom, void* avg) const;
        
        uint32_t width() const { return m_width; }
        uint32_t height() const { return m_height; }
        SpectrumType spectrumType() const { return m_spType; }
        ColorFormat format() const { return m_colorFormat; }
        
        void saveImage(const std::string &filepath, bool gammaCorrection) const;
    };
    
    
    
    template <uint32_t log2_tileWidth>
    class SLR_API TiledImage2DTemplate : public Image2D {
        static const size_t tileWidth = 1 << log2_tileWidth;
        static const uint32_t localMask = (1 << log2_tileWidth) - 1;
        size_t m_stride;
        size_t m_numTileX;
        size_t m_allocSize;
        uint8_t* m_data;
        
        const void* getInternal(uint32_t x, uint32_t y) const override {
            uint32_t tx = x >> log2_tileWidth;
            uint32_t ty = y >> log2_tileWidth;
            uint32_t lx = x & localMask;
            uint32_t ly = y & localMask;
            return m_data + m_stride * ((ty * m_numTileX + tx) * tileWidth * tileWidth + ly * tileWidth + lx);
        }
        
        void setInternal(uint32_t x, uint32_t y, const void* data, size_t size) override {
            uint32_t tx = x >> log2_tileWidth;
            uint32_t ty = y >> log2_tileWidth;
            uint32_t lx = x & localMask;
            uint32_t ly = y & localMask;
            std::memcpy(m_data + m_stride * ((ty * m_numTileX + tx) * tileWidth * tileWidth + ly * tileWidth + lx), data, size);
        }
    public:
        ~TiledImage2DTemplate() {
            
        }
        
        TiledImage2DTemplate(uint32_t width, uint32_t height, ColorFormat fmt, Allocator* mem) {
            m_width = width;
            m_height = height;
            m_spType = SpectrumType::Reflectance;
            m_colorFormat = fmt;
            m_stride = sizesOfColorFormats[(uint32_t)m_colorFormat];
            m_numTileX = (m_width + (tileWidth - 1)) >> log2_tileWidth;
            size_t numTileY = (m_height + (tileWidth - 1)) >> log2_tileWidth;
            size_t tileSize = m_stride * tileWidth * tileWidth;
            m_allocSize = m_numTileX * numTileY * tileSize;
            m_data = (uint8_t*)mem->alloc(m_allocSize, SLR_L1_Cacheline_Size);
            
            memset(m_data, 0, m_allocSize);
        }
        
        TiledImage2DTemplate(const void* linearData, uint32_t width, uint32_t height, ColorFormat fmt, Allocator* mem, ImageStoreMode mode, SpectrumType spType) {
            m_width = width;
            m_height = height;
            m_spType = spType;
            
            std::function<void(int32_t, int32_t)> convertFunc;
            switch (fmt) {
                case ColorFormat::RGB8x3:
                    switch (mode) {
                        case ImageStoreMode::AsIs:
#ifdef SLR_Use_Spectral_Representation
                            m_colorFormat = ColorFormat::uvs16Fx3;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGB8x3 &val = *((RGB8x3*)linearData + m_width * y + x);
                                float RGB[3] = {val.r / 255.0f, val.g / 255.0f, val.b / 255.0f};
                                float uvs[3];
                                UpsampledContinuousSpectrum::sRGB_to_uvs(spType, RGB, uvs);
                                uvs16Fx3 storedVal{(half)uvs[0], (half)uvs[1], (half)(uvs[2] * UPSAMPLED_CONTINOUS_SPECTRUM_SCALE_FACTOR)};
                                SLRAssert(std::isfinite((float)storedVal.u) &&
                                          std::isfinite((float)storedVal.v) &&
                                          std::isfinite((float)storedVal.s), "Invalid value.");
                                setInternal(x, y, &storedVal, m_stride);
                            };
#else
                            m_colorFormat = ColorFormat::RGB8x3;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGB8x3 &val = *((RGB8x3*)linearData + m_width * y + x);
                                RGB8x3 storedVal{val.r, val.g, val.b};
                                setInternal(x, y, &storedVal, m_stride);
                            };
#endif
                            break;
                        case ImageStoreMode::NormalTexture:
                            m_colorFormat = ColorFormat::RGB8x3;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGB8x3 &val = *((RGB8x3*)linearData + m_width * y + x);
                                RGB8x3 storedVal{val.r, val.g, val.b};
                                setInternal(x, y, &storedVal, m_stride);
                            };
                            break;
                        case ImageStoreMode::AlphaTexture:
                            m_colorFormat = ColorFormat::Gray8;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGB8x3 &val = *((RGB8x3*)linearData + m_width * y + x);
                                Gray8 storedVal{val.r};
                                setInternal(x, y, &storedVal, m_stride);
                            };
                            break;
                        default:
                            break;
                    }
                    break;
                case ColorFormat::RGB_8x4:
                    switch (mode) {
                        case ImageStoreMode::AsIs:
#ifdef SLR_Use_Spectral_Representation
                            m_colorFormat = ColorFormat::uvs16Fx3;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGB_8x4 &val = *((RGB_8x4*)linearData + m_width * y + x);
                                float RGB[3] = {val.r / 255.0f, val.g / 255.0f, val.b / 255.0f};
                                float uvs[3];
                                UpsampledContinuousSpectrum::sRGB_to_uvs(spType, RGB, uvs);
                                uvs16Fx3 storedVal{(half)uvs[0], (half)uvs[1], (half)(uvs[2] * UPSAMPLED_CONTINOUS_SPECTRUM_SCALE_FACTOR)};
                                SLRAssert(isfinite(storedVal.u) &&
                                          isfinite(storedVal.v) &&
                                          isfinite(storedVal.s), "Invalid value.");
                                setInternal(x, y, &storedVal, m_stride);
                            };
#else
                            m_colorFormat = ColorFormat::RGB8x3;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGB_8x4 &val = *((RGB_8x4*)linearData + m_width * y + x);
                                RGB8x3 storedVal{val.r, val.g, val.b};
                                setInternal(x, y, &storedVal, m_stride);
                            };
#endif
                            break;
                        case ImageStoreMode::NormalTexture:
                            m_colorFormat = ColorFormat::RGB8x3;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGB_8x4 &val = *((RGB_8x4*)linearData + m_width * y + x);
                                RGB8x3 storedVal{val.r, val.g, val.b};
                                setInternal(x, y, &storedVal, m_stride);
                            };
                            break;
                        case ImageStoreMode::AlphaTexture:
                            m_colorFormat = ColorFormat::Gray8;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGB_8x4 &val = *((RGB_8x4*)linearData + m_width * y + x);
                                Gray8 storedVal{val.r};
                                setInternal(x, y, &storedVal, m_stride);
                            };
                            break;
                        default:
                            break;
                    }
                    break;
                case ColorFormat::RGBA8x4:
                    switch (mode) {
                        case ImageStoreMode::AsIs:
#ifdef SLR_Use_Spectral_Representation
                            m_colorFormat = ColorFormat::uvsA16Fx4;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGBA8x4 &val = *((RGBA8x4*)linearData + m_width * y + x);
                                float RGB[3] = {val.r / 255.0f, val.g / 255.0f, val.b / 255.0f};
                                float uvs[3];
                                UpsampledContinuousSpectrum::sRGB_to_uvs(spType, RGB, uvs);
                                uvsA16Fx4 storedVal{(half)uvs[0], (half)uvs[1], (half)(uvs[2] * UPSAMPLED_CONTINOUS_SPECTRUM_SCALE_FACTOR), (half)(val.a / 255.0f)};
                                SLRAssert(isfinite(storedVal.u) && isfinite(storedVal.v) && isfinite(storedVal.s) &&
                                          isfinite(storedVal.a) && (float)storedVal.a >= 0,
                                          "Invalid value: %g, %g, %g, %g", (float)storedVal.u, (float)storedVal.v, (float)storedVal.s, (float)storedVal.a);
                                setInternal(x, y, &storedVal, m_stride);
                            };
#else
                            m_colorFormat = ColorFormat::RGBA8x4;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGBA8x4 &val = *((RGBA8x4*)linearData + m_width * y + x);
                                RGBA8x4 storedVal{val.r, val.g, val.b, val.a};
                                setInternal(x, y, &storedVal, m_stride);
                            };
#endif
                            break;
                        case ImageStoreMode::NormalTexture:
                            m_colorFormat = ColorFormat::RGB8x3;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGBA8x4 &val = *((RGBA8x4*)linearData + m_width * y + x);
                                RGB8x3 storedVal{val.r, val.g, val.b};
                                setInternal(x, y, &storedVal, m_stride);
                            };
                            break;
                        case ImageStoreMode::AlphaTexture:
                            m_colorFormat = ColorFormat::Gray8;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGBA8x4 &val = *((RGBA8x4*)linearData + m_width * y + x);
                                Gray8 storedVal{val.a};
                                setInternal(x, y, &storedVal, m_stride);
                            };
                            break;
                        default:
                            break;
                    }
                    break;
                case ColorFormat::RGBA16Fx4:
                    switch (mode) {
                        case ImageStoreMode::AsIs:
#ifdef SLR_Use_Spectral_Representation
                            m_colorFormat = ColorFormat::uvsA16Fx4;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGBA16Fx4 &val = *((RGBA16Fx4*)linearData + m_width * y + x);
                                float RGB[3] = {val.r, val.g, val.b};
                                float uvs[3];
                                RGB[0] = std::max(RGB[0], 0.0f);
                                RGB[1] = std::max(RGB[1], 0.0f);
                                RGB[2] = std::max(RGB[2], 0.0f);
                                SLRAssert(val.a > 0.0f, "Invalid alpha value.");
                                UpsampledContinuousSpectrum::sRGB_to_uvs(spType, RGB, uvs);
                                uvsA16Fx4 storedVal{(half)uvs[0], (half)uvs[1], (half)(uvs[2] * UPSAMPLED_CONTINOUS_SPECTRUM_SCALE_FACTOR), (half)val.a};
                                SLRAssert(isfinite(storedVal.u) && isfinite(storedVal.v) && isfinite(storedVal.s) &&
                                          isfinite(storedVal.a) && (float)storedVal.a >= 0,
                                          "Invalid value: %g, %g, %g, %g", (float)storedVal.u, (float)storedVal.v, (float)storedVal.s, (float)storedVal.a);
                                setInternal(x, y, &storedVal, m_stride);
                            };
#else
                            m_colorFormat = ColorFormat::RGBA16Fx4;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGBA16Fx4 &val = *((RGBA16Fx4*)linearData + m_width * y + x);
                                RGBA16Fx4 storedVal{val.r, val.g, val.b, val.a};
                                setInternal(x, y, &storedVal, m_stride);
                            };
#endif
                            break;
                        case ImageStoreMode::NormalTexture:
                            SLRAssert(false, "Source image format is not compatible to the specified format.");
                            break;
                        case ImageStoreMode::AlphaTexture:
                            m_colorFormat = ColorFormat::Gray8;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const RGBA16Fx4 &val = *((RGBA16Fx4*)linearData + m_width * y + x);
                                SLRAssert(val.a > 0.0f, "Invalid alpha value.");
                                Gray8 storedVal{(uint8_t)std::min(uint32_t(255 * val.a), uint32_t(255))};
                                setInternal(x, y, &storedVal, m_stride);
                            };
                            break;
                        default:
                            break;
                    }
                    break;
                case ColorFormat::Gray8:
                    switch (mode) {
                        case ImageStoreMode::AsIs:
                            m_colorFormat = ColorFormat::Gray8;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const Gray8 &val = *((Gray8*)linearData + m_width * y + x);
                                setInternal(x, y, &val, m_stride);
                            };
                            break;
                        case ImageStoreMode::NormalTexture:
                            SLRAssert(false, "Source image format is not compatible to the specified format.");
                            break;
                        case ImageStoreMode::AlphaTexture:
                            m_colorFormat = ColorFormat::Gray8;
                            convertFunc = [this, &linearData, &spType](int32_t x, int32_t y) {
                                const Gray8 &val = *((Gray8*)linearData + m_width * y + x);
                                setInternal(x, y, &val, m_stride);
                            };
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    SLRAssert(false, "Color format is invalid.");
                    break;
            }
            m_stride = sizesOfColorFormats[(uint32_t)m_colorFormat];
            m_numTileX = (m_width + (tileWidth - 1)) >> log2_tileWidth;
            size_t numTileY = (m_height + (tileWidth - 1)) >> log2_tileWidth;
            size_t tileSize = m_stride * tileWidth * tileWidth;
            m_allocSize = m_numTileX * numTileY * tileSize;
            m_data = (uint8_t*)mem->alloc(m_allocSize, SLR_L1_Cacheline_Size);
            
            for (int i = 0; i < m_height; ++i) {
                for (int j = 0; j < m_width; ++j) {
                    convertFunc(j, i);
                }
            }
        }
        
        const uint8_t* data() const { return m_data; }
        size_t size() const { return m_allocSize; }
    };
}

#endif /* __SLR_image_2d__ */
