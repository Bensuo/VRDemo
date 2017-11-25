#pragma once

#include <OVR_CAPI_GL.h>
#include "TextureBuffer.h"
#include "Transform3D.hpp"
#include "Avatar.h"

struct VRControllerState
{
    Transform3D Transform;
};

class VRInputState
{
private:
    VRControllerState m_left;
    VRControllerState m_right;
public:
    VRInputState(const VRControllerState left, const VRControllerState right)
    : m_left(left), m_right(right) {}

    const VRControllerState& GetLeft() { return m_left; }
    const VRControllerState& GetRight() { return m_right; }
};

class VRSystem
{
public:
	VRSystem();
	~VRSystem();

	void Init();
	void BeginFrame();
	void EndFrame();
	void ClearEyeBuffer(int eye);
	void CommitBuffer(int eye);
	void RenderMirror(int w, int h);
	glm::mat4 GetViewFromEye(int eye);
	glm::mat4 GetProjectionMatrix(int eye);

    void DrawAvatar(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& viewPos);
    void UpdateAvatar(const float delta_time);

    VRInputState GetInputState();

    glm::vec3 EyePos(int eye);
private:
    int frameIndex;
	ovrSession session;
	ovrGraphicsLuid luid;
	ovrHmdDesc hmd_desc;
	TextureBuffer *eye_render_texture[2] = { nullptr, nullptr };
	GLuint eye_depth_buffer[2] = { 0,0 };
	ovrMirrorTexture mirror_texture;
	GLuint mirror_fbo;
	GLuint mirror_tex_id;
	ovrLayerEyeFov layer;
    Avatar m_avatar;
};

