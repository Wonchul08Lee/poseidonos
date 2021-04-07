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

#include "stripe_locker.h"

#include "src/include/ibof_event_id.h"
#include "src/logger/logger.h"

namespace ibofos
{
bool StripeLockerState::isStateChanging;

StripeLocker::StripeLocker()
{
    normalLocker = new StripeLockerNormalState();
    busyLocker = new StripeLockerBusyState();
    locker = normalLocker;
    locker->isStateChanging = false;
}

StripeLocker::~StripeLocker()
{
    delete busyLocker;
    busyLocker = nullptr;
    delete normalLocker;
    normalLocker = nullptr;
}

bool
StripeLocker::TryModeChanging(StripeLockerMode mode)
{
    unique_lock<mutex> lock(statechangingMtx);
    if (state == mode)
    {
        return true;
    }
    locker->isStateChanging = true;
    if (locker->StateChange(mode) == true)
    {
        _ChangeMode(mode);
        return true;
    }
    return false;
}

void
StripeLocker::_ChangeMode(StripeLockerMode mode)
{
    state = mode;
    if (mode == StripeLockerMode::BUSY)
    {
        locker = busyLocker;
        IBOF_TRACE_DEBUG((int)IBOF_EVENT_ID::REBUILD_DEBUG_MSG,
            "busylocker: set size is {}", locker->Count());
        locker->isStateChanging = false;
    }
    else if (mode == StripeLockerMode::NORMAL)
    {
        locker = normalLocker;
        IBOF_TRACE_DEBUG((int)IBOF_EVENT_ID::REBUILD_DEBUG_MSG,
            "normalLocker: set size is {}", locker->Count());
        locker->isStateChanging = false;
    }
}

bool
StripeLocker::TryLock(StripeId id)
{
    unique_lock<mutex> lock(statechangingMtx, defer_lock);
    if (lock.try_lock())
    {
        return locker->TryLock(id);
    }
    else
    {
        return false;
    }
}

void
StripeLocker::Unlock(StripeId id)
{
    locker->Unlock(id);
}

} // namespace ibofos