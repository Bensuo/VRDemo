#pragma once

#include <OVR_CAPI_GL.h>
#include "TextureBuffer.h"
#include "Signal.h"
#include <glm/glm.hpp>
#include "Transform3D.hpp"
#include "Avatar.h"
#include "glm/vec3.hpp"
#include <unordered_map>

enum Button
{
    Button_A,
    Button_B,
    Button_X,
    Button_Y
};

struct VRControllerState
{
    Transform3D Transform;
    glm::vec2 ThumbstickAxis;
    std::unordered_map<Button, bool> buttons;
public:
    VRControllerState() {}

    explicit VRControllerState(const std::unordered_map<Button, bool>& buttons)
        : buttons(buttons)
    {}

    bool IsButtonUp(const Button button) const
    {
        const auto& it = buttons.find(button);

        if (it != buttons.end())
        {
            return it->second;
        }

        return false;
    }

    bool IsButtonDown(const Button button) const
    {
        return !IsButtonUp(button);
    }
};

class VRInputState
{
private:
    VRControllerState m_left;
    VRControllerState m_right;
public:
    VRInputState() {}
    VRInputState(const VRControllerState& left, const VRControllerState& right)
    : m_left(left), m_right(right) {}

    const VRControllerState& GetLeft() const { return m_left; }
    const VRControllerState& GetRight() const { return m_right; }
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

    Engine::Signal<> InitialiseFail;
    Engine::Signal<> CreateSessionFail;

    glm::vec3 GetOrigin();

    glm::vec3 GetFront();

    void DrawAvatar(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& viewPos);
    void UpdateAvatar(const float delta_time);

    VRInputState GetInputState() const;

    Transform3D GetHMDTransform();
    Transform3D GetEyeTransform(int eye);
    glm::vec3 GetEyePos(int eye);
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
    ovrTrackingState hmdState;
};

