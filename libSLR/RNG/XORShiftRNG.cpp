﻿//
//  XORShiftRNG.cpp
//
//  Created by 渡部 心 on 11/07/21.
//  Copyright (c) 2014年 渡部 心. All rights reserved.
//

#include "XORShiftRNG.h"

namespace SLR {
    template <> 
    SLR_API XORShiftRNGTemplate<Types32bit>::XORShiftRNGTemplate() {
        Types32bit::Int seed = (Types32bit::Int)time(NULL);
        for (unsigned int i = 0; i < 4; ++i)
            m_state[i] = seed = 1812433253U * (seed ^ (seed >> 30)) + i;
        for (int i = 0; i < 50; ++i)
            getUInt();
    }
    
    template <> 
    SLR_API XORShiftRNGTemplate<Types32bit>::XORShiftRNGTemplate(Types32bit::Int seed) {
        for (int i = 0; i < 4; ++i)
            m_state[i] = seed = 1812433253U * (seed ^ (seed >> 30)) + i;
        for (int i = 0; i < 50; ++i)
            getUInt();
    }
    
    template <> 
    SLR_API Types32bit::UInt XORShiftRNGTemplate<Types32bit>::getUInt() {
        Types32bit::UInt* a = m_state;
        Types32bit::UInt t(a[0] ^ (a[0] << 11));
        a[0] = a[1];
        a[1] = a[2];
        a[2] = a[3];
        return a[3] = (a[3] ^ (a[3] >> 19)) ^ (t ^ (t >> 8));
    }
    
    template class SLR_API XORShiftRNGTemplate<Types32bit>;
}
