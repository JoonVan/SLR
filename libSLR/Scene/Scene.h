﻿//
//  Scene.h
//
//  Created by 渡部 心 on 2016/09/19.
//  Copyright (c) 2016年 渡部 心. All rights reserved.
//

#ifndef __SLR_Scene__
#define __SLR_Scene__

#include "../defines.h"
#include "../declarations.h"
#include "../Core/surface_object.h"
#include "../Core/medium_object.h"
#include "node.h"

namespace SLR {
    class SLR_API Scene {
        Node* m_rootNode;
        InfiniteSphereNode* m_envNode;
        
        Allocator* m_sceneMem;
        SurfaceObjectAggregate* m_surfaceAggregate;
        MediumObjectAggregate* m_mediumAggregate;
        InfiniteSphereSurfaceObject* m_envSphere;
        Point3D m_worldCenter;
        float m_worldRadius;
        float m_worldDiscArea;
        Camera* m_camera;
    public:
        Scene(Node* rootNode) : m_rootNode(rootNode), m_envNode(nullptr) { }
        
        void setEnvironmentNode(InfiniteSphereNode* envNode) { m_envNode = envNode; }
        
        void build(Allocator* sceneMem);
        void destory();
        
        const Camera* getCamera() const { return m_camera; }
        Point3D getWorldCenter() const { return m_worldCenter; }
        float getWorldRadius() const { return m_worldRadius; }
        float getWorldDiscArea() const { return m_worldDiscArea; }
        
        bool intersect(const Ray &ray, const RaySegment &segment, LightPathSampler &pathSampler, SurfaceInteraction* si) const;
        bool interact(const Ray &ray, const RaySegment &segment, const WavelengthSamples &wls, LightPathSampler &pathSampler, ArenaAllocator &mem,
                      Interaction** interact, SampledSpectrum* medThroughput, bool* singleWavelength) const;
        bool testVisibility(const SurfacePoint &shdP, const SurfacePoint &lightP, float time, float* fractionalVisibility) const;
        bool testVisibility(const InteractionPoint* shdP, const InteractionPoint* lightP, float time,
                            const WavelengthSamples &wls, LightPathSampler &pathSampler, SampledSpectrum* fractionalVisibility, bool* singleWavelength) const;
        void selectSurfaceLight(float u, float time, SurfaceLight* light, float* prob) const;
        void selectLight(float u, float time, ArenaAllocator &mem, Light** light, float* prob) const;
    };
}

#endif /* __SLR_Scene__ */
