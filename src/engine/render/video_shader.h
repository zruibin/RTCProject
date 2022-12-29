/*
 * video_shader.h
 *
 * Created by Ruibin.Chow on 2022/12/28.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef VIDEO_SHADER_H
#define VIDEO_SHADER_H

#include <memory>
#include "engine/render/gl_defines.h"
#include <absl/types/optional.h>
#include <api/video/video_rotation.h>

namespace engine {


class VideoShader : public std::enable_shared_from_this<VideoShader> {
    
public:
    VideoShader() = default;
    ~VideoShader();

    bool CreateAndSetupI420Program();
    bool CreateAndSetupNV12Program();
    bool PrepareVertexBuffer(webrtc::VideoRotation rotation);
    void ApplyShadingForFrame(int width,
                              int height,
                              webrtc::VideoRotation rotation,
                              GLuint yPlane,
                              GLuint uPlane,
                              GLuint vPlane);
    void ApplyShadingForFrame(int width,
                              int height,
                              webrtc::VideoRotation rotation,
                              GLuint yPlane,
                              GLuint uvPlane);

protected:
    GLuint CreateShader(GLenum type, const GLchar* source);
    GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader);
    GLuint CreateProgramFromFragmentSource(const char fragmentShaderSource[]);
    bool CreateVertexBuffer(GLuint* vertexBuffer, GLuint* vertexArray);
    void SetVertexData(webrtc::VideoRotation rotation);

private:
    GLuint vertexBuffer_ = 0;
    GLuint vertexArray_ = 0;

    // Store current rotation and only upload new vertex data when rotation changes.
    absl::optional<webrtc::VideoRotation> currentRotation_;

    GLuint i420Program_ = 0;
    GLuint nv12Program_ = 0;
};

}


#endif /* !VIDEO_SHADER_H */
