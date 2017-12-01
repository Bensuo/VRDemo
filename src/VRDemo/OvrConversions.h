/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*/

#pragma once

#include <OVR_Avatar.h>

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

static glm::vec3 _glmFromOvrVector(const ovrVector3f& ovrVector)
{
    return glm::vec3(ovrVector.x, ovrVector.y, ovrVector.z);
}

static glm::vec2 _glmFromOvrVector(const ovrVector2f& ovrVector)
{
    return glm::vec2(ovrVector.x, ovrVector.y);
}


static glm::quat _glmFromOvrQuat(const ovrQuatf& ovrQuat)
{
    return glm::quat(ovrQuat.w, ovrQuat.x, ovrQuat.y, ovrQuat.z);
}

static void _glmFromOvrAvatarTransform(const ovrAvatarTransform& transform, glm::mat4* target) {
    glm::vec3 position(transform.position.x, transform.position.y, transform.position.z);
    glm::quat orientation(transform.orientation.w, transform.orientation.x, transform.orientation.y, transform.orientation.z);
    glm::vec3 scale(transform.scale.x, transform.scale.y, transform.scale.z);
    *target = glm::translate(position) * glm::mat4_cast(orientation) * glm::scale(scale);
}

static void _ovrAvatarTransformFromGlm(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, ovrAvatarTransform* target) {
    target->position.x = position.x;
    target->position.y = position.y;
    target->position.z = position.z;
    target->orientation.x = orientation.x;
    target->orientation.y = orientation.y;
    target->orientation.z = orientation.z;
    target->orientation.w = orientation.w;
    target->scale.x = scale.x;
    target->scale.y = scale.y;
    target->scale.z = scale.z;
}

static void _ovrAvatarTransformFromGlm(const glm::mat4& matrix, ovrAvatarTransform* target) {
    glm::vec3 scale;
    glm::quat orientation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, scale, orientation, translation, skew, perspective);
    _ovrAvatarTransformFromGlm(translation, orientation, scale, target);
}

static void _ovrAvatarHandInputStateFromOvr(const ovrAvatarTransform& transform, const ovrInputState& inputState, ovrHandType hand, ovrAvatarHandInputState* state)
{
    state->transform = transform;
    state->buttonMask = 0;
    state->touchMask = 0;
    state->joystickX = inputState.Thumbstick[hand].x;
    state->joystickY = inputState.Thumbstick[hand].y;
    state->indexTrigger = inputState.IndexTrigger[hand];
    state->handTrigger = inputState.HandTrigger[hand];
    state->isActive = false;
    if (hand == ovrHand_Left)
    {
        if (inputState.Buttons & ovrButton_X) state->buttonMask |= ovrAvatarButton_One;
        if (inputState.Buttons & ovrButton_Y) state->buttonMask |= ovrAvatarButton_Two;
        if (inputState.Buttons & ovrButton_Enter) state->buttonMask |= ovrAvatarButton_Three;
        if (inputState.Buttons & ovrButton_LThumb) state->buttonMask |= ovrAvatarButton_Joystick;
        if (inputState.Touches & ovrTouch_X) state->touchMask |= ovrAvatarTouch_One;
        if (inputState.Touches & ovrTouch_Y) state->touchMask |= ovrAvatarTouch_Two;
        if (inputState.Touches & ovrTouch_LThumb) state->touchMask |= ovrAvatarTouch_Joystick;
        if (inputState.Touches & ovrTouch_LThumbRest) state->touchMask |= ovrAvatarTouch_ThumbRest;
        if (inputState.Touches & ovrTouch_LIndexTrigger) state->touchMask |= ovrAvatarTouch_Index;
        if (inputState.Touches & ovrTouch_LIndexPointing) state->touchMask |= ovrAvatarTouch_Pointing;
        if (inputState.Touches & ovrTouch_LThumbUp) state->touchMask |= ovrAvatarTouch_ThumbUp;
        state->isActive = (inputState.ControllerType & ovrControllerType_LTouch) != 0;
    }
    else if (hand == ovrHand_Right)
    {
        if (inputState.Buttons & ovrButton_A) state->buttonMask |= ovrAvatarButton_One;
        if (inputState.Buttons & ovrButton_B) state->buttonMask |= ovrAvatarButton_Two;
        if (inputState.Buttons & ovrButton_Home) state->buttonMask |= ovrAvatarButton_Three;
        if (inputState.Buttons & ovrButton_RThumb) state->buttonMask |= ovrAvatarButton_Joystick;
        if (inputState.Touches & ovrTouch_A) state->touchMask |= ovrAvatarTouch_One;
        if (inputState.Touches & ovrTouch_B) state->touchMask |= ovrAvatarTouch_Two;
        if (inputState.Touches & ovrTouch_RThumb) state->touchMask |= ovrAvatarTouch_Joystick;
        if (inputState.Touches & ovrTouch_RThumbRest) state->touchMask |= ovrAvatarTouch_ThumbRest;
        if (inputState.Touches & ovrTouch_RIndexTrigger) state->touchMask |= ovrAvatarTouch_Index;
        if (inputState.Touches & ovrTouch_RIndexPointing) state->touchMask |= ovrAvatarTouch_Pointing;
        if (inputState.Touches & ovrTouch_RThumbUp) state->touchMask |= ovrAvatarTouch_ThumbUp;
        state->isActive = (inputState.ControllerType & ovrControllerType_RTouch) != 0;
    }
}

static void _computeWorldPose(const ovrAvatarSkinnedMeshPose& localPose, glm::mat4* worldPose)
{
    for (uint32_t i = 0; i < localPose.jointCount; ++i)
    {
        glm::mat4 local;
        _glmFromOvrAvatarTransform(localPose.jointTransform[i], &local);

        int parentIndex = localPose.jointParents[i];
        if (parentIndex < 0)
        {
            worldPose[i] = local;
        }
        else
        {
            worldPose[i] = worldPose[parentIndex] * local;
        }
    }
}

static glm::mat4 _computeReflectionMatrix(const glm::vec4& plane)
{
    return glm::mat4(
        1.0f - 2.0f * plane.x * plane.x,
        -2.0f * plane.x * plane.y,
        -2.0f * plane.x * plane.z,
        -2.0f * plane.w * plane.x,

        -2.0f * plane.y * plane.x,
        1.0f - 2.0f * plane.y * plane.y,
        -2.0f * plane.y * plane.z,
        -2.0f * plane.w * plane.y,

        -2.0f * plane.z * plane.x,
        -2.0f * plane.z * plane.y,
        1.0f - 2.0f * plane.z * plane.z,
        -2.0f * plane.w * plane.z,

        0.0f,
        0.0f,
        0.0f,
        1.0f
    );
}