#include "VRSystem.h"
#include <iostream>
#include <Extras/OVR_Math.h>
#include <glm/gtc/type_ptr.inl>

VRSystem::VRSystem()
{
}

VRSystem::~VRSystem()
{
}

void VRSystem::Init()
{
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
	ovrTrackingState hmdState = ovr_GetTrackingState(session, seconds, ovrTrue);
	ovr_CalcEyePoses(hmdState.HeadPose.ThePose, HmdToEyePose, layer.RenderPose);
	//ovr_GetEyePoses(m_vr_session, frameIndex, ovrTrue, HmdToEyePose, EyeRenderPose, &sensorSampleTime);

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
	OVR::Matrix4f proj = ovrMatrix4f_Projection(hmd_desc.DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_None);
	return glm::transpose(glm::make_mat4(&proj.M[0][0]));
}

glm::mat4 VRSystem::GetViewFromEye(glm::vec3 eyePos, int eye, glm::vec3& front, float rotationY)
{
	OVR::Matrix4f rollPitchYaw = OVR::Matrix4f::RotationY(rotationY);
	OVR::Matrix4f finalRollPitchYaw = rollPitchYaw * OVR::Matrix4f(layer.RenderPose[eye].Orientation);
	OVR::Vector3f finalUp = finalRollPitchYaw.Transform(OVR::Vector3f(0, 1, 0));
	OVR::Vector3f finalForward = finalRollPitchYaw.Transform(OVR::Vector3f(0, 0, -1));
	front = glm::vec3(finalForward.x, 0, finalForward.z);
	OVR::Vector3f shiftedEyePos = OVR::Vector3f(eyePos.x, eyePos.y, eyePos.z) + rollPitchYaw.Transform(layer.RenderPose[eye].Position);

	OVR::Matrix4f view = OVR::Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);

	return glm::transpose(glm::make_mat4(&view.M[0][0]));
}

glm::vec3 VRSystem::GetEyeOffset(int eye)
{
	auto pos = layer.RenderPose[eye].Position;
	return glm::vec3(pos.x, pos.y, pos.z);
}
