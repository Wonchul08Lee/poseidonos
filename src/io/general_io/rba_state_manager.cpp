/*
 *   BSD LICENSE
 *   Copyright (c) 2021 Samsung Electronics Corporation
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "src/io/general_io/rba_state_manager.h"

#include <utility>

#include "src/include/ibof_event_id.hpp"
#include "src/include/memory.h"
#include "src/sys_event/volume_event_publisher.h"

namespace ibofos
{
RBAStateManager::RBAStateManager(void)
: VolumeEvent("RBAStateManager")
{
}

RBAStateManager::~RBAStateManager()
{
    for (auto& vol : rbaStatesInArray)
    {
        vol.SetSize(0);
    }
}

void
RBAStateManager::CreateRBAState(uint32_t volumeID, uint64_t totalRBACount)
{
    RBAStatesInVolume& targetVolume = rbaStatesInArray[volumeID];
    targetVolume.SetSize(totalRBACount);
}

void
RBAStateManager::DeleteRBAState(uint32_t volumeID)
{
    RBAStatesInVolume& targetVolume = rbaStatesInArray[volumeID];
    targetVolume.SetSize(0);
}

bool
RBAStateManager::BulkAcquireOwnership(uint32_t volumeID,
    BlkAddr startRba,
    uint32_t count)
{
    if (unlikely(volumeID >= MAX_VOLUME_COUNT))
    {
        std::pair<IBOF_EVENT_ID, EventLevel> eventIdWithLevel(
            IBOF_EVENT_ID::RBAMGR_WRONG_VOLUME_ID, EventLevel::ERROR);
        throw eventIdWithLevel;
    }

    RBAStatesInVolume& targetVolume = rbaStatesInArray[volumeID];
    bool acquired = targetVolume.AcquireOwnership(startRba, count);

    return acquired;
}

void
RBAStateManager::BulkReleaseOwnership(uint32_t volumeID,
    BlkAddr startRba,
    uint32_t count)
{
    if (unlikely(volumeID >= MAX_VOLUME_COUNT))
    {
        IbofEventId::Print(IBOF_EVENT_ID::RBAMGR_WRONG_VOLUME_ID,
            EventLevel::ERROR);
        return;
    }

    RBAStatesInVolume& targetVolume = rbaStatesInArray[volumeID];
    targetVolume.ReleaseOwnership(startRba, count);
}

bool
RBAStateManager::AcquireOwnership(uint32_t volumeID, BlkAddr rba)
{
    return BulkAcquireOwnership(volumeID, rba, 1);
}

void
RBAStateManager::ReleaseOwnership(uint32_t volumeID, BlkAddr rba)
{
    BulkReleaseOwnership(volumeID, rba, 1);
}

RBAStateManager::RBAState::RBAState(void)
: ownered(ATOMIC_FLAG_INIT)
{
}

bool
RBAStateManager::RBAState::AcquireOwnership(void)
{
    bool acquired = (ownered.test_and_set(memory_order_relaxed) == false);

    return acquired;
}

void
RBAStateManager::RBAState::ReleaseOwnership(void)
{
    ownered.clear(memory_order_relaxed);
}

RBAStateManager::RBAStatesInVolume::RBAStatesInVolume(void)
: rbaStates(nullptr),
  size(0)
{
}

bool
RBAStateManager::RBAStatesInVolume::AcquireOwnership(BlkAddr startRba, uint32_t cnt)
{
    BlkAddr endRba = startRba + cnt - 1;
    if (likely(_IsAccessibleRba(endRba)))
    {
        BlkAddr targetRba = startRba;
        for (; targetRba <= endRba; targetRba++)
        {
            bool targetAcquired = rbaStates[targetRba].AcquireOwnership();
            if (targetAcquired == false)
            {
                uint32_t acquiredCount = targetRba - startRba;
                ReleaseOwnership(startRba, acquiredCount);
                return false;
            }
        }

        return true;
    }
    return false;
}

void
RBAStateManager::RBAStatesInVolume::ReleaseOwnership(BlkAddr startRba, uint32_t cnt)
{
    BlkAddr endRba = startRba + cnt - 1;
    if (likely(_IsAccessibleRba(endRba)))
    {
        for (BlkAddr targetRba = startRba; targetRba <= endRba; targetRba++)
        {
            rbaStates[targetRba].ReleaseOwnership();
        }
    }
}

void
RBAStateManager::RBAStatesInVolume::SetSize(uint64_t newSize)
{
    if (newSize == 0)
    {
        if (rbaStates != nullptr)
        {
            delete[] rbaStates;
            rbaStates = nullptr;
            size = newSize;
        }
    }

    if (size == 0 && newSize > 0)
    {
        rbaStates = new RBAState[newSize];
        size = newSize;
        return;
    }
}

bool
RBAStateManager::RBAStatesInVolume::_IsAccessibleRba(BlkAddr endRba)
{
    return size > endRba;
}

bool
RBAStateManager::VolumeCreated(std::string volName, int volID, uint64_t volSizeByte,
    uint64_t maxiops, uint64_t maxbw)
{
    CreateRBAState(volID, ChangeByteToBlock(volSizeByte));
    return true;
}

bool
RBAStateManager::VolumeDeleted(std::string volName, int volID, uint64_t volSizeByte)
{
    DeleteRBAState(volID);
    return true;
}

bool
RBAStateManager::VolumeMounted(std::string volName, std::string subnqn, int volID, uint64_t volSizeByte,
    uint64_t maxiops, uint64_t maxbw)
{
    return true;
}

bool
RBAStateManager::VolumeUnmounted(std::string volName, int volID)
{
    return true;
}

bool
RBAStateManager::VolumeLoaded(std::string name, int volID, uint64_t volSizeByte,
    uint64_t maxiops, uint64_t maxbw)
{
    CreateRBAState(volID, ChangeByteToBlock(volSizeByte));
    return true;
}

bool
RBAStateManager::VolumeUpdated(std::string volName, int volID, uint64_t maxiops, uint64_t maxbw)
{
    return true;
}

void
RBAStateManager::VolumeDetached(vector<int> volList)
{
}

} // namespace ibofos