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

#pragma once

#include "comp_mdpage_gen.h"
#include "mdpage.h"
#include "mfs_async_runnable_template.h"
#include "mfs_asynccb_cxt_template.h"
#include "mfs_def.h"
#include "mim_req.h"
#include "mim_state.h"
#include "mpio_io_info.h"
#include "mpio_state_execute_entry.h"
#include "mss.h"

#define AsMpioStateEntryPoint(funcPointer, obj) AsEntryPointParam1(funcPointer, obj)

enum class MpioType
{
    First,
    Write = First,
    Read,
    Last = Read,
    Max,
};

class Mpio;
using PartialMpioDoneCb = std::function<void(Mpio*)>;
using MpioAsyncDoneCb = AsyncCallback;

// meta page io class
class Mpio : public MetaAsyncRunnableClass<MetaAsyncCbCxt, MpAioState, MpioStateExecuteEntry>
{
public:
    explicit Mpio(void* mdPageBuf);
    Mpio(void* mdPageBuf, MetaStorageType targetMediaType, MpioIoInfo& mpioIoInfo, bool partialIO, bool forceSyncIO);
    virtual ~Mpio(void);
    Mpio(const Mpio& mpio) = delete;
    Mpio& operator=(const Mpio& mio) = delete;
    void Reset(void);

    void Setup(MetaStorageType targetMediaType, MpioIoInfo& mpioIoInfo, bool partialIO, bool forceSyncIO);
    void SetLocalAioCbCxt(MpioAsyncDoneCb& callback);
    virtual MpioType GetType(void) = 0;
    virtual void InitStateHandler(void) = 0;

    void SetPartialDoneNotifier(PartialMpioDoneCb& partialMpioDoneNotifier);
    uint32_t GetId(void);
    void SetId(uint32_t id);
    bool IsAIOMode(void);
    bool IsPartialIO(void);
    const MetaIoOpcode GetOpcode(void);
    void AllocateMDPage(void* buf);
    void BuildCompositeMDPage(CompMDPageGenClass* compMdPageGenHelper);
    bool IsValidPage(void);
    bool CheckDataIntegrity(void);
    void* GetMDPageDataBuf(void);
    void* GetUserDataBuf(void);
    void Issue(void);

    bool DoIO(MpAioState expNextState);
    bool DoE2ECheck(MpAioState expNextState);
    bool CheckReadStatus(MpAioState expNextState);
    bool CheckWriteStatus(MpAioState expNextState);
    MfsError GetErrorStatus(void);

    MpioIoInfo io;

protected:
    MDPage mdpage;

    bool partialIO;
    MetaStorageSubsystem* mssIntf;
    bool aioModeEnabled;
    int error;
    bool errorStopState;
    bool forceSyncIO;
    MetaAsyncCbCxt aioCbCxt;

    bool _DoMemCpy(void* dst, void* src, size_t nbytes);
    bool _DoMemSetZero(void* addr, size_t nbytes);

    static void _HandleAsyncMemOpDone(void* obj);
    void _HandlePartialDone(void* notused = nullptr);

private:
    MssOpcode _ConvertToMssOpcode(const MpAioState mpioState);
    bool _CheckIOStatus(MpAioState expNextState);
    void _BackupMssAioCbCxtPointer(MssAioCbCxt* cbCxt);

    MssAioData mssAioData;
    MssAioCbCxt mssAioCbCxt;

    PartialMpioDoneCb partialMpioDoneNotifier;
    MssCallbackPointer mpioDoneCallback;
};

extern InstanceTagIdAllocator mpioTagIdAllocator;
