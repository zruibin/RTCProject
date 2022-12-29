/*
 * i420_texture_cache.h
 *
 * Created by Ruibin.Chow on 2022/12/28.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef I420_TEXTURE_CACHE_H
#define I420_TEXTURE_CACHE_H

#include <memory>
#include <vector>
#include <stdint.h>
#include <api/video/i420_buffer.h>
#include <api/video/video_frame.h>
#include "engine/render/gl_defines.h"

namespace engine {

namespace {
    const GLsizei kNumTextureSets = 2;
    const GLsizei kNumTexturesPerSet = 3;
    const GLsizei kNumTextures = kNumTexturesPerSet * kNumTextureSets;
};

class I420TextureCache : public std::enable_shared_from_this<I420TextureCache> {
    
public:
    I420TextureCache() = default;
    ~I420TextureCache();
    
public:
    void Init();
    void UploadFrameToTextures(const webrtc::VideoFrame& frame);
    GLuint YTexture();
    GLuint UTexture();
    GLuint VTexture();
    
protected:
    void SetupTextures();
    void UploadPlane(const uint8_t* plane,
                     GLuint texture,
                     size_t width,
                     size_t height,
                     int32_t stride);
    
private:
    bool hasUnpackRowLength_;
    GLint currentTextureSet_ = 0;
    // Handles for OpenGL constructs.
    GLuint textures_[kNumTextures];
    // Used to create a non-padded plane for GPU upload when we receive padded frames.
    std::vector<uint8_t> planeBuffer_;
};

}

#endif /* !I420_TEXTURE_CACHE_H */
