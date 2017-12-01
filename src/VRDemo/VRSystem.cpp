#include "VRSystem.h"
#include <iostream>
#include <Extras/OVR_Math.h>
#include "OvrConversions.h"

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#include <OVR_Platform.h>
#include <OVR_Avatar.h>

VRSystem::VRSystem()
{}

VRSystem::~VRSystem()
{}

void VRSystem::Init()
{
    const char* TEST_APP_ID = "958062084316416";

    if (ovr_PlatformInitializeWindows(TEST_APP_ID) != ovrPlatformInitialize_Success)
    {
        std::cout << "OVR FAILED INIT" << std::endl;
    }

    //Init vr
    ovrResult result = ovr_Initialize(nullptr);
    if (OVR_FAILURE(result))
    {
        std::cout << "OVR FAILED INIT" << std::endl;
        InitialiseFail();
        return;
    }

    result = ovr_Create(&session, &luid);
    if (OVR_FAILURE(result))
    {
        ovr_Shutdown();
        std::cout << "OVR ERROR SHUTTING DOWN" << std::endl;
        CreateSessionFail();
        return;
    }

    //m_vr_resolution = m_hmd_desc.Resolution;
    hmd_desc = ovr_GetHmdDesc(session);

    ovr_SetTrackingOriginType(session, ovrTrackingOrigin_FloorLevel);

    m_avatar = Avatar(session);

	//Setup texture and depth buffers
	for (int eye = 0; eye < 2; eye++)
	{
		ovrSizei size = ovr_GetFovTextureSize(session, ovrEyeType(eye), hmd_desc.DefaultEyeFov[eye], 1);
		eye_render_texture[eye] = new TextureBuffer(session, size);

		//depth buffer
		glGenTextures(1, &eye_depth_buffer[eye]);
		glBindTexture(GL_TEXTURE_2D, eye_depth_buffer[eye]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		GLenum internalFormat = GL_DEPTH_COMPONENT24;
		GLenum type = GL_UNSIGNED_INT;
		/*if (GLE_ARB_depth_buffer_float)
		{
		internalFormat = GL_DEPTH_COMPONENT32F;
		type = GL_FLOAT;
		}*/

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.w, size.h, 0, GL_DEPTH_COMPONENT, type, NULL);
	}
	ovrMirrorTextureDesc desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = 1280;
	desc.Height = 720;
	desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

	// Create mirror texture and an FBO used to copy mirror texture to back buffer
	result = ovr_CreateMirrorTextureGL(session, &desc, &mirror_texture);
	result = ovr_GetMirrorTextureBufferGL(session, mirror_texture, &mirror_tex_id);

	glGenFramebuffers(1, &mirror_fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirror_fbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirror_tex_id, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	ovrSessionStatus status;
	ovr_GetSessionStatus(session, &status);
	while (!status.HmdMounted)
	{
		ovr_GetSessionStatus(session, &status);
	}
	ovr_RecenterTrackingOrigin(session);

}

void VRSystem::BeginFrame()
{
	ovrEyeRenderDesc eyeRenderDesc[2];
	eyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left, hmd_desc.DefaultEyeFov[0]);
	eyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, hmd_desc.DefaultEyeFov[1]);

	ovrPosef HmdToEyePose[2] = { eyeRenderDesc[0].HmdToEyePose,
		eyeRenderDesc[1].HmdToEyePose };
	// sensorSampleTime is fed into the layer later
	//layer init
	layer = ovrLayerEyeFov();
	layer.Header.Type = ovrLayerType_EyeFov;
	layer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft; //Because opengl
	layer.ColorTexture[0] = eye_render_texture[0]->TextureChain;
	layer.ColorTexture[1] = eye_render_texture[1]->TextureChain;
	layer.Fov[0] = eyeRenderDesc[0].Fov;
	layer.Fov[1] = eyeRenderDesc[1].Fov;
	ovrSizei bufferSize = eye_render_texture[0]->GetSize();
	layer.Viewport[0] = OVR::Recti(0, 0, bufferSize.w, bufferSize.h);
	bufferSize = eye_render_texture[1]->GetSize();
	layer.Viewport[1] = OVR::Recti(0, 0, bufferSize.w, bufferSize.h);
	layer.SensorSampleTime = ovr_GetTimeInSeconds();
	double seconds = ovr_GetPredictedDisplayTime(session, 0);
	hmdState = ovr_GetTrackingState(session, seconds, ovrTrue);
	ovr_CalcEyePoses(hmdState.HeadPose.ThePose, HmdToEyePose, layer.RenderPose);

	//Wait to begin frame drawing
	ovrResult result = ovr_WaitToBeginFrame(session, 0);
	result = ovr_BeginFrame(session, 0);
}

void VRSystem::EndFrame()
{
	ovrLayerHeader* layers = &layer.Header;
    
    ovrResult result = ovr_EndFrame(session, 0, nullptr, &layers, 1);

	//frameIndex++;
}

void VRSystem::ClearEyeBuffer(int eye)
{
	eye_render_texture[eye]->SetAndClearRenderSurface(eye_depth_buffer[eye]);
}

void VRSystem::CommitBuffer(int eye)
{
	eye_render_texture[eye]->UnsetRenderSurface();
	eye_render_texture[eye]->Commit();
}

void VRSystem::RenderMirror(int w, int h)
{
	// Blit mirror texture to back buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirror_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//glDrawBuffer(GL_BACK);
	glBlitFramebuffer(0, h, w, 0,
		0, 0, w, h,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

glm::mat4 VRSystem::GetProjectionMatrix(int eye)
{
    ovrMatrix4f ovrProjection = ovrMatrix4f_Projection(hmd_desc.DefaultEyeFov[eye], 0.01f, 1000.0f, ovrProjection_None);
    return glm::mat4(
        ovrProjection.M[0][0], ovrProjection.M[1][0], ovrProjection.M[2][0], ovrProjection.M[3][0],
        ovrProjection.M[0][1], ovrProjection.M[1][1], ovrProjection.M[2][1], ovrProjection.M[3][1],
        ovrProjection.M[0][2], ovrProjection.M[1][2], ovrProjection.M[2][2], ovrProjection.M[3][2],
        ovrProjection.M[0][3], ovrProjection.M[1][3], ovrProjection.M[2][3], ovrProjection.M[3][3]
    );
}

glm::vec3 VRSystem::GetOrigin()
{
    return _glmFromOvrVector(hmdState.HeadPose.ThePose.Position);
}

glm::vec3 VRSystem::GetFront()
{
    auto orient = _glmFromOvrQuat(hmdState.HeadPose.ThePose.Orientation);
    return orient * glm::vec3(0, 0, -1);
}

void VRSystem::DrawAvatar(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & viewPos)
{
    m_avatar.Draw(view, proj, viewPos);
}

void VRSystem::UpdateAvatar(const float delta_time)
{
    m_avatar.Update(delta_time);
}

VRInputState VRSystem::GetInputState() const
{
    ovrInputState inputState;
    ovr_GetInputState(session, ovrControllerType_Touch, &inputState);

    std::unordered_map<Button, bool> left_buttons;
    std::unordered_map<Button, bool> right_buttons;

    if (inputState.Buttons & ovrButton_Y)
    {
        left_buttons.emplace(Button_Y, true);
    }
    if (inputState.Buttons & ovrButton_X)
    {
        left_buttons.emplace(Button_X, true);
    }

    if (inputState.Buttons & ovrButton_A)
    {
        right_buttons.emplace(Button_A, true);
    }
    if (inputState.Buttons & ovrButton_B)
    {
        right_buttons.emplace(Button_B, true);
    }

    VRControllerState left(left_buttons), right(right_buttons);

    ovrTrackingState trackState = ovr_GetTrackingState(session, 0, ovrTrue);

    auto pos_left = trackState.HandPoses[ovrHand_Left].ThePose.Position;
    left.Transform.SetPosition(_glmFromOvrVector(pos_left));
    auto rot_left = trackState.HandPoses[ovrHand_Left].ThePose.Orientation;
    left.Transform.SetRotation(_glmFromOvrQuat(rot_left));
    left.ThumbstickAxis = _glmFromOvrVector(inputState.Thumbstick[ovrHand_Left]);

    auto pos_right = trackState.HandPoses[ovrHand_Right].ThePose.Position;
    right.Transform.SetPosition(_glmFromOvrVector(pos_right));
    auto rot_right = trackState.HandPoses[ovrHand_Right].ThePose.Orientation;
    right.Transform.SetRotation(_glmFromOvrQuat(rot_right));
    right.ThumbstickAxis = _glmFromOvrVector(inputState.Thumbstick[ovrHand_Right]);

    return VRInputState(left, right);
}

glm::vec3 VRSystem::GetEyePos(int eye)
{
    return _glmFromOvrVector(layer.RenderPose[eye].Position);
}

Transform3D VRSystem::GetHMDTransform()
{
    Transform3D result;
    result.SetPosition(_glmFromOvrVector(hmdState.HeadPose.ThePose.Position));
    result.SetRotation(_glmFromOvrQuat(hmdState.HeadPose.ThePose.Orientation));
    return result;
}

Transform3D VRSystem::GetEyeTransform(int eye)
{
    Transform3D result;
    result.SetPosition(_glmFromOvrVector(layer.RenderPose[eye].Position));
    result.SetRotation(_glmFromOvrQuat(layer.RenderPose[eye].Orientation));
    return result;
}

glm::mat4 VRSystem::GetViewFromEye(int eye)
{
    ovrVector3f eyePosition = layer.RenderPose[eye].Position;
    ovrQuatf eyeOrientation = layer.RenderPose[eye].Orientation;
    glm::quat glmOrientation = _glmFromOvrQuat(eyeOrientation);
    glm::vec3 eyeWorld = _glmFromOvrVector(eyePosition);
    glm::vec3 eyeForward = glmOrientation * glm::vec3(0, 0, -1);
    glm::vec3 eyeUp = glmOrientation * glm::vec3(0, 1, 0);
    return glm::lookAt(eyeWorld, eyeWorld + eyeForward, eyeUp);
}
