#pragma once
#include <OVR_CAPI_GL.h>
#include "TextureBuffer.h"
#include "Signal.h"
#include <glm/glm.hpp>

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
	glm::mat4 GetViewFromEye(glm::vec3 eyePos, int eye, glm::vec3& front, float rotationY = 0.0f);
	glm::mat4 GetProjectionMatrix(int eye);

    Engine::Signal<> InitialiseFail;
    Engine::Signal<> CreateSessionFail;
private:
	ovrSession session;
	ovrGraphicsLuid luid;
	ovrHmdDesc hmd_desc;
	TextureBuffer *eye_render_texture[2] = { nullptr, nullptr };
	GLuint eye_depth_buffer[2] = { 0,0 };
	ovrMirrorTexture mirror_texture;
	GLuint mirror_fbo;
	GLuint mirror_tex_id;
	ovrLayerEyeFov layer;
};

