//
//  DiffuseEmission.cpp
//
//  Created by 渡部 心 on 2015/09/06.
//  Copyright (c) 2015年 渡部 心. All rights reserved.
//

#include "DiffuseEmission.h"
#include "../EDFs/basic_EDFs.h"
#include "../Memory/ArenaAllocator.h"
#include "textures.h"

Spectrum DiffuseEmission::emittance(const SurfacePoint &surfPt) const {
    return m_emittance->evaluate(surfPt.texCoord);
}

EDF* DiffuseEmission::getEDF(const SurfacePoint &surfPt, ArenaAllocator &mem, float scale) const {
    return mem.create<DiffuseEDF>();
}
