//-------------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
//-------------------------------------------------------------------------------------------------------
#pragma once

/*
 * Stackwalking on Evanesco:
 * ------------------------
 * NOTE: This structure is currently only used for _M_ARM, which walks a chain of frame pointers.
 * This requires frame chaining using r11 (i.e., no FPO) and the homing of register parameters on
 * entry to Javascript functions.
 *
 * The relevant part of the frame looks like this (high addresses at the top, low ones at the bottom):
 *
 * ----------------------
 * r3     <=== Homed input parameters
 * r2     <
 * r1     <
 * r0     <===
 * lr     <=== return address
 * r11    <=== current r11 (frame pointer)
 * ...
 *
 */

#if !defined(_M_ARM)
#error This is only for ARM
#endif

namespace Js
{

    class ArmStackFrame
    {
    public:
        ArmStackFrame() : frame(nullptr), codeAddr(nullptr), addressOfCodeAddr(nullptr)
        {
        }

        bool InitializeByFrameId(void * returnAddress, ScriptContext* scriptContext);
        bool InitializeByReturnAddress(void * returnAddress, ScriptContext* scriptContext);

        bool Next();

        void *GetInstructionPointer();
        void **GetArgv(bool isCurrentContextNative = false, bool shouldCheckForNativeAddr = true);
        void *GetReturnAddress(bool isCurrentContextNative = false, bool shouldCheckForNativeAddr = true);
        void *GetAddressOfInstructionPointer() { Assert(addressOfCodeAddr != nullptr); return addressOfCodeAddr; }
        void *GetAddressOfReturnAddress(bool isCurrentContextNative = false, bool shouldCheckForNativeAddr = true);
        bool SkipToFrame(void * returnAddress);
        void *GetFrame() { return (void *)frame;};
        bool IsInStackCheckCode(void *entry) const;

    private:

        void ** frame;         // r11 (frame pointer) - other interesting values are relative to this address
        void * codeAddr;       // return address from the current frame
        void* addressOfCodeAddr;
        //ProbeStack height is 40 when stack probe is before prolog & 58 when it is after prolog (for small stacks)
        //Choosing the higher number is safe as @58 we are still in the prolog and inlinee frame is not setup yet and
        //we shouldn't try to query the frame height for the inlined function.
        //TODO (abchatra): Get rid of this magic number in future and design a more safe way of handling the stack check code height.
        static const size_t stackCheckCodeHeight = StackFrameConstants::StackCheckCodeHeight;
    };

};