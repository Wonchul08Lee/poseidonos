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

#include "mfs_io_range_overlap_chker.h"

#include "src/include/branch_prediction.h"

MetaFsIoRangeOverlapChker::MetaFsIoRangeOverlapChker(void)
{
    outstandingIoMap = nullptr;
}
MetaFsIoRangeOverlapChker::~MetaFsIoRangeOverlapChker(void)
{
    if (nullptr != outstandingIoMap)
    {
        delete outstandingIoMap;
    }
}

void
MetaFsIoRangeOverlapChker::Init(MetaLpnType maxLpn)
{
    outstandingIoMap = new BitMap(maxLpn + 1);
    outstandingIoMap->ResetBitmap();
}

void
MetaFsIoRangeOverlapChker::Reset(void)
{
    delete outstandingIoMap;
    outstandingIoMap = nullptr;
}

bool
MetaFsIoRangeOverlapChker::IsRangeOverlapConflicted(MetaFsIoReqMsg* newReq)
{
    if (unlikely(outstandingIoMap == nullptr))
        return false;

    if (false == outstandingIoMap->IsSetBit(newReq->baseMetaLpn))
        return false;

    return true;
}

void
MetaFsIoRangeOverlapChker::FreeLockContext(uint64_t startLpn, bool isRead)
{
    if (true != isRead)
    {
        outstandingIoMap->ClearBit(startLpn);
    }
}

void
MetaFsIoRangeOverlapChker::PushReqToRangeLockMap(MetaFsIoReqMsg* newReq)
{
    if (likely(outstandingIoMap != nullptr))
    {
        if (newReq->reqType == MetaIoReqTypeEnum::Write)
        {
            outstandingIoMap->SetBit(newReq->baseMetaLpn);
        }
    }
}

BitMap*
MetaFsIoRangeOverlapChker::GetOutstandingMioMap(void)
{
    return outstandingIoMap;
}

uint64_t
MetaFsIoRangeOverlapChker::GetOutstandingMioCount(void)
{
    return outstandingIoMap->GetNumBitsSet();
}