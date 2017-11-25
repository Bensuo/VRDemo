#pragma once

#include <OVR_Avatar.h>

#include <GL/glew.h>

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include "GameTimer.h"
#include <map>

class Avatar
{
    struct MeshData
    {
        GLuint m_vertex_array;
        GLuint m_vertex_buffer;
        GLuint m_element_buffer;
        GLuint m_element_count;
        glm::mat4 m_bind_pose[OVR_AVATAR_MAXIMUM_JOINT_COUNT];
        glm::mat4 m_inverse_bind_pose[OVR_AVATAR_MAXIMUM_JOINT_COUNT];
    };

    struct TextureData
    {
        GLuint texture_ID;
    };

    enum {
        VERTEX,
        FRAGMENT,
        SHADER_COUNT
    };

    GLuint m_skinned_mesh_program;
    GLuint m_skinned_mesh_PBS_program;
    ovrAvatar* m_avatar;
    int m_loading_assets = 0;   
    int m_frame_index = 0;
    ovrSession m_ovr;
    std::map<ovrAvatarAssetID, void*> m_asset_map;
    float m_elapsed_seconds;

    void RenderSkinnedMeshPart(const ovrAvatarRenderPart_SkinnedMeshRender* mesh, uint32_t visibilityMask, const glm::mat4& world, const glm::mat4& view, const glm::mat4 proj, const glm::vec3& viewPos);
    void SetPBSState(GLuint program, const ovrAvatarAssetID albedoTextureID, const ovrAvatarAssetID surfaceTextureID);
    void RenderSkinnedMeshPartPBS(const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh, uint32_t visibilityMask, const glm::mat4& world, const glm::mat4& view, const glm::mat4 proj, const glm::vec3& viewPos);
    void RenderProjector(const ovrAvatarRenderPart_ProjectorRender* projector, ovrAvatar* avatar, uint32_t visibilityMask, const glm::mat4& world, const glm::mat4& view, const glm::mat4 proj, const glm::vec3& viewPos);
    MeshData* LoadMesh(const ovrAvatarMeshAssetData* data);
public:
    Avatar() {}
    Avatar(ovrSession ovr);
    void HandleAvatarSpecification(const ovrAvatarMessage_AvatarSpecification * message);
    void HandleAssetLoaded(const ovrAvatarMessage_AssetLoaded * message);
    Avatar::TextureData * LoadTexture(const ovrAvatarTextureAssetData * data);
    ~Avatar();
    void SetTextureSamplers(GLuint program, const char uniformName[], size_t count, const int textureUnits[], const ovrAvatarAssetID assetIDs[]);
    void SetTextureSampler(GLuint program, int textureUnit, const char uniformName[], ovrAvatarAssetID assetID);
    void SetMeshState(GLuint program, const ovrAvatarTransform & localTransform, const MeshData* data, const ovrAvatarSkinnedMeshPose & skinnedPose, const glm::mat4 & world, const glm::mat4 & view, const glm::mat4 proj, const glm::vec3 & viewPos);
    void SetMaterialState(GLuint program, const ovrAvatarMaterialState * state, glm::mat4 * projectorInv);
    void Draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& viewPos);
    void Update(const float delta_time);
};