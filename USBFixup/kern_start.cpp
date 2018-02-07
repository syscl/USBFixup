/*
 * Copyright (c) 2018 syscl. All rights reserved.
 */

#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>

#include "USBFixup.hpp"

static USBFx ufx;

const char* kBootArgvDisable[] = { "-ufxoff"  };
const char* kBootArgvDebug[]   = { "-ufxdbg"  };
const char* kBootArgvBeta[]    = { "-ufxbeta" };


PluginConfiguration ADDPR(config) =
{
    xStringify(PRODUCT_NAME),
    parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal | LiluAPI::AllowInstallerRecovery,
    
    kBootArgvDisable, arrsize(kBootArgvDisable),
    
    kBootArgvDebug,   arrsize(kBootArgvDebug),
    
    kBootArgvBeta,    arrsize(kBootArgvBeta),
    
    // minKernel: 10.11.x - maxKernel: 10.13.z
    KernelVersion::ElCapitan, KernelVersion::HighSierra,
    
    // now let's get start
    []() { ufx.init(); }
};
