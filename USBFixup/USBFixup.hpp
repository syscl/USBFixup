/*
 * Copyright (c) 2018 syscl. All rights reserved.
 */

#ifndef USBFixup_hpp
#define USBFixup_hpp

#include <Headers/kern_patcher.hpp>

#define kCurrentKextID "org.syscl.driver.USBFixup"

struct KextPatch {
    KernelPatcher::LookupPatch patch;
    uint32_t minKernel;
    uint32_t maxKernel;
};

class USBFx
{
public:
    // default constructor
    USBFx() : gKernMajorVersion(getKernelVersion()), gKernMinorVersion(getKernelMinorVersion()) { }
    // destructor
    ~USBFx() { }
    // methods that are used to process patching
    bool init();
	
private:
    const KernelVersion gKernMajorVersion;
    const KernelMinorVersion gKernMinorVersion;
    
	/**
	 *  Patch kext if needed and prepare other patches
	 *
	 *  @param patcher KernelPatcher instance
	 *  @param index   kinfo handle
	 *  @param address kinfo load address
	 *  @param size    kinfo memory size
	 */
	void processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);
    
    /**
     *  Apply kext patches for loaded kext index
     *
     *  @param patcher    KernelPatcher instance
     *  @param index      kinfo index
     *  @param patches    patch list
     *  @param patchesNum patch number
     */
    void applyPatches(KernelPatcher &patcher, size_t index, const KextPatch *patches, size_t patchesNum);
    
    /**
     *  Current progress mask
     */
    struct ProcessingState {
        enum {
            NothingReady = 0,
            EverythingDone = 1,
        };
    };
    int progressState {ProcessingState::NothingReady};
};

#endif /* USBFixup_hpp */
