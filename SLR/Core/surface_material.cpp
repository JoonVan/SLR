//
//  surface_material.cpp
//
//  Created by 渡部 心 on 2015/04/06.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#include "surface_material.h"
#include "../Memory/ArenaAllocator.h"
#include "../Core/directional_distribution_functions.h"
#include "../Core/textures.h"

#include "../SurfaceMaterials/basic_SurfaceMaterials.h"
#include "../SurfaceMaterials/ModifiedWardDurReflection.h"
#include "../SurfaceMaterials/AshikhminReflection.h"
#include "../SurfaceMaterials/MixedSurfaceMaterial.h"
#include "../SurfaceMaterials/AddedSurfaceMaterial.h"
#include "../SurfaceMaterials/DiffuseEmission.h"
#include "../SurfaceMaterials/IBLEmission.h"

Fresnel* SpatialFresnelNoOp::getFresnel(const SurfacePoint &surfPt, ArenaAllocator &mem) const {
    return mem.create<FresnelNoOp>();
}

Fresnel* SpatialFresnelConductor::getFresnel(const SurfacePoint &surfPt, ArenaAllocator &mem) const {
    return mem.create<FresnelConductor>(m_eta->evaluate(surfPt.texCoord), m_k->evaluate(surfPt.texCoord));
}

Fresnel* SpatialFresnelDielectric::getFresnel(const SurfacePoint &surfPt, ArenaAllocator &mem) const {
    return mem.create<FresnelDielectric>(m_etaExt->evaluate(surfPt.texCoord), m_etaInt->evaluate(surfPt.texCoord));
}

BSDF* EmitterSurfaceMaterial::getBSDF(const SurfacePoint &surfPt, ArenaAllocator &mem, float scale) const {
    return m_mat->getBSDF(surfPt, mem);
};

Spectrum EmitterSurfaceMaterial::emittance(const SurfacePoint &surfPt) const {
    return m_emit->emittance(surfPt);
};

EDF* EmitterSurfaceMaterial::getEDF(const SurfacePoint &surfPt, ArenaAllocator &mem, float scale) const {
    return m_emit->getEDF(surfPt, mem);
};


SurfaceMaterialRef SurfaceMaterial::createMatte(const SpectrumTextureRef &reflectance, const FloatTextureRef &sigma) {
    return createShared<DiffuseReflection>(reflectance, sigma);
}

SurfaceMaterialRef SurfaceMaterial::createMetal(const SpectrumTextureRef &coeffR, const SpectrumTextureRef &eta, const SpectrumTextureRef &k) {
    SpatialFresnelRef fresnel = createShared<SpatialFresnelConductor>(eta, k);
    return createShared<SpecularReflection>(coeffR, fresnel);
}

SurfaceMaterialRef SurfaceMaterial::createGlass(const SpectrumTextureRef &coeffR, const SpectrumTextureRef &coeffT, const SpatialFresnelDielectricRef &frDiel) {
    SurfaceMaterialRef r = createShared<SpecularReflection>(coeffR, frDiel);
    SurfaceMaterialRef t = createShared<SpecularTransmission>(coeffT, frDiel->etaExt(), frDiel->etaInt());
    return createAddedMaterial(r, t);
}

SurfaceMaterialRef SurfaceMaterial::createModifiedWardDur(const SpectrumTextureRef &reflectance, const FloatTextureRef &anisoX, const FloatTextureRef &anisoY) {
    return createShared<ModifiedWardDurReflection>(reflectance, anisoX, anisoY);
}

SurfaceMaterialRef SurfaceMaterial::createAshikhminShirley(const SpectrumTextureRef &Rd, const SpectrumTextureRef &Rs, const FloatTextureRef &nu, const FloatTextureRef &nv) {
    SurfaceMaterialRef specular = createShared<AshikhminSpecularReflection>(Rs, nu, nv);
    SurfaceMaterialRef diffuse = createShared<AshikhminDiffuseReflection>(Rs, Rd);
    return createAddedMaterial(specular, diffuse);
}

SurfaceMaterialRef SurfaceMaterial::createAddedMaterial(const SurfaceMaterialRef &mat0, const SurfaceMaterialRef &mat1) {
    return createShared<AddedSurfaceMaterial>(mat0, mat1);
}

SurfaceMaterialRef SurfaceMaterial::createMixedMaterial(const SurfaceMaterialRef &mat0, const SurfaceMaterialRef &mat1, const FloatTextureRef &factor) {
    return createShared<MixedSurfaceMaterial>(mat0, mat1, factor);
}

SurfaceMaterialRef SurfaceMaterial::createEmitterSurfaceMaterial(const SurfaceMaterialRef &mat, const EmitterSurfacePropertyRef &emit) {
    return createShared<EmitterSurfaceMaterial>(mat, emit);
}

EmitterSurfacePropertyRef EmitterSurfaceProperty::createDiffuseEmitter(const SpectrumTextureRef &emittance) {
    return createShared<DiffuseEmission>(emittance);
}