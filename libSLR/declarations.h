﻿//
//  declarations.h
//
//  Created by 渡部 心 on 2015/04/27.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#ifndef __SLR_declarations__
#define __SLR_declarations__

#include <vector>
#include <memory>
#include <stdint.h>

namespace SLR {
    // ----------------------------------------------------------------
    // Memory Allocators
    
    class Allocator;
    class DefaultAllocator;
    class StackAllocator;
    class ArenaAllocator;
    class MSpaceAllocator;
    
    // END: Memory Allocators
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // Basic Types
    
    // RGB & Spectrum
    template <typename RealType> struct CompensatedSum;
    template <typename RealType> struct RGBSamplesTemplate;
    template <typename RealType> struct RGBTemplate;
    template <typename RealType> class RGBStorageTemplate;
    template <typename RealType, uint32_t NumSpectralSamples> class ContinuousSpectrumTemplate;
    template <typename RealType, uint32_t NumSpectralSamples> class RegularContinuousSpectrumTemplate;
    template <typename RealType, uint32_t NumSpectralSamples> class IrregularContinuousSpectrumTemplate;
    template <typename RealType, uint32_t NumSpectralSamples> class UpsampledContinuousSpectrumTemplate;
    template <typename RealType, uint32_t NumSpectralSamples> class ScaledAndOffsetUpsampledContinuousSpectrumTemplate;
    template <typename RealType, uint32_t NumSpectralSamples> struct WavelengthSamplesTemplate;
    template <typename RealType, uint32_t NumSpectralSamples> struct SampledSpectrumTemplate;
    template <typename RealType, uint32_t NumStrataForStorage> struct DiscretizedSpectrumTemplate;
    template <typename RealType, uint32_t NumStrataForStorage> class SpectrumStorageTemplate;
    // FIXME: Current code is inconsistent with respect to float precision.
    typedef float SpectrumFloat;
#ifdef SLR_Use_Spectral_Representation
    const static uint32_t NumSpectralSamples = 16;
    const static uint32_t NumStrataForStorage = 16;
    
    typedef ContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> ContinuousSpectrum;
    typedef RegularContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> RegularContinuousSpectrum;
    typedef IrregularContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> IrregularContinuousSpectrum;
    typedef UpsampledContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> UpsampledContinuousSpectrum;
    typedef ScaledAndOffsetUpsampledContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> ScaledAndOffsetUpsampledContinuousSpectrum;
    
    typedef WavelengthSamplesTemplate<SpectrumFloat, NumSpectralSamples> WavelengthSamples;
    typedef SampledSpectrumTemplate<SpectrumFloat, NumSpectralSamples> SampledSpectrum;
    typedef DiscretizedSpectrumTemplate<SpectrumFloat, NumStrataForStorage> DiscretizedSpectrum;
    typedef SpectrumStorageTemplate<SpectrumFloat, NumStrataForStorage> SpectrumStorage;
    
    typedef ContinuousSpectrum AssetSpectrum;
#else
    const static uint32_t NumSpectralSamples = 3;
    const static uint32_t NumStrataForStorage = 3;
    
    typedef ContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> ContinuousSpectrum;
    typedef RegularContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> RegularContinuousSpectrum;
    typedef IrregularContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> IrregularContinuousSpectrum;
    typedef UpsampledContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> UpsampledContinuousSpectrum;
    typedef ScaledAndOffsetUpsampledContinuousSpectrumTemplate<SpectrumFloat, NumSpectralSamples> ScaledAndOffsetUpsampledContinuousSpectrum;
    
    typedef RGBTemplate<SpectrumFloat> RGBSpectrum;
    
    typedef RGBSamplesTemplate<SpectrumFloat> WavelengthSamples;
    typedef RGBTemplate<SpectrumFloat> SampledSpectrum;
    typedef RGBTemplate<SpectrumFloat> DiscretizedSpectrum;
    typedef RGBStorageTemplate<SpectrumFloat> SpectrumStorage;
    
    typedef RGBSpectrum AssetSpectrum;
#endif
    typedef CompensatedSum<SampledSpectrum> SampledSpectrumSum;
    
    template <typename RealType> struct Point3DTemplate;
    template <typename RealType> struct Vector3DTemplate;
    template <typename RealType> struct Vector4DTemplate;
    template <typename RealType> struct Normal3DTemplate;
    template <typename RealType> struct Matrix3x3Template;
    template <typename RealType> struct Matrix4x4Template;
    template <typename RealType> struct QuaternionTemplate;
    template <typename RealType> struct TexCoord2DTemplate;
    template <typename RealType> struct RayTemplate;
    template <typename RealType> struct RaySegmentTemplate;
    template <typename RealType> struct BoundingBox3DTemplate;
    typedef Point3DTemplate<float> Point3D;
    typedef Vector3DTemplate<float> Vector3D;
    typedef Vector3DTemplate<float> Tangent3D;
    typedef Vector3DTemplate<float> Bitangent3D;
    typedef Vector4DTemplate<float> Vector4D;
    typedef Normal3DTemplate<float> Normal3D;
    typedef Matrix3x3Template<float> Matrix3x3;
    typedef Matrix4x4Template<float> Matrix4x4;
    typedef QuaternionTemplate<float> Quaternion;
    typedef TexCoord2DTemplate<float> TexCoord2D;
    typedef RayTemplate<float> Ray;
    typedef RaySegmentTemplate<float> RaySegment;
    typedef BoundingBox3DTemplate<float> BoundingBox3D;
    typedef CompensatedSum<float> FloatSum;
    
    // END: Basic Types
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // Core
    
    // RNG
    struct Types32bit;
    struct Types64bit;
    template <typename TypeSet> class RandomNumberGeneratorTemplate;
    template <typename TypeSet> class XORShiftRNGTemplate;
    template <typename TypeSet> class LinearCongruentialRNGTemplate;
    typedef RandomNumberGeneratorTemplate<Types32bit> RandomNumberGenerator;
    
    // Distribution
    template <typename RealType> class DiscreteDistribution1DTemplate;
    template <typename RealType> class ContinuousDistribution1DTemplate;
    template <typename RealType> class RegularConstantContinuousDistribution1DTemplate;
    template <typename RealType> class ContinuousDistribution2DTemplate;
    template <typename RealType> class RegularConstantContinuousDistribution2DTemplate;
    template <typename RealType> class MultiContinuousDistribution2DTemplate;
    typedef DiscreteDistribution1DTemplate<float> DiscreteDistribution1D;
    typedef ContinuousDistribution1DTemplate<float> ContinuousDistribution1D;
    typedef RegularConstantContinuousDistribution1DTemplate<float> RegularConstantContinuousDistribution1D;
    typedef ContinuousDistribution2DTemplate<float> ContinuousDistribution2D;
    typedef RegularConstantContinuousDistribution2DTemplate<float> RegularConstantContinuousDistribution2D;
    typedef MultiContinuousDistribution2DTemplate<float> MultiContinuousDistribution2D;
    template <typename RealType> class ImprovedPerlinNoise3DGeneratorTemplate;
    template <typename RealType> class MultiOctavePerlinNoise3DGeneratorTemplate;
    template <typename RealType> class CurlNoise3DGeneratorTemplate;
    template <typename RealType> class WorleyNoise3DGeneratorTemplate;
    template <typename RealType> class MultiOctaveWorleyNoise3DGeneratorTemplate;
    
    // Transform
    class Transform;
    class StaticTransform;
    class AnimatedTransform;
    class ChainedTransform;
    
    // geometry
    class Interaction;
    class SurfaceInteraction;
    class MediumInteraction;
    struct ReferenceFrame;
    class InteractionPoint;
    class SurfacePoint;
    class MediumPoint;
    struct Vertex;
    class SurfaceShape;
    class MediumDistribution;
    
    // Camera
    struct LensPosQuery;
    struct LensPosSample;
    struct LensPosQueryResult;
    class Camera;
    
    // directional distribution function
    struct DirectionType;
    struct EDFQuery;
    struct EDFSample;
    struct EDFQueryResult;
    struct ABDFQuery;
    struct ABDFSample;
    struct ABDFReverseInfo;
    struct ABDFQueryResult;
    struct BSDFQuery;
    struct BSDFSample;
    struct BSDFReverseInfo;
    struct BSDFQueryResult;
    struct PFQuery;
    struct PFSample;
    struct PFQueryResult;
    struct VolumetricBSDFQuery;
    struct VolumetricBSDFSample;
    struct VolumetricBSDFQueryResult;
    struct IDFQuery;
    struct IDFSample;
    struct IDFQueryResult;
    class EDF;
    class AbstractBDF;
    class BSDF;
    class PhaseFunction;
    class VolumetricBSDF;
    class IDF;
    class Fresnel;
    class FresnelNoOp;
    class FresnelConductor;
    class FresnelDielectric;
    class MicrofacetDistribution;
    class BerryMicrofacetDistribution;
    class GGXMicrofacetDistribution;
    class GTRMicrofacetDistribution;
    
    // Object
    struct LightPosQuery;
    struct LightPosQueryResult;
    class Light;
    class Object;
    
    // Surface Object
    struct SurfaceLightPosSample;
    struct SurfaceLightPosQueryResult;
    class SurfaceLight;
    class SurfaceObject;
    class SingleSurfaceObject;
    class BumpSingleSurfaceObject;
    class InfiniteSphereSurfaceObject;
    class TransformedSurfaceObject;
    class SurfaceObjectAggregate;
    
    // Medium Object
    struct VolumetricLightPosSample;
    struct VolumetricLightPosQueryResult;
    class VolumetricLight;
    class MediumObject;
    class SingleMediumObject;
    class TransformedMediumObject;
    class EnclosedMediumObject;
    class MediumObjectAggregate;
    
    // Light Path Sampler
    struct PixelPosition;
    class LightPathSampler;
    class AlphaTestSampler;
    class FreePathSampler;
    class IndependentLightPathSampler;
    
    // Accelerator
    class Accelerator;
    
    // Texture & Mapping
    class Texture2DMapping;
    class Texture3DMapping;
    class OffsetAndScale2DMapping;
    class OffsetAndScale3DMapping;
    class WorldPosition3DMapping;
    class SpectrumTexture;
    class NormalTexture;
    class FloatTexture;
    
    // Surface Material
    class SurfaceMaterial;
    class EmitterSurfaceProperty;
    class EmitterSurfaceMaterial;
    class SVFresnel;
    class SVFresnelNoOp;
    class SVFresnelConductor;
    class SVFresnelDielectric;
    class SVMicrofacetDistribution;
    class BerrySVMicrofacetDistribution;
    class GGXSVMicrofacetDistribution;
    class GTRSVMicrofacetDistribution;
    
    // Medium Material
    class MediumMaterial;
    class EmitterMediumProperty;
    class EmitterMediumMaterial;
    class NullMediumMaterial;
    
    // Image & Tiled Image
    class Image2D;
    template <uint32_t log2_tileWidth = 3> class TiledImage2DTemplate;
    typedef TiledImage2DTemplate<> TiledImage2D;
    
    // Image Sensor
    class ImageSensor;
    
    // Renderer
    class Renderer;
    class RenderSettings;
    class ProgressReporter;
    
    // END: Core
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // RNG
    
    typedef XORShiftRNGTemplate<Types32bit> XORShiftRNG;
    typedef LinearCongruentialRNGTemplate<Types32bit> LinearCongruentialRNG;
    
    // END: RNG
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // Surface Shape
    
    class TriangleSurfaceShape;
    class InfinitesimalPointSurfaceShape;
    class InfiniteSphereSurfaceShape;
    
    // END: Surface Shape
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // Medium Distribution
    
    class HomogeneousMediumDistribution;
    class DensityGridMediumDistribution;
    class AchromaticExtinctionGridMediumDistribution;
    class GridMediumDistribution;
    class VacuumMediumDistribution;
    class CloudMediumDistribution;
    
    // END: Medium Distribution
    // ----------------------------------------------------------------

    
    
    // ----------------------------------------------------------------
    // BSDF
    
    class LambertianBRDF;
    class SpecularBRDF;
    class SpecularBSDF;
    class FlippedBSDF;
    class NullBSDF;
    class OrenNayerBRDF;
    class ModifiedWardDurBRDF;
    class AshikhminShirleyBRDF;
    class MicrofacetBRDF;
    class MicrofacetBSDF;
    class DisneyBRDF;
    class DisneyBSDF;
    class MultiBSDF;
    
    // END: BSDF
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // Phase Function
    
    class IsotropicPhaseFunction;
    class HenyeyGreensteinPhaseFunction;
    class SchlickPhaseFunction;
    
    // END: Phase Function
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // EDF
    
    class DiffuseEDF;
    class IdealDirectionalEDF;
    class IBLEDF;
    class MultiEDF;
    
    // END: EDF
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // Texture
    
    class ConstantSpectrumTexture;
    class ConstantFloatTexture;
    class ImageSpectrumTexture;
    class ImageNormalTexture;
    class ImageFloatTexture;
    class CheckerBoardSpectrumTexture;
    class CheckerBoardNormalTexture;
    class CheckerBoardFloatTexture;
    class VoronoiSpectrumTexture;
    class VoronoiNormalTexture;
    class VoronoiFloatTexture;
    class WorleyNoiseFloatTexture;
    class PerlinNoiseNormalTexture;
    class PerlinNoiseFloatTexture;
    class AnalyticSkySpectrumTexture;
    
    // END: Texture
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // Surface Material
    
    class DiffuseReflectionSurfaceMaterial;
    class SpecularReflectionSurfaceMaterial;
    class SpecularScatteringSurfaceMaterial;
    class FlippedSurfaceMaterial;
    class ModifiedWardDurReflectionSurfaceMaterial;
    class AshikhminShirleyReflectionSurfaceMaterial;
    class MicrofacetReflectionSurfaceMaterial;
    class MicrofacetScatteringSurfaceMaterial;
    class DisneyReflectionSurfaceMaterial;
    class DisneyScatteringSurfaceMaterial;
    class SummedSurfaceMaterial;
    class MixedSurfaceMaterial;
    class DiffuseEmitterSurfaceProperty;
    class IdealDirectionalEmitterSurfaceProperty;
    class IBLEmitterSurfaceProperty;
    
    // END: Surface Material
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // Medium Material
    
    class IsotropicScatteringMediumMaterial;
    class HenyeyGreensteinScatteringMediumMaterial;
    class SchlickScatteringMediumMaterial;
    
    // END: Medium Material
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // Accelerator
    
    class StandardBVH;
    class SBVH;
    class QBVH;
    
    // END: Accelerator
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // Camera
    
    class PerspectiveCamera;
    class PerspectiveIDF;
    class EquirectangularCamera;
    class EquirectangularIDF;
    
    // END: Camera
    // ----------------------------------------------------------------

    
    
    // ----------------------------------------------------------------
    // Scene
    
    class Node;
    class InternalNode;
    class ReferenceNode;
    class InfinitesimalPointNode;
    class InfiniteSphereNode;
    class PerspectiveCameraNode;
    class EquirectangularCameraNode;
    class SurfaceNode;
    struct MaterialGroupInTriangleMesh;
    class TriangleMeshNode;
    class MediumNode;
    class HomogeneousMediumNode;
    class GridMediumNode;
    class DensityGridMediumNode;
    class VacuumMediumNode;
    class CloudMediumNode;
    class Scene;
    
    // END: Scene
    // ----------------------------------------------------------------
    
    
    
    // ----------------------------------------------------------------
    // Renderer
    
    class PTRenderer;
    class BPTRenderer;
    class AMCMCPPMRenderer;
    class VolumetricPTRenderer;
    class VolumetricBPTRenderer;
    
    // END: Renderer
    // ----------------------------------------------------------------
}

#endif /* __SLR_declarations__ */
