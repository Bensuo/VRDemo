#include "Avatar.h"
#include "OvrConversions.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <OVR_Avatar.h>
#include <OVR_Platform.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "ShaderFactory.h"

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Avatar code based on the “Mirror” demo developed by Oculus. This class is just a thin wrapper around their implementation.
* https://developer.oculus.com/documentation/avatarsdk/latest/concepts/avatars-gsg-native-intro/#avatars-gsg-native
*/

Avatar::Avatar(ovrSession ovr)
    : m_ovr(ovr), m_avatar(nullptr)
{
    ovrAvatar_Initialize("VRDemo");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW not initialized correctly" << std::endl;

    // Start retrieving the avatar specification
    printf("Requesting avatar specification...\r\n");
    ovrID userID = ovr_GetLoggedInUserID();
    ovrAvatar_RequestAvatarSpecification(userID);

    char errorBuffer[512];
    m_skinned_mesh_program = Engine::Content::ShaderFactory::Load("res/shaders/AvatarVertexShader.glsl", "res/shaders/AvatarFragmentShader.glsl").Program();
    if (m_skinned_mesh_program < 0) {
        ovr_Destroy(ovr);
        ovr_Shutdown();         
        std::abort();
    }
    m_skinned_mesh_PBS_program = Engine::Content::ShaderFactory::Load("res/shaders/AvatarVertexShader.glsl", "res/shaders/AvatarFragmentShaderPBS.glsl").Program();
    if (m_skinned_mesh_PBS_program < 0) {
        ovr_Destroy(ovr);
        ovr_Shutdown();        
        std::abort();
    }

    ovr_SetTrackingOriginType(ovr, ovrTrackingOrigin_FloorLevel);
    ovr_RecenterTrackingOrigin(ovr);
}

void Avatar::HandleAvatarSpecification(const ovrAvatarMessage_AvatarSpecification* message)
{
    // Create the avatar instance
    m_avatar = ovrAvatar_Create(message->avatarSpec, ovrAvatarCapability_All);

    // Trigger load operations for all of the assets referenced by the avatar
    uint32_t refCount = ovrAvatar_GetReferencedAssetCount(m_avatar);
    for (uint32_t i = 0; i < refCount; ++i)
    {
        ovrAvatarAssetID id = ovrAvatar_GetReferencedAsset(m_avatar, i);
        ovrAvatarAsset_BeginLoading(id);
        ++m_loading_assets;
    }
    printf("Loading %d assets...\r\n", m_loading_assets);
}

Avatar::MeshData* Avatar::LoadMesh(const ovrAvatarMeshAssetData* data)
{
    MeshData* mesh = new MeshData();

    // Create the vertex array and buffer
    glGenVertexArrays(1, &mesh->m_vertex_array);
    glGenBuffers(1, &mesh->m_vertex_buffer);
    glGenBuffers(1, &mesh->m_element_buffer);

    // Bind the vertex buffer and assign the vertex data
    glBindVertexArray(mesh->m_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data->vertexCount * sizeof(ovrAvatarMeshVertex), data->vertexBuffer, GL_STATIC_DRAW);

    // Bind the index buffer and assign the index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indexCount * sizeof(GLushort), data->indexBuffer, GL_STATIC_DRAW);
    mesh->m_element_count = data->indexCount;

    // Fill in the array attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ovrAvatarMeshVertex), &((ovrAvatarMeshVertex*)0)->x);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ovrAvatarMeshVertex), &((ovrAvatarMeshVertex*)0)->nx);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ovrAvatarMeshVertex), &((ovrAvatarMeshVertex*)0)->tx);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(ovrAvatarMeshVertex), &((ovrAvatarMeshVertex*)0)->u);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 4, GL_BYTE, GL_FALSE, sizeof(ovrAvatarMeshVertex), &((ovrAvatarMeshVertex*)0)->blendIndices);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(ovrAvatarMeshVertex), &((ovrAvatarMeshVertex*)0)->blendWeights);
    glEnableVertexAttribArray(5);

    // Clean up
    glBindVertexArray(0);

    // Translate the bind pose
    _computeWorldPose(data->skinnedBindPose, mesh->m_bind_pose);
    for (uint32_t i = 0; i < data->skinnedBindPose.jointCount; ++i)
    {
        mesh->m_inverse_bind_pose[i] = glm::inverse(mesh->m_bind_pose[i]);
    }
    return mesh;
}

void Avatar::HandleAssetLoaded(const ovrAvatarMessage_AssetLoaded* message)
{
    // Determine the type of the asset that got loaded
    ovrAvatarAssetType assetType = ovrAvatarAsset_GetType(message->asset);
    void* data = nullptr;

    // Call the appropriate loader function
    switch (assetType)
    {
    case ovrAvatarAssetType_Mesh:
        data = LoadMesh(ovrAvatarAsset_GetMeshData(message->asset));
        break;
    case ovrAvatarAssetType_Texture:
        data = LoadTexture(ovrAvatarAsset_GetTextureData(message->asset));
        break;
    }

    // Store the data that we loaded for the asset in the asset map
    m_asset_map[message->assetID] = data;
    --m_loading_assets;
    printf("Loading %d assets...\r\n", m_loading_assets);
}

Avatar::TextureData* Avatar::LoadTexture(const ovrAvatarTextureAssetData* data)
{
    // Create a texture
    TextureData* texture = new TextureData();
    glGenTextures(1, &texture->texture_ID);
    glBindTexture(GL_TEXTURE_2D, texture->texture_ID);

    // Load the image data
    switch (data->format)
    {

        // Handle uncompressed image data
    case ovrAvatarTextureFormat_RGB24:
        for (uint32_t level = 0, offset = 0, width = data->sizeX, height = data->sizeY; level < data->mipCount; ++level)
        {
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data->textureData + offset);
            offset += width * height * 3;
            width /= 2;
            height /= 2;
        }
        break;

        // Handle compressed image data
    case ovrAvatarTextureFormat_DXT1:
    case ovrAvatarTextureFormat_DXT5:
        GLenum glFormat;
        int blockSize;
        if (data->format == ovrAvatarTextureFormat_DXT1)
        {
            blockSize = 8;
            glFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        }
        else
        {
            blockSize = 16;
            glFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        }

        for (uint32_t level = 0, offset = 0, width = data->sizeX, height = data->sizeY; level < data->mipCount; ++level)
        {
            GLsizei levelSize = (width < 4 || height < 4) ? blockSize : blockSize * (width / 4) * (height / 4);
            glCompressedTexImage2D(GL_TEXTURE_2D, level, glFormat, width, height, 0, levelSize, data->textureData + offset);
            offset += levelSize;
            width /= 2;
            height /= 2;
        }
        break;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return texture;
}

Avatar::~Avatar()
{
}

void Avatar::SetTextureSamplers(GLuint program, const char uniformName[], size_t count, const int textureUnits[], const ovrAvatarAssetID assetIDs[])
{
    for (int i = 0; i < count; ++i)
    {
        ovrAvatarAssetID assetID = assetIDs[i];

        GLuint textureID = 0;
        if (assetID)
        {
            void* data = m_asset_map[assetID];
            if (data)
            {
                TextureData* textureData = (TextureData*)data;
                textureID = textureData->texture_ID;
            }
        }
        glActiveTexture(GL_TEXTURE0 + textureUnits[i]);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    GLint uniformLocation = glGetUniformLocation(program, uniformName);
    glUniform1iv(uniformLocation, (GLsizei)count, textureUnits);
}


void Avatar::SetTextureSampler(GLuint program, int textureUnit, const char uniformName[], ovrAvatarAssetID assetID)
{
    GLuint textureID = 0;
    if (assetID)
    {
        void* data = m_asset_map[assetID];
        TextureData* textureData = (TextureData*)data;
        textureID = textureData->texture_ID;
    }
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(program, uniformName), textureUnit);
}

void Avatar::SetMeshState(
    GLuint program,
    const ovrAvatarTransform& localTransform,
    const MeshData* data,
    const ovrAvatarSkinnedMeshPose& skinnedPose,
    const glm::mat4& world,
    const glm::mat4& view,
    const glm::mat4 proj,
    const glm::vec3& viewPos
) {
    // Compute the final world and viewProjection matrices for this part
    glm::mat4 local;
    _glmFromOvrAvatarTransform(localTransform, &local);
    glm::mat4 worldMat = world * local;
    glm::mat4 viewProjMat = proj * view;

    // Compute the skinned pose
    glm::mat4* skinnedPoses = (glm::mat4*)alloca(sizeof(glm::mat4) * skinnedPose.jointCount);
    _computeWorldPose(skinnedPose, skinnedPoses);
    for (uint32_t i = 0; i < skinnedPose.jointCount; ++i)
    {
        skinnedPoses[i] = skinnedPoses[i] * data->m_inverse_bind_pose[i];
    }

    // Pass the world view position to the shader for view-dependent rendering
    glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, glm::value_ptr(viewPos));

    // Assign the vertex uniforms
    glUniformMatrix4fv(glGetUniformLocation(program, "world"), 1, 0, glm::value_ptr(worldMat));
    glUniformMatrix4fv(glGetUniformLocation(program, "viewProj"), 1, 0, glm::value_ptr(viewProjMat));
    glUniformMatrix4fv(glGetUniformLocation(program, "meshPose"), (GLsizei)skinnedPose.jointCount, 0, glm::value_ptr(*skinnedPoses));
}

void Avatar::SetMaterialState(GLuint program, const ovrAvatarMaterialState* state, glm::mat4* projectorInv)
{
    // Assign the fragment uniforms
    glUniform1i(glGetUniformLocation(program, "useAlpha"), state->alphaMaskTextureID != 0);
    glUniform1i(glGetUniformLocation(program, "useNormalMap"), state->normalMapTextureID != 0);
    glUniform1i(glGetUniformLocation(program, "useRoughnessMap"), state->roughnessMapTextureID != 0);

    glUniform1f(glGetUniformLocation(program, "elapsedSeconds"), m_elapsed_seconds);

    if (projectorInv)
    {
        glUniform1i(glGetUniformLocation(program, "useProjector"), 1);
        glUniformMatrix4fv(glGetUniformLocation(program, "projectorInv"), 1, 0, glm::value_ptr(*projectorInv));
    }
    else
    {
        glUniform1i(glGetUniformLocation(program, "useProjector"), 0);
    }

    int textureSlot = 1;
    glUniform4fv(glGetUniformLocation(program, "baseColor"), 1, &state->baseColor.x);
    glUniform1i(glGetUniformLocation(program, "baseMaskType"), state->baseMaskType);
    glUniform4fv(glGetUniformLocation(program, "baseMaskParameters"), 1, &state->baseMaskParameters.x);
    glUniform4fv(glGetUniformLocation(program, "baseMaskAxis"), 1, &state->baseMaskAxis.x);
    SetTextureSampler(program, textureSlot++, "alphaMask", state->alphaMaskTextureID);
    glUniform4fv(glGetUniformLocation(program, "alphaMaskScaleOffset"), 1, &state->alphaMaskScaleOffset.x);
    SetTextureSampler(program, textureSlot++, "normalMap", state->normalMapTextureID);
    glUniform4fv(glGetUniformLocation(program, "normalMapScaleOffset"), 1, &state->normalMapScaleOffset.x);
    SetTextureSampler(program, textureSlot++, "parallaxMap", state->parallaxMapTextureID);
    glUniform4fv(glGetUniformLocation(program, "parallaxMapScaleOffset"), 1, &state->parallaxMapScaleOffset.x);
    SetTextureSampler(program, textureSlot++, "roughnessMap", state->roughnessMapTextureID);
    glUniform4fv(glGetUniformLocation(program, "roughnessMapScaleOffset"), 1, &state->roughnessMapScaleOffset.x);

    struct LayerUniforms {
        int layerSamplerModes[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
        int layerBlendModes[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
        int layerMaskTypes[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
        ovrAvatarVector4f layerColors[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
        int layerSurfaces[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
        ovrAvatarAssetID layerSurfaceIDs[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
        ovrAvatarVector4f layerSurfaceScaleOffsets[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
        ovrAvatarVector4f layerSampleParameters[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
        ovrAvatarVector4f layerMaskParameters[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
        ovrAvatarVector4f layerMaskAxes[OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT];
    } layerUniforms;
    memset(&layerUniforms, 0, sizeof(layerUniforms));
    for (uint32_t i = 0; i < state->layerCount; ++i)
    {
        const ovrAvatarMaterialLayerState& layerState = state->layers[i];
        layerUniforms.layerSamplerModes[i] = layerState.sampleMode;
        layerUniforms.layerBlendModes[i] = layerState.blendMode;
        layerUniforms.layerMaskTypes[i] = layerState.maskType;
        layerUniforms.layerColors[i] = layerState.layerColor;
        layerUniforms.layerSurfaces[i] = textureSlot++;
        layerUniforms.layerSurfaceIDs[i] = layerState.sampleTexture;
        layerUniforms.layerSurfaceScaleOffsets[i] = layerState.sampleScaleOffset;
        layerUniforms.layerSampleParameters[i] = layerState.sampleParameters;
        layerUniforms.layerMaskParameters[i] = layerState.maskParameters;
        layerUniforms.layerMaskAxes[i] = layerState.maskAxis;
    }

    glUniform1i(glGetUniformLocation(program, "layerCount"), state->layerCount);
    glUniform1iv(glGetUniformLocation(program, "layerSamplerModes"), OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT, layerUniforms.layerSamplerModes);
    glUniform1iv(glGetUniformLocation(program, "layerBlendModes"), OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT, layerUniforms.layerBlendModes);
    glUniform1iv(glGetUniformLocation(program, "layerMaskTypes"), OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT, layerUniforms.layerMaskTypes);
    glUniform4fv(glGetUniformLocation(program, "layerColors"), OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT, (float*)layerUniforms.layerColors);
    SetTextureSamplers(program, "layerSurfaces", OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT, layerUniforms.layerSurfaces, layerUniforms.layerSurfaceIDs);
    glUniform4fv(glGetUniformLocation(program, "layerSurfaceScaleOffsets"), OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT, (float*)layerUniforms.layerSurfaceScaleOffsets);
    glUniform4fv(glGetUniformLocation(program, "layerSampleParameters"), OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT, (float*)layerUniforms.layerSampleParameters);
    glUniform4fv(glGetUniformLocation(program, "layerMaskParameters"), OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT, (float*)layerUniforms.layerMaskParameters);
    glUniform4fv(glGetUniformLocation(program, "layerMaskAxes"), OVR_AVATAR_MAX_MATERIAL_LAYER_COUNT, (float*)layerUniforms.layerMaskAxes);
}

void Avatar::RenderSkinnedMeshPart(const ovrAvatarRenderPart_SkinnedMeshRender* mesh, uint32_t visibilityMask, const glm::mat4& world, const glm::mat4& view, const glm::mat4 proj, const glm::vec3& viewPos)
{
    // If this part isn't visible from the viewpoint we're rendering from, do nothing
    if ((mesh->visibilityMask & visibilityMask) == 0)
    {
        return;
    }

    // Get the GL mesh data for this mesh's asset
    MeshData* data = (MeshData*)m_asset_map[mesh->meshAssetID];

    glUseProgram(m_skinned_mesh_program);

    // Apply the vertex state
    SetMeshState(m_skinned_mesh_program, mesh->localTransform, data, mesh->skinnedPose, world, view, proj, viewPos);

    // Apply the material state
    SetMaterialState(m_skinned_mesh_program, &mesh->materialState, nullptr);

    // Draw the mesh
    glBindVertexArray(data->m_vertex_array);
    glDepthFunc(GL_LESS);

    // Write to depth first for self-occlusion
    if (mesh->visibilityMask & ovrAvatarVisibilityFlag_SelfOccluding)
    {
        glDepthMask(GL_TRUE);
        glColorMaski(0, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDrawElements(GL_TRIANGLES, (GLsizei)data->m_element_count, GL_UNSIGNED_SHORT, 0);
        glDepthFunc(GL_EQUAL);
    }

    // Render to color buffer
    glDepthMask(GL_FALSE);
    glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDrawElements(GL_TRIANGLES, (GLsizei)data->m_element_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

void Avatar::SetPBSState(GLuint program, const ovrAvatarAssetID albedoTextureID, const ovrAvatarAssetID surfaceTextureID)
{
    int textureSlot = 0;
    SetTextureSampler(program, textureSlot++, "albedo", albedoTextureID);
    SetTextureSampler(program, textureSlot++, "surface", surfaceTextureID);
}

void Avatar::RenderSkinnedMeshPartPBS(const ovrAvatarRenderPart_SkinnedMeshRenderPBS* mesh, uint32_t visibilityMask, const glm::mat4& world, const glm::mat4& view, const glm::mat4 proj, const glm::vec3& viewPos)
{
    // If this part isn't visible from the viewpoint we're rendering from, do nothing
    if ((mesh->visibilityMask & visibilityMask) == 0)
    {
        return;
    }

    // Get the GL mesh data for this mesh's asset
    MeshData* data = (MeshData*)m_asset_map[mesh->meshAssetID];

    glUseProgram(m_skinned_mesh_PBS_program);

    // Apply the vertex state
    SetMeshState(m_skinned_mesh_PBS_program, mesh->localTransform, data, mesh->skinnedPose, world, view, proj, viewPos);

    // Apply the material state
    SetPBSState(m_skinned_mesh_PBS_program, mesh->albedoTextureAssetID, mesh->surfaceTextureAssetID);

    // Draw the mesh
    glBindVertexArray(data->m_vertex_array);
    glDepthFunc(GL_LESS);

    // Write to depth first for self-occlusion
    if (mesh->visibilityMask & ovrAvatarVisibilityFlag_SelfOccluding)
    {
        glDepthMask(GL_TRUE);
        glColorMaski(0, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDrawElements(GL_TRIANGLES, (GLsizei)data->m_element_count, GL_UNSIGNED_SHORT, 0);
        glDepthFunc(GL_EQUAL);
    }
    glDepthMask(GL_FALSE);

    // Draw the mesh
    glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDrawElements(GL_TRIANGLES, (GLsizei)data->m_element_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

void Avatar::RenderProjector(const ovrAvatarRenderPart_ProjectorRender* projector, ovrAvatar* avatar, uint32_t visibilityMask, const glm::mat4& world, const glm::mat4& view, const glm::mat4 proj, const glm::vec3& viewPos)
{

    // Compute the mesh transform
    const ovrAvatarComponent* component = ovrAvatarComponent_Get(avatar, projector->componentIndex);
    const ovrAvatarRenderPart* renderPart = component->renderParts[projector->renderPartIndex];
    const ovrAvatarRenderPart_SkinnedMeshRender* mesh = ovrAvatarRenderPart_GetSkinnedMeshRender(renderPart);

    // If this part isn't visible from the viewpoint we're rendering from, do nothing
    if ((mesh->visibilityMask & visibilityMask) == 0)
    {
        return;
    }

    // Compute the projection matrix
    glm::mat4 projection;
    _glmFromOvrAvatarTransform(projector->localTransform, &projection);
    glm::mat4 worldProjection = world * projection;
    glm::mat4 projectionInv = glm::inverse(worldProjection);

    // Compute the mesh transform
    glm::mat4 meshWorld;
    _glmFromOvrAvatarTransform(component->transform, &meshWorld);

    // Get the GL mesh data for this mesh's asset
    MeshData* data = (MeshData*)m_asset_map[mesh->meshAssetID];

    glUseProgram(m_skinned_mesh_program);

    // Apply the vertex state
    SetMeshState(m_skinned_mesh_program, mesh->localTransform, data, mesh->skinnedPose, meshWorld, view, proj, viewPos);

    // Apply the material state
    SetMaterialState(m_skinned_mesh_program, &projector->materialState, &projectionInv);

    // Draw the mesh
    glBindVertexArray(data->m_vertex_array);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_EQUAL);
    glDrawElements(GL_TRIANGLES, (GLsizei)data->m_element_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

void Avatar::Draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& viewPos)
{
    // If we have the avatar and have finished loading assets, render it
    if (m_avatar && !m_loading_assets)
    {
        // Traverse over all components on the avatar
        uint32_t componentCount = ovrAvatarComponent_Count(m_avatar);
        for (uint32_t i = 0; i < componentCount; ++i)
        {
            const ovrAvatarComponent* component = ovrAvatarComponent_Get(m_avatar, i);

            // Compute the transform for this componentfframeIndex
            glm::mat4 world;
            _glmFromOvrAvatarTransform(component->transform, &world);

            // Render each rebder part attached to the component
            for (uint32_t j = 0; j < component->renderPartCount; ++j)
            {
                const ovrAvatarRenderPart* renderPart = component->renderParts[j];
                ovrAvatarRenderPartType type = ovrAvatarRenderPart_GetType(renderPart);
                switch (type)
                {
                case ovrAvatarRenderPartType_SkinnedMeshRender:
                    RenderSkinnedMeshPart(ovrAvatarRenderPart_GetSkinnedMeshRender(renderPart), ovrAvatarVisibilityFlag_FirstPerson, world, view, proj, viewPos);
                    break;
                case ovrAvatarRenderPartType_SkinnedMeshRenderPBS:
                    RenderSkinnedMeshPartPBS(ovrAvatarRenderPart_GetSkinnedMeshRenderPBS(renderPart), ovrAvatarVisibilityFlag_FirstPerson, world, view, proj, viewPos);
                    break;
                case ovrAvatarRenderPartType_ProjectorRender:
                    RenderProjector(ovrAvatarRenderPart_GetProjectorRender(renderPart), m_avatar, ovrAvatarVisibilityFlag_FirstPerson, world, view, proj, viewPos);
                    break;
                }
            }
        }
    }
    ++m_frame_index;
}

void Avatar::Update(const float delta_time)
{
    m_elapsed_seconds += m_elapsed_seconds;

    // Pump avatar messages
    while (ovrAvatarMessage* message = ovrAvatarMessage_Pop())
    {
        switch (ovrAvatarMessage_GetType(message))
        {
        case ovrAvatarMessageType_AvatarSpecification:
            HandleAvatarSpecification(ovrAvatarMessage_GetAvatarSpecification(message));
            break;
        case ovrAvatarMessageType_AssetLoaded:
            HandleAssetLoaded(ovrAvatarMessage_GetAssetLoaded(message));
            break;
        }
        ovrAvatarMessage_Free(message);
    }

    if (m_avatar)
    {
        // Convert the OVR inputs into Avatar SDK inputs
        ovrInputState touchState;
        ovr_GetInputState(m_ovr, ovrControllerType_Active, &touchState);
        double seconds = ovr_GetPredictedDisplayTime(m_ovr, 0);
        ovrTrackingState trackingState = ovr_GetTrackingState(m_ovr, seconds, false);

        glm::vec3 hmdP = _glmFromOvrVector(trackingState.HeadPose.ThePose.Position);
        glm::quat hmdQ = _glmFromOvrQuat(trackingState.HeadPose.ThePose.Orientation);
        glm::vec3 leftP = _glmFromOvrVector(trackingState.HandPoses[ovrHand_Left].ThePose.Position);
        glm::quat leftQ = _glmFromOvrQuat(trackingState.HandPoses[ovrHand_Left].ThePose.Orientation);
        glm::vec3 rightP = _glmFromOvrVector(trackingState.HandPoses[ovrHand_Right].ThePose.Position);
        glm::quat rightQ = _glmFromOvrQuat(trackingState.HandPoses[ovrHand_Right].ThePose.Orientation);

        ovrAvatarTransform hmd;
        _ovrAvatarTransformFromGlm(hmdP, hmdQ, glm::vec3(1.0f), &hmd);

        ovrAvatarTransform left;
        _ovrAvatarTransformFromGlm(leftP, leftQ, glm::vec3(1.0f), &left);

        ovrAvatarTransform right;
        _ovrAvatarTransformFromGlm(rightP, rightQ, glm::vec3(1.0f), &right);

        ovrAvatarHandInputState inputStateLeft;
        _ovrAvatarHandInputStateFromOvr(left, touchState, ovrHand_Left, &inputStateLeft);

        ovrAvatarHandInputState inputStateRight;
        _ovrAvatarHandInputStateFromOvr(right, touchState, ovrHand_Right, &inputStateRight);

        ovrAvatarPose_UpdateBody(m_avatar, hmd);
        ovrAvatarPose_UpdateHands(m_avatar, inputStateLeft, inputStateRight);
        ovrAvatarPose_Finalize(m_avatar, delta_time);
    }
}