#ifndef _VIRTIO_H
#define _VIRTIO_H

#include "types.h"
#include "defs.h"
#include "pci.h"

#define VIRTIO_VENDOR 0x1AF4

#define VIRTIO_PCI_DEVICE_BLOCK_DEVICE 0x1001

void virtio_init(void);

#endif // _VIRTIO_H
