//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// QueryVk.cpp:
//    Implements the class methods for QueryVk.
//

#include "libANGLE/renderer/vulkan/QueryVk.h"
#include "libANGLE/Context.h"
#include "libANGLE/TransformFeedback.h"
#include "libANGLE/renderer/vulkan/ContextVk.h"
#include "libANGLE/renderer/vulkan/RendererVk.h"
#include "libANGLE/renderer/vulkan/TransformFeedbackVk.h"

#include "common/debug.h"

namespace rx
{

QueryVk::QueryVk(gl::QueryType type)
    : QueryImpl(type),
      mTransformFeedbackPrimitivesDrawn(0),
      mCachedResult(0),
      mCachedResultValid(false)
{}

QueryVk::~QueryVk() = default;

void QueryVk::onDestroy(const gl::Context *context)
{
    ContextVk *contextVk = vk::GetImpl(context);
    if (getType() != gl::QueryType::TransformFeedbackPrimitivesWritten)
    {
        vk::DynamicQueryPool *queryPool = contextVk->getQueryPool(getType());
        queryPool->freeQuery(contextVk, &mQueryHelper);
        queryPool->freeQuery(contextVk, &mQueryHelperTimeElapsedBegin);
    }
}

angle::Result QueryVk::stashQueryHelper(ContextVk *contextVk)
{
    ASSERT(isRenderPassQuery(contextVk));
    mStashedQueryHelpers.emplace_back(std::move(mQueryHelper));
    mQueryHelper.deinit();
    ANGLE_TRY(contextVk->getQueryPool(getType())->allocateQuery(contextVk, &mQueryHelper));
    return angle::Result::Continue;
}

angle::Result QueryVk::accumulateStashedQueryResult(ContextVk *contextVk, vk::QueryResult *result)
{
    for (vk::QueryHelper &query : mStashedQueryHelpers)
    {
        vk::QueryResult v(getQueryResultCount());
        ANGLE_TRY(query.getUint64Result(contextVk, &v));
        *result += v;
    }
    mStashedQueryHelpers.clear();
    return angle::Result::Continue;
}

angle::Result QueryVk::begin(const gl::Context *context)
{
    ContextVk *contextVk = vk::GetImpl(context);

    mCachedResultValid = false;

    if (isTransformFeedbackQuery())
    {
        mTransformFeedbackPrimitivesDrawn = 0;

        // Transform feedback query is a handled by a CPU-calculated value when emulated.
        if (contextVk->getFeatures().emulateTransformFeedback.enabled)
        {
            ASSERT(!contextVk->getFeatures().supportsTransformFeedbackExtension.enabled);
            return angle::Result::Continue;
        }
    }

    if (!mQueryHelper.valid())
    {
        ANGLE_TRY(contextVk->getQueryPool(getType())->allocateQuery(contextVk, &mQueryHelper));
    }

    if (isRenderPassQuery(contextVk))
    {
        // For pathological usage case where begin/end is called back to back without flush and get
        // result, we have to force flush so that the same mQueryHelper will not encoded in the same
        // renderpass twice without resetting it.
        if (mQueryHelper.usedInRecordedCommands())
        {
            ANGLE_TRY(contextVk->flushImpl(nullptr));
            // As soon as beginQuery is called, previous query's result will not retrievable by API.
            // We must clear it so that it will not count against current beginQuery call.
            mStashedQueryHelpers.clear();
            mQueryHelper.deinit();
            ANGLE_TRY(contextVk->getQueryPool(getType())->allocateQuery(contextVk, &mQueryHelper));
        }
        contextVk->beginRenderPassQuery(this);
    }
    else if (getType() == gl::QueryType::TimeElapsed)
    {
        // Note: TimeElapsed is implemented by using two Timestamp queries and taking the diff.
        if (!mQueryHelperTimeElapsedBegin.valid())
        {
            ANGLE_TRY(contextVk->getQueryPool(getType())->allocateQuery(
                contextVk, &mQueryHelperTimeElapsedBegin));
        }

        ANGLE_TRY(mQueryHelperTimeElapsedBegin.flushAndWriteTimestamp(contextVk));
    }
    else
    {
        ANGLE_TRY(mQueryHelper.beginQuery(contextVk));
    }

    return angle::Result::Continue;
}

angle::Result QueryVk::end(const gl::Context *context)
{
    ContextVk *contextVk = vk::GetImpl(context);

    if (isRenderPassQuery(contextVk))
    {
        contextVk->endRenderPassQuery(this);
    }
    else if (isTransformFeedbackQuery())
    {
        // Transform feedback query is a handled by a CPU-calculated value when emulated.
        ASSERT(contextVk->getFeatures().emulateTransformFeedback.enabled);
        mCachedResult = mTransformFeedbackPrimitivesDrawn;

        // There could be transform feedback in progress, so add the primitives drawn so far
        // from the current transform feedback object.
        gl::TransformFeedback *transformFeedback =
            context->getState().getCurrentTransformFeedback();
        if (transformFeedback)
        {
            mCachedResult += transformFeedback->getPrimitivesDrawn();
        }
        mCachedResultValid = true;
    }
    else if (getType() == gl::QueryType::TimeElapsed)
    {
        ANGLE_TRY(mQueryHelper.flushAndWriteTimestamp(contextVk));
    }
    else
    {
        ANGLE_TRY(mQueryHelper.endQuery(contextVk));
    }

    return angle::Result::Continue;
}

angle::Result QueryVk::queryCounter(const gl::Context *context)
{
    ASSERT(getType() == gl::QueryType::Timestamp);
    ContextVk *contextVk = vk::GetImpl(context);

    mCachedResultValid = false;

    if (!mQueryHelper.valid())
    {
        ANGLE_TRY(contextVk->getQueryPool(getType())->allocateQuery(contextVk, &mQueryHelper));
    }

    return mQueryHelper.flushAndWriteTimestamp(contextVk);
}

bool QueryVk::isUsedInRecordedCommands() const
{
    if (mQueryHelper.usedInRecordedCommands())
    {
        return true;
    }

    for (const vk::QueryHelper &query : mStashedQueryHelpers)
    {
        if (query.usedInRecordedCommands())
        {
            return true;
        }
    }

    return false;
}

bool QueryVk::isCurrentlyInUse(Serial lastCompletedSerial) const
{
    if (mQueryHelper.isCurrentlyInUse(lastCompletedSerial))
    {
        return true;
    }

    for (const vk::QueryHelper &query : mStashedQueryHelpers)
    {
        if (query.isCurrentlyInUse(lastCompletedSerial))
        {
            return true;
        }
    }

    return false;
}

angle::Result QueryVk::finishRunningCommands(ContextVk *contextVk)
{
    Serial lastCompletedSerial = contextVk->getLastCompletedQueueSerial();

    if (mQueryHelper.usedInRunningCommands(lastCompletedSerial))
    {
        ANGLE_TRY(mQueryHelper.finishRunningCommands(contextVk));
        lastCompletedSerial = contextVk->getLastCompletedQueueSerial();
    }

    for (vk::QueryHelper &query : mStashedQueryHelpers)
    {
        if (query.usedInRunningCommands(lastCompletedSerial))
        {
            ANGLE_TRY(query.finishRunningCommands(contextVk));
            lastCompletedSerial = contextVk->getLastCompletedQueueSerial();
        }
    }
    return angle::Result::Continue;
}

angle::Result QueryVk::getResult(const gl::Context *context, bool wait)
{
    ANGLE_TRACE_EVENT0("gpu.angle", "QueryVk::getResult");

    if (mCachedResultValid)
    {
        return angle::Result::Continue;
    }

    ContextVk *contextVk = vk::GetImpl(context);
    RendererVk *renderer = contextVk->getRenderer();

    // glGetQueryObject* requires an implicit flush of the command buffers to guarantee execution in
    // finite time.
    // Note regarding time-elapsed: end should have been called after begin, so flushing when end
    // has pending work should flush begin too.

    if (isUsedInRecordedCommands())
    {
        ANGLE_TRY(contextVk->flushImpl(nullptr));

        ASSERT(!mQueryHelperTimeElapsedBegin.usedInRecordedCommands());
        ASSERT(!mQueryHelper.usedInRecordedCommands());
    }

    ANGLE_TRY(contextVk->checkCompletedCommands());

    // If the command buffer this query is being written to is still in flight, its reset
    // command may not have been performed by the GPU yet.  To avoid a race condition in this
    // case, wait for the batch to finish first before querying (or return not-ready if not
    // waiting).
    if (isCurrentlyInUse(contextVk->getLastCompletedQueueSerial()))
    {
        if (!wait)
        {
            return angle::Result::Continue;
        }
        ANGLE_PERF_WARNING(contextVk->getDebug(), GL_DEBUG_SEVERITY_HIGH,
                           "GPU stall due to waiting on uncompleted query");

        // Assert that the work has been sent to the GPU
        ASSERT(!isUsedInRecordedCommands());
        ANGLE_TRY(finishRunningCommands(contextVk));
    }

    vk::QueryResult result(getQueryResultCount());

    if (wait)
    {
        ANGLE_TRY(mQueryHelper.getUint64Result(contextVk, &result));
        ANGLE_TRY(accumulateStashedQueryResult(contextVk, &result));
    }
    else
    {
        bool available = false;
        ANGLE_TRY(mQueryHelper.getUint64ResultNonBlocking(contextVk, &result, &available));
        if (!available)
        {
            // If the results are not ready, do nothing.  mCachedResultValid remains false.
            return angle::Result::Continue;
        }
        ANGLE_TRY(accumulateStashedQueryResult(contextVk, &result));
    }

    double timestampPeriod = renderer->getPhysicalDeviceProperties().limits.timestampPeriod;

    // Fix up the results to what OpenGL expects.
    switch (getType())
    {
        case gl::QueryType::AnySamples:
        case gl::QueryType::AnySamplesConservative:
            // OpenGL query result in these cases is binary
            mCachedResult = !!result.getResult();
            break;
        case gl::QueryType::Timestamp:
            mCachedResult = static_cast<uint64_t>(result.getResult() * timestampPeriod);
            break;
        case gl::QueryType::TimeElapsed:
        {
            vk::QueryResult timeElapsedBegin(1);

            // Since the result of the end query of time-elapsed is already available, the
            // result of begin query must be available too.
            ANGLE_TRY(mQueryHelperTimeElapsedBegin.getUint64Result(contextVk, &timeElapsedBegin));

            uint64_t delta = result.getResult() - timeElapsedBegin.getResult();
            mCachedResult  = static_cast<uint64_t>(delta * timestampPeriod);
            break;
        }
        case gl::QueryType::TransformFeedbackPrimitivesWritten:
            mCachedResult = result.getResult();
            break;
        default:
            UNREACHABLE();
            break;
    }

    mCachedResultValid = true;
    return angle::Result::Continue;
}
angle::Result QueryVk::getResult(const gl::Context *context, GLint *params)
{
    ANGLE_TRY(getResult(context, true));
    *params = static_cast<GLint>(mCachedResult);
    return angle::Result::Continue;
}

angle::Result QueryVk::getResult(const gl::Context *context, GLuint *params)
{
    ANGLE_TRY(getResult(context, true));
    *params = static_cast<GLuint>(mCachedResult);
    return angle::Result::Continue;
}

angle::Result QueryVk::getResult(const gl::Context *context, GLint64 *params)
{
    ANGLE_TRY(getResult(context, true));
    *params = static_cast<GLint64>(mCachedResult);
    return angle::Result::Continue;
}

angle::Result QueryVk::getResult(const gl::Context *context, GLuint64 *params)
{
    ANGLE_TRY(getResult(context, true));
    *params = mCachedResult;
    return angle::Result::Continue;
}

angle::Result QueryVk::isResultAvailable(const gl::Context *context, bool *available)
{
    ANGLE_TRY(getResult(context, false));
    *available = mCachedResultValid;

    return angle::Result::Continue;
}

void QueryVk::onTransformFeedbackEnd(GLsizeiptr primitivesDrawn)
{
    mTransformFeedbackPrimitivesDrawn += primitivesDrawn;
}

bool QueryVk::isRenderPassQuery(ContextVk *contextVk) const
{
    return isOcclusionQuery() ||
           (isTransformFeedbackQuery() &&
            contextVk->getFeatures().supportsTransformFeedbackExtension.enabled);
}

uint32_t QueryVk::getQueryResultCount() const
{
    return isTransformFeedbackQuery() ? 2 : 1;
}
}  // namespace rx
