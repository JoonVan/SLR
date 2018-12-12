﻿//
//  medium_materials.cpp
//
//  Created by 渡部 心 on 2017/02/05.
//  Copyright (c) 2017年 渡部 心. All rights reserved.
//

#include "medium_materials.h"

#include <libSLR/MediumMaterial/medium_material_headers.h>
#include "textures.h"

namespace SLRSceneGraph {
    MediumMaterial::~MediumMaterial() {
        delete m_rawData;
    }
    
    
    
    EmitterMediumProperty::~EmitterMediumProperty() {
        delete m_rawData;
    }
    
    
    
    EmitterMediumMaterial::EmitterMediumMaterial(const MediumMaterialRef &mat, const EmitterMediumPropertyRef &emit) :
    m_mat(mat), m_emit(emit) {
        m_rawData = new SLR::EmitterMediumMaterial(mat->getRaw(), emit->getRaw());
    }
    
    
    
    IsotropicScatteringMediumMaterial::IsotropicScatteringMediumMaterial() {
        m_rawData = new SLR::IsotropicScatteringMediumMaterial();
    }
    
    
    
    HenyeyGreensteinScatteringMediumMaterial::HenyeyGreensteinScatteringMediumMaterial(const FloatTextureRef &g) : m_g(g) {
        m_rawData = new SLR::HenyeyGreensteinScatteringMediumMaterial(m_g->getRaw());
    }
    
    
    
    MediumMaterialRef MediumMaterial::createIsotropic() {
        return createShared<IsotropicScatteringMediumMaterial>();
    }
    
    MediumMaterialRef MediumMaterial::createHenyeyGreenstein(const FloatTextureRef &g) {
        return createShared<HenyeyGreensteinScatteringMediumMaterial>(g);
    }
}
