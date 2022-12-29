/*
 *
 * i420_texture_cache.cc
 *
 * Created by Ruibin.Chow on 2022/12/28.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "i420_texture_cache.h"

namespace engine {

I420TextureCache::~I420TextureCache() {
    glDeleteTextures(kNumTextures, textures_);
}

void I420TextureCache::Init() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    SetupTextures();
}

void I420TextureCache::UploadFrameToTextures(const webrtc::VideoFrame& frame) {
    currentTextureSet_ = (currentTextureSet_ + 1) % kNumTextureSets;
    
    rtc::scoped_refptr<webrtc::VideoFrameBuffer> vfb = frame.video_frame_buffer();
    if (!vfb) {
        return;
    }
    
    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer = vfb->ToI420();
    
    const int chromaWidth = buffer->ChromaWidth();
    const int chromaHeight = buffer->ChromaHeight();
    if (buffer->StrideY() != frame.width() ||
        buffer->StrideU() != chromaWidth ||
        buffer->StrideV() != chromaWidth) {
        planeBuffer_.resize(buffer->width() * buffer->height());
    }
    
    UploadPlane(buffer->DataY(),
                YTexture(),
                buffer->width(),
                buffer->height(),
                buffer->StrideY());
    
    UploadPlane(buffer->DataU(),
                UTexture(),
                buffer->ChromaWidth(),
                buffer->ChromaHeight(),
                buffer->StrideU());
    
    UploadPlane(buffer->DataV(),
                VTexture(),
                buffer->ChromaWidth(),
                buffer->ChromaHeight(),
                buffer->StrideV());
}

GLuint I420TextureCache::YTexture() {
    return textures_[currentTextureSet_ * kNumTexturesPerSet];
}

GLuint I420TextureCache::UTexture() {
    return textures_[currentTextureSet_ * kNumTexturesPerSet + 1];
}

GLuint I420TextureCache::VTexture() {
    return textures_[currentTextureSet_ * kNumTexturesPerSet + 2];
}

void I420TextureCache::SetupTextures() {
    glGenTextures(kNumTextures, textures_);
    // Set parameters for each of the textures we created.
    for (GLsizei i = 0; i < kNumTextures; i++) {
        glBindTexture(GL_TEXTURE_2D, textures_[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

void I420TextureCache::UploadPlane(const uint8_t* plane,
                                   GLuint texture,
                                   size_t width,
                                   size_t height,
                                   int32_t stride) {
    glBindTexture(GL_TEXTURE_2D, texture);
    
    const uint8_t *uploadPlane = plane;
    
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    if ((size_t)stride != width) {
        if (hasUnpackRowLength_) {
            // GLES3 allows us to specify stride.
            glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RED,
                         static_cast<GLsizei>(width),
                         static_cast<GLsizei>(height),
                         0,
                         GL_RED,
                         GL_UNSIGNED_BYTE,
                         uploadPlane);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            return;
        }
        else {
            // Make an unpadded copy and upload that instead. Quick profiling showed
            // that this is faster than uploading row by row using glTexSubImage2D.
            uint8_t *unpaddedPlane = planeBuffer_.data();
            for (size_t y = 0; y < height; ++y) {
                memcpy(unpaddedPlane + y * width, plane + y * stride, width);
            }
            uploadPlane = unpaddedPlane;
        }
    }
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RED,
                 static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height),
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 uploadPlane);
}

}

