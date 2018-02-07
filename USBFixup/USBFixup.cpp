/*
 * Copyright (c) 2018 syscl. All rights reserved.
 */

#include <Headers/kern_api.hpp>
#include <Headers/kern_util.hpp>
#include <Headers/plugin_start.hpp>

#include <Headers/kern_iokit.hpp>

#include "USBFixup.hpp"

static const char *kextXHCIPCI[] { "/System/Library/Extensions/IOUSBHostFamily.kext/Contents/PlugIns/AppleUSBXHCIPCI.kext/Contents/MacOS/AppleUSBXHCIPCI" };
static const char *kextXHCIPCIId { "com.apple.driver.usb.AppleUSBXHCIPCI" };

static KernelPatcher::KextInfo kextList[] {
    { kextXHCIPCIId,      kextXHCIPCI,   arrsize(kextXHCIPCI),  {true}, {}, KernelPatcher::KextInfo::Unloaded },
};

static size_t kextListSize = arrsize(kextList);

// methods that are implmented here

bool USBFx::init()
{
    LiluAPI::Error error = lilu.onKextLoad(kextList, kextListSize,
       [](void* user, KernelPatcher& patcher, size_t index, mach_vm_address_t address, size_t size) {
           USBFx* patch = static_cast<USBFx*>(user);
           patch->processKext(patcher, index, address, size);
       }, this);
    
    if (error != LiluAPI::Error::NoError)
    {
        SYSLOG(kCurrentKextID, "failed to register onPatcherLoad method %d", error);
        return false;
    }
    
    return true;
}

void USBFx::processKext(KernelPatcher& patcher, size_t index, mach_vm_address_t address, size_t size)
{
    // check if we already done here
    if (progressState == ProcessingState::EverythingDone) return;
    
    for (size_t i = 0; i < kextListSize; i++)
    {
        if (kextList[i].loadIndex != index) continue;
        
        if ((progressState & ProcessingState::EverythingDone) && !strcmp(kextList[i].id, kextXHCIPCIId))
        {
            SYSLOG(kCurrentKextID, "found %s", kextList[i].id);
            
            switch (gKernMajorVersion)
            {
                case KernelVersion::ElCapitan: {
                    const uint8_t find[] { 0x83, 0xBD, 0x8C, 0xFE, 0xFF, 0xFF, 0x10 };
                    const uint8_t repl[] { 0x83, 0xBD, 0x8C, 0xFE, 0xFF, 0xFF, 0x1B };
                    KextPatch rm_port_limit_patch {
                        { &kextList[i], find, repl, sizeof(find), 1 },
                        KernelVersion::ElCapitan, KernelVersion::ElCapitan
                    };
                    applyPatches(patcher, index, &rm_port_limit_patch, 1);
                    SYSLOG(kCurrentKextID, "remove port limit for XHCI to %d", repl[6]);
                    progressState |= ProcessingState::EverythingDone;
                }
                break;
                    
                case KernelVersion::Sierra: {
                    const uint8_t find[] { 0x83, 0xBD, 0x74, 0xFF, 0xFF, 0xFF, 0x10 };
                    const uint8_t repl[] { 0x83, 0xBD, 0x74, 0xFF, 0xFF, 0xFF, 0x1B };
                    KextPatch rm_port_limit_patch {
                        { &kextList[i], find, repl, sizeof(find), 1 },
                        KernelVersion::Sierra, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &rm_port_limit_patch, 1);
                    SYSLOG(kCurrentKextID, "remove port limit for XHCI to %d", repl[6]);
                    progressState |= ProcessingState::EverythingDone;
                }
                break;
                    
                case KernelVersion::HighSierra: {
                    uint8_t find[] { 0x83, 0x7D, 0x8C, 0x10, 0x0F, 0x83, 0x25, 0x1B, 0x00, 0x00 };
                    uint8_t repl[] { 0x83, 0x7D, 0x8C, 0x10, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
                    // correct patch for 10.13.4+
                    if (gKernMinorVersion >= 4)
                    {
                        find[2] = repl[2] = 0x94;
                        find[3] = repl[3] = 0x0F;
                        find[6] = 0x97;
                        find[7] = 0x04;
                    }
                    KextPatch rm_port_limit_patch {
                        { &kextList[i], find, repl, sizeof(find), 1 },
                        KernelVersion::Sierra, KernelVersion::Sierra
                    };
                    applyPatches(patcher, index, &rm_port_limit_patch, 1);
                    SYSLOG(kCurrentKextID, "remove port limit for XHCI");
                    progressState |= ProcessingState::EverythingDone;
                }
                break;
                    
                default: {
                    DBGLOG(kCurrentKextID, "macOS version not support. Aborted");
                    progressState |= ProcessingState::EverythingDone;
                }
                break;
            }
        }
    }
    patcher.clearError();
}

void USBFx::applyPatches(KernelPatcher& patcher, size_t index, const KextPatch* patches, size_t patchNum)
{
    for (size_t p = 0; p < patchNum; p++)
    {
        auto &patch = patches[p];
        if (patch.patch.kext->loadIndex == index)
        {
            if (patcher.compatibleKernel(patch.minKernel, patch.maxKernel))
            {
                patcher.applyLookupPatch(&patch.patch);
                SYSLOG(kCurrentKextID, "patch %s (%ld/%ld).", patch.patch.kext->id, p+1, patchNum);
                patcher.clearError();
            }
        }
    }
}

