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

#include "region.h"
#include "src/include/memory.h"
#include "rte_malloc.h"

bool
Region::Move(Region* target)
{
    IBOF_EVENT_ID rc = IBOF_EVENT_ID::SUCCESS;
    MetaLpnType targetBaseLPN = target->content.GetBaseMetaLpn();
    MetaLpnType sourceBaseLPN = content.GetBaseMetaLpn();

    IBOF_TRACE_DEBUG((int)IBOF_EVENT_ID::MFS_DEBUG_MESSAGE,
        "Copy meta lpns<mediaType, source_startLpn, target_startLpn, totalLpn>={}, {}, {}, {}",
        (int)regionType, targetBaseLPN, sourceBaseLPN, content.GetSize());

    void* rBuf = (void*)rte_malloc(nullptr, 4096, 1);
    MDPage bufferPage(nullptr);
    bufferPage.Init(rBuf);

    for (uint64_t idx = 0; idx < content.GetSize(); idx++)
    {
        rc = mssIntf->ReadPage(regionType, sourceBaseLPN + idx, bufferPage.GetDataBuf(), 1);
        if (rc != IBOF_EVENT_ID::SUCCESS)
        {
            IBOF_TRACE_DEBUG((int)IBOF_EVENT_ID::MFS_DEBUG_MESSAGE,
                "Failed loading meta lpns {} of {}, #{}",
                idx + 1, content.GetSize(), sourceBaseLPN + idx);
            break;
        }

        bufferPage.Make(targetBaseLPN + idx, content.GetInode()->data.basic.field.fd);

        rc = mssIntf->WritePage(regionType, targetBaseLPN + idx, bufferPage.GetDataBuf(), 1);
        if (rc != IBOF_EVENT_ID::SUCCESS)
        {
            IBOF_TRACE_DEBUG((int)IBOF_EVENT_ID::MFS_DEBUG_MESSAGE,
                "Failed saving meta lpns {} of {}, #{}",
                idx, content.GetSize(), targetBaseLPN + idx);
            break;
        }
    }

    if (IBOF_EVENT_ID::SUCCESS == rc)
    {
        IBOF_TRACE_DEBUG((int)IBOF_EVENT_ID::MFS_INFO_MESSAGE,
            "Try to trim: startLpn={}, count={}", sourceBaseLPN, content.GetSize());
        rc = Erase(sourceBaseLPN, content.GetSize());
    }

    rte_free(rBuf);

    content.SetBaseMetaLpn(targetBaseLPN);
    content.GetInode()->data.basic.field.pagemap.baseMetaLpn = targetBaseLPN;

    return (rc == IBOF_EVENT_ID::SUCCESS) ? true : false;
}

IBOF_EVENT_ID
Region::Erase(MetaLpnType startLpn, MetaLpnType numTrimLpns)
{
    void* buf = nullptr;

    IBOF_EVENT_ID ret = IBOF_EVENT_ID::SUCCESS;
    ret = mssIntf->TrimFileData(regionType, startLpn, buf, numTrimLpns);

    if (ret != IBOF_EVENT_ID::SUCCESS)
    {
        IBOF_TRACE_DEBUG((int)IBOF_EVENT_ID::MFS_DEBUG_MESSAGE,
            "NVMe Admin TRIM has been failed due to not supported the command");

        const FileSizeType pageSize = MetaFsIoConfig::META_PAGE_SIZE_IN_BYTES;
        FileSizeType fileSize = numTrimLpns * pageSize;

        buf = Memory<pageSize>::Alloc(fileSize / pageSize);
        assert(buf != nullptr);

        // write all zeros
        ret = mssIntf->WritePage(regionType, startLpn, buf, numTrimLpns); // should be async.

        if (ret != IBOF_EVENT_ID::SUCCESS)
        {
            IBOF_TRACE_DEBUG((int)IBOF_EVENT_ID::MFS_DEBUG_MESSAGE,
                "failed to write all zero patterned data");

            Memory<>::Free(buf);
            return ret;
        }
        Memory<>::Free(buf);
    }

    return ret;
}