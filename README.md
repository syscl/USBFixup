USBFixup
====

This kext provides you the ability to fix up the USB (```OHCI```, ```UHCI```and ```XHCI```) common issues including port limits issues and port identification issues on macOS (```10.11+```).

####  Requriments
Lilu.kext ```1.2.0+```
macOS version that is greater than ```10.11``` (because previous systems do not have this kind of issue)

####  Usage
Place it under any bootloader injection folder or ```/Library/Extensions``` then reboot.

####  Credits
- vit9696 for his Lilu - a kext that patches kext and processes.
- arix98 for his initial ideas about remove the ports limit of ```XHCI```
- syscl for writing and maintaining this software

Copyright (c) 2018 syscl. All rights reserved.
