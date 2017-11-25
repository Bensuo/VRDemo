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
        GLuint vertexArray;
        GLuint vertexBuffer;
        GLuint elementBuffer;
        GLuint elementCount;
        glm::mat4 bindPose[OVR_AVATAR_MAXIMUM_JOINT_COUNT];
        glm::mat4 inverseBindPose[OVR_AVATAR_MAXIMUM_JOINT_COUNT];
    };

    struct TextureData
    {
        GLuint textureID;
    };

    enum {
        VERTEX,
        FRAGMENT,
        SHADER_COUNT
    };

    GLuint _skinnedMeshProgram;
    GLuint _skinnedMeshPBSProgram;
    ovrAvatar* _avatar;
    int _loadingAssets = 0;   
    int frameIndex = 0;
    ovrSession ovr;
    std::map<ovrAvatarAssetID, void*> _assetMap;
    float _elapsedSeconds;

    void _renderSkinnedMeshPart(const ovrAvatarRenderPart_SkinnedMeshRender* mesh, uint32_t visibilityMask, const glm::mat4& world, const glm::mat4& view, const glm::mat4 proj, const glm::vec3& viewPos);
    void _setPBSState(GLuint program, const ovrAvatarAssetID albedoTextureID, const ovrAvatarAssetID surfaceTextureID);
    void _renderSkinnedMeshPartPBS(const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh, uint32_t visibilityMask, const glm::mat4& world, const glm::mat4& view, const glm::mat4 proj, const glm::vec3& viewPos);
    void _renderProjector(const ovrAvatarRenderPart_ProjectorRender* projector, ovrAvatar* avatar, uint32_t visibilityMask, const glm::mat4& world, const glm::mat4& view, const glm::mat4 proj, const glm::vec3& viewPos);
    MeshData* _loadMesh(const ovrAvatarMeshAssetData* data);
public:
    Avatar() {}
    Avatar(ovrSession ovr);
    void _handleAvatarSpecification(const ovrAvatarMessage_AvatarSpecification * message);
    void _handleAssetLoaded(const ovrAvatarMessage_AssetLoaded * message);
    Avatar::TextureData * _loadTexture(const ovrAvatarTextureAssetData * data);
    ~Avatar();
    void _setTextureSamplers(GLuint program, const char uniformName[], size_t count, const int textureUnits[], const ovrAvatarAssetID assetIDs[]);
    void _setTextureSampler(GLuint program, int textureUnit, const char uniformName[], ovrAvatarAssetID assetID);
    void _setMeshState(GLuint program, const ovrAvatarTransform & localTransform, const MeshData* data, const ovrAvatarSkinnedMeshPose & skinnedPose, const glm::mat4 & world, const glm::mat4 & view, const glm::mat4 proj, const glm::vec3 & viewPos);
    void _setMaterialState(GLuint program, const ovrAvatarMaterialState * state, glm::mat4 * projectorInv);
    void Draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& viewPos);
    void Update(const float delta_time);
};