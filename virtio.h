#ifndef _VIRTIO_H
#define _VIRTIO_H

#include "types.h"
#include "defs.h"
#include "pci.h"

#define VIRTIO_VENDOR 0x1AF4

// https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-1020002
#define VIRTIO_PCI_DEVICE_BLOCK_DEVICE 0x1001

// https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-1930005
#define VIRTIO_DEVICE_TYPE_BLOCK_DEVICE 0x02

void virtio_init(void);

#endif // _VIRTIO_H
