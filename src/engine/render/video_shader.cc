/*
 *
 * video_shader.cc
 *
 * Created by Ruibin.Chow on 2022/12/28.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "video_shader.h"
#include <algorithm>
#include <array>
#include <string>
#include <memory>
#include "log/logging.h"

namespace {

// Vertex shader doesn't do anything except pass coordinates through.
const std::string kRTCVertexShaderSource = R"(
#version 150
in vec2 position;
in vec2 texcoord;
out vec2 v_texcoord;

void main() {
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    v_texcoord = texcoord;
}

)";

static const int kYTextureUnit = 0;
static const int kUTextureUnit = 1;
static const int kVTextureUnit = 2;
static const int kUvTextureUnit = 1;

// Fragment shader converts YUV values from input textures into a final RGB
// pixel. The conversion formula is from http://www.fourcc.org/fccyvrgb.php.
const std::string kI420FragmentShaderSource = R"(
#version 150

precision highp float;
in vec2 v_texcoord;

uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;

out vec4 fragColor;

void main() {
    float y, u, v, r, g, b;
    y = texture(s_textureY, v_texcoord).r;
    u = texture(s_textureU, v_texcoord).r;
    v = texture(s_textureV, v_texcoord).r;

    u = u - 0.5;
    v = v - 0.5;
    r = y + 1.403 * v;
    g = y - 0.344 * u - 0.714 * v;
    b = y + 1.770 * u;

    fragColor = vec4(r, g, b, 1.0);
}
)";


const std::string kNV12FragmentShaderSource = R"(
#version 150
precision mediump float;
in vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureUV;

out vec4 fragColor;

void main() {
    mediump float y;
    mediump vec2 uv;
    y = texture(s_textureY, v_texcoord).r;
    uv = texture(s_textureUV, v_texcoord).ra - vec2(0.5, 0.5);
    fragColor= vec4(y + 1.403 * uv.y,
                    y - 0.344 * uv.x - 0.714 * uv.y,
                    y + 1.770 * uv.x,
                    1.0);
}
)";

}


namespace engine {

VideoShader::~VideoShader() {
    glDeleteProgram(i420Program_);
    glDeleteProgram(nv12Program_);
    glDeleteBuffers(1, &vertexBuffer_);
    glDeleteVertexArrays(1, &vertexArray_);
}

// Compiles a shader of the given |type| with GLSL source |source| and returns
// the shader handle or 0 on error.
GLuint VideoShader::CreateShader(GLenum type, const GLchar *source) {
    GLuint shader = glCreateShader(type);
    if (!shader) {
        return 0;
    }
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint compileStatus = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        GLint logLength = 0;
        // The null termination character is included in the returned log length.
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            std::unique_ptr<char[]> compileLog(new char[logLength]);
            // The returned string is null terminated.
            glGetShaderInfoLog(shader, logLength, NULL, compileLog.get());
            Log(ERROR) << "Shader compile error: " << compileLog.get();
        }
        glDeleteShader(shader);
        shader = 0;
    }
    return shader;
}

// Links a shader program with the given vertex and fragment shaders and
// returns the program handle or 0 on error.
GLuint VideoShader::CreateProgram(GLuint vertexShader, GLuint fragmentShader) {
    if (vertexShader == 0 || fragmentShader == 0) {
        return 0;
    }
    GLuint program = glCreateProgram();
    if (!program) {
        return 0;
    }
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        glDeleteProgram(program);
        program = 0;
    }
    return program;
}

// Creates and links a shader program with the given fragment shader source and
// a plain vertex shader. Returns the program handle or 0 on error.
GLuint VideoShader::CreateProgramFromFragmentSource(const char fragmentShaderSource[]) {
    GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, kRTCVertexShaderSource.c_str());
    //RTC_CHECK(vertexShader) << "failed to create vertex shader";
    if (vertexShader == 0) {
        Log(ERROR) << "failed to create vertex shader";
    }
    GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    //RTC_CHECK(fragmentShader) << "failed to create fragment shader";
    if (fragmentShader == 0) {
        Log(ERROR) << "failed to create fragment shader";
    }
    GLuint program = CreateProgram(vertexShader, fragmentShader);
    // Shaders are created only to generate program.
    if (vertexShader) {
        glDeleteShader(vertexShader);
    }
    if (fragmentShader) {
        glDeleteShader(fragmentShader);
    }

    // Set vertex shader variables 'position' and 'texcoord' in program.
    GLint position = glGetAttribLocation(program, "position");
    GLint texcoord = glGetAttribLocation(program, "texcoord");
    if (position < 0 || texcoord < 0) {
        glDeleteProgram(program);
        return 0;
    }

    // Read position attribute with size of 2 and stride of 4 beginning at the start of the array. The
    // last argument indicates offset of data within the vertex buffer.
    glVertexAttribPointer(position,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          4 * sizeof(GLfloat),
                          (void *)0);
    glEnableVertexAttribArray(position);

    // Read texcoord attribute  with size of 2 and stride of 4 beginning at the first texcoord in the
    // array. The last argument indicates offset of data within the vertex buffer.
    glVertexAttribPointer(texcoord,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          4 * sizeof(GLfloat),
                          (void *)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(texcoord);

    return program;
}

bool VideoShader::CreateVertexBuffer(GLuint *vertexBuffer, GLuint *vertexArray) {
    glGenVertexArrays(1, vertexArray);
    if (*vertexArray == 0) {
        return false;
    }
    glBindVertexArray(*vertexArray);

    glGenBuffers(1, vertexBuffer);
    if (*vertexBuffer == 0) {
        glDeleteVertexArrays(1, vertexArray);
        return false;
    }
    glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    return true;
}

// Set vertex data to the currently bound vertex buffer.
void VideoShader::SetVertexData(webrtc::VideoRotation rotation) {
    // When modelview and projection matrices are identity (default) the world is
    // contained in the square around origin with unit size 2. Drawing to these
    // coordinates is equivalent to drawing to the entire screen. The texture is
    // stretched over that square using texture coordinates (u, v) that range
    // from (0, 0) to (1, 1) inclusive. Texture coordinates are flipped vertically
    // here because the incoming frame has origin in upper left hand corner but
    // OpenGL expects origin in bottom left corner.
    std::array<std::array<GLfloat, 2>, 4> UVCoords = { {
        {{0, 1}},  // Lower left.
        {{1, 1}},  // Lower right.
        {{1, 0}},  // Upper right.
        {{0, 0}},  // Upper left.
    } };

    // Rotate the UV coordinates.
    int rotation_offset;
    switch (rotation) {
    case webrtc::kVideoRotation_0:
        rotation_offset = 0;
        break;
    case webrtc::kVideoRotation_90:
        rotation_offset = 1;
        break;
    case webrtc::kVideoRotation_180:
        rotation_offset = 2;
        break;
    case webrtc::kVideoRotation_270:
        rotation_offset = 3;
        break;
    }
    std::rotate(UVCoords.begin(), UVCoords.begin() + rotation_offset,
        UVCoords.end());

    const GLfloat gVertices[] = {
        // X, Y, U, V.
        -1, -1, UVCoords[0][0], UVCoords[0][1],
         1, -1, UVCoords[1][0], UVCoords[1][1],
         1,  1, UVCoords[2][0], UVCoords[2][1],
        -1,  1, UVCoords[3][0], UVCoords[3][1],
    };

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(gVertices), gVertices);
}

bool VideoShader::CreateAndSetupI420Program() {
    assert(!i420Program_);
    i420Program_ = CreateProgramFromFragmentSource(kI420FragmentShaderSource.c_str());
    if (!i420Program_) {
        return false;
    }
    GLint ySampler = glGetUniformLocation(i420Program_, "s_textureY");
    GLint uSampler = glGetUniformLocation(i420Program_, "s_textureU");
    GLint vSampler = glGetUniformLocation(i420Program_, "s_textureV");

    if (ySampler < 0 || uSampler < 0 || vSampler < 0) {
        Log(ERROR) << "Failed to get uniform variable locations in I420 shader";
        glDeleteProgram(i420Program_);
        i420Program_ = 0;
        return false;
    }

    glUseProgram(i420Program_);
    glUniform1i(ySampler, kYTextureUnit);
    glUniform1i(uSampler, kUTextureUnit);
    glUniform1i(vSampler, kVTextureUnit);

    return true;
}

bool VideoShader::CreateAndSetupNV12Program() {
    assert(!nv12Program_);
    nv12Program_ = CreateProgramFromFragmentSource(kNV12FragmentShaderSource.c_str());
    if (!nv12Program_) {
        return false;
    }
    GLint ySampler = glGetUniformLocation(nv12Program_, "s_textureY");
    GLint uvSampler = glGetUniformLocation(nv12Program_, "s_textureUV");

    if (ySampler < 0 || uvSampler < 0) {
        Log(ERROR) << "Failed to get uniform variable locations in NV12 shader";
        glDeleteProgram(nv12Program_);
        nv12Program_ = 0;
        return false;
    }

    glUseProgram(nv12Program_);
    glUniform1i(ySampler, kYTextureUnit);
    glUniform1i(uvSampler, kUvTextureUnit);

    return true;
}

bool VideoShader::PrepareVertexBuffer(webrtc::VideoRotation rotation) {
    if (!vertexBuffer_ && !CreateVertexBuffer(&vertexBuffer_, &vertexArray_)) {
        Log(ERROR) << "Failed to setup vertex buffer";
        return false;
    }

    glBindVertexArray(vertexArray_);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    if (!currentRotation_ || rotation != *currentRotation_) {
        currentRotation_ = absl::optional<webrtc::VideoRotation>(rotation);
        SetVertexData(*currentRotation_);
    }
    return true;
}

void VideoShader::ApplyShadingForFrame(int width,
                                       int height,
                                       webrtc::VideoRotation rotation,
                                       GLuint yPlane,
                                       GLuint uPlane,
                                       GLuint vPlane) {
    if (!PrepareVertexBuffer(rotation)) {
        return;
    }

    if (!i420Program_ && !CreateAndSetupI420Program()) {
        Log(ERROR) << "Failed to setup I420 program";
        return;
    }

    glUseProgram(i420Program_);

    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + kYTextureUnit));
    glBindTexture(GL_TEXTURE_2D, yPlane);

    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + kUTextureUnit));
    glBindTexture(GL_TEXTURE_2D, uPlane);

    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + kVTextureUnit));
    glBindTexture(GL_TEXTURE_2D, vPlane);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void VideoShader::ApplyShadingForFrame(int width,
                                       int height,
                                       webrtc::VideoRotation rotation,
                                       GLuint yPlane,
                                       GLuint uvPlane) {
    if (!PrepareVertexBuffer(rotation)) {
        return;
    }

    if (!nv12Program_ && !CreateAndSetupNV12Program()) {
        Log(ERROR) << "Failed to setup NV12 shader";
        return;
    }

    glUseProgram(nv12Program_);

    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + kYTextureUnit));
    glBindTexture(GL_TEXTURE_2D, yPlane);

    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + kUvTextureUnit));
    glBindTexture(GL_TEXTURE_2D, uvPlane);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

}

