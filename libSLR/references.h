//
//  references.h
//
//  Created by 渡部 心 on 2015/04/27.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#ifndef SLR_references_h
#define SLR_references_h

#include <vector>
#include <memory>
#include <stdint.h>

namespace SLR {
    // Memory Allocators
    class Allocator;
    class DefaultAllocator;
    class StackAllocator;
    class ArenaAllocator;
    class MSpaceAllocator;
    
    // RGB & Spectrum
    template<typename RealType> struct CompensatedSum;
    template <typename RealType> struct RGBSamplesTemplate;
    template <typename RealType> struct RGBTemplate;
    template <typename RealType> struct RGBStorageTemplate;
    template <typename RealType, uint32_t N> struct ContinuousSpectrumTemplate;
    template <typename RealType, uint32_t N> struct RegularContinuousSpectrumTemplate;
    template <typename RealType, uint32_t N> struct IrregularContinuousSpectrumTemplate;
    template <typename RealType, uint32_t N> struct UpsampledContinuousSpectrumTemplate;
    template <typename RealType, uint32_t N> struct WavelengthSamplesTemplate;
    template <typename RealType, uint32_t N> struct SampledSpectrumTemplate;
    template <typename RealType, uint32_t N> struct DiscretizedSpectrumTemplate;
    template <typename RealType, uint32_t numStrata> struct SpectrumStorageTemplate;
    // FIXME: Current code is inconsistent with respect to float precision.
    typedef float SpectrumFloat;
    typedef RGBTemplate<SpectrumFloat> RGBInputSpectrum;
    const static uint32_t NumSpectralSamples = 16;
    typedef ContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> ContinuousSpectrum;
    typedef RegularContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> RegularContinuousSpectrum;
    typedef IrregularContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> IrregularContinuousSpectrum;
    typedef UpsampledContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> UpsampledContinuousSpectrum;
#ifdef Use_Spectral_Representation
    const static uint32_t NumStrataForStorage = 16;
    typedef WavelengthSamplesTemplate<SpectrumFloat, NumSpectralSamples> WavelengthSamples;
    typedef SampledSpectrumTemplate<SpectrumFloat, NumSpectralSamples> SampledSpectrum;
    typedef DiscretizedSpectrumTemplate<SpectrumFloat, NumStrataForStorage> DiscretizedSpectrum;
    typedef SpectrumStorageTemplate<SpectrumFloat, NumStrataForStorage> SpectrumStorage;
    
    typedef ContinuousSpectrum InputSpectrum;
#else
    typedef RGBSamplesTemplate<SpectrumFloat> WavelengthSamples;
    typedef RGBTemplate<SpectrumFloat> SampledSpectrum;
    typedef RGBTemplate<SpectrumFloat> DiscretizedSpectrum;
    typedef RGBStorageTemplate<SpectrumFloat> SpectrumStorage;
    
    typedef RGBInputSpectrum InputSpectrum;
#endif
    typedef CompensatedSum<SampledSpectrum> SampledSpectrumSum;
    
    // Basic Types
    template <typename RealType> struct Vector3Template;
    template <typename RealType> struct Vector4Template;
    template <typename RealType> struct Normal3Template;
    template <typename RealType> struct Point3Template;
    template <typename RealType> struct Matrix4x4Template;
    template <typename RealType> struct QuaternionTemplate;
    template <typename RealType> struct TexCoord2Template;
    typedef Vector3Template<float> Vector3D;
    typedef Vector3Template<float> Tangent3D;
    typedef Vector3Template<float> Bitangent3D;
    typedef Vector4Template<float> Vector4D;
    typedef Normal3Template<float> Normal3D;
    typedef Point3Template<float> Point3D;
    typedef Matrix4x4Template<float> Matrix4x4;
    typedef QuaternionTemplate<float> Quaternion;
    typedef TexCoord2Template<float> TexCoord2D;
    typedef CompensatedSum<float> FloatSum;
    
    // Transforms
    class Transform;
    class StaticTransform;
    class AnimatedTransform;
    class ChainedTransform;
    
    // Random Number Generators
    struct Types32bit;
    struct Types64bit;
    template <typename TypeSet> class RandomNumberGeneratorTemplate;
    template <typename TypeSet> class XORShiftTemplate;
    
    typedef RandomNumberGeneratorTemplate<Types32bit> RandomNumberGenerator;
    typedef XORShiftTemplate<Types32bit> XORShift;
    
    // Distributions
    template <typename RealType> class RegularConstantDiscrete1DTemplate;
    template <typename RealType> class RegularConstantContinuous1DTemplate;
    template <typename RealType> class RegularConstantContinuous2DTemplate;
    typedef RegularConstantDiscrete1DTemplate<float> RegularConstantDiscrete1D;
    typedef RegularConstantContinuous1DTemplate<float> RegularConstantContinuous1D;
    typedef RegularConstantContinuous2DTemplate<float> RegularConstantContinuous2D;
    
    // Image & Tiled Image
    class Image2D;
    template <uint32_t log2_tileWidth = 3> class TiledImage2DTemplate;
    
    typedef TiledImage2DTemplate<> TiledImage2D;
    
    //
    class ImageSensor;
    
    //
    struct Ray;
    struct BoundingBox3D;
    struct Vertex;
    struct ReferenceFrame;
    struct Intersection;
    struct SurfacePoint;
    
    // Surface Objects
    class SurfaceObject;
    class SingleSurfaceObject;
    class InfiniteSphereSurfaceObject;
    class SurfaceObjectAggregate;
    class TransformedSurfaceObject;
    class Scene;
    
    struct LightPosQuery;
    struct LightPosQueryResult;
    class Light;
    
    struct LensPosQuery;
    struct LensPosQueryResult;
    
    // Cameras
    class Camera;
    class PerspectiveCamera;
    class EquirectangularCamera;
    
    // Surfaces
    class Surface;
    class Triangle;
    class InfiniteSphere;
    
    // Accelerators
    class BBVH;
    
    // Textures
    class SpectrumTexture;
    class Normal3DTexture;
    class FloatTexture;
    class ConstantSpectrumTexture;
    class ConstantFloatTexture;
    class ImageSpectrumTexture;
    class ImageNormal3DTexture;
    class ImageFloatTexture;
    class CheckerBoardSpectrumTexture;
    class CheckerBoardNormal3DTexture;
    class CheckerBoardFloatTexture;
    
    // Materials
    class SpatialFresnel;
    class SpatialFresnelNoOp;
    class SpatialFresnelConductor;
    class SpatialFresnelDielectric;
    class SurfaceMaterial;
    class EmitterSurfaceProperty;
    class DiffuseReflection;
    class SpecularReflection;
    class SpecularTransmission;
    class ModifiedWardDurReflection;
    class AshikhminSpecularReflection;
    class AshikhminDiffuseReflection;
    class SummedSurfaceMaterial;
    class MixedSurfaceMaterial;
    class EmitterSurfaceMaterial;
    class DiffuseEmission;
    class IBLEmission;
    
    // Directional Distribution Functions
    struct DirectionType;
    struct EDFQuery;
    struct EDFSample;
    struct EDFQueryResult;
    struct BSDFQuery;
    struct BSDFSample;
    struct BSDFQueryResult;
    struct IDFQuery;
    struct IDFSample;
    struct IDFQueryResult;
    class EDF;
    class BSDF;
    class IDF;
    class DiffuseEDF;
    class IBLEDF;
    class MultiEDF;
    class Fresnel;
    class FresnelNoOp;
    class FresnelConductor;
    class FresnelDielectric;
    class LambertianBRDF;
    class SpecularBRDF;
    class SpecularBTDF;
    class InverseBSDF;
    class ModifiedWardDurBRDF;
    class AshikhminSpecularBRDF;
    class AshikhminDiffuseBRDF;
    class MultiBSDF;
    class PerspectiveIDF;
    class EquirectangularIDF;
    
    class RenderSettings;
    class Renderer;
    
    // Renderers
    class PathTracingRenderer;
    class AMCMCPPMRenderer;
}

#endif
