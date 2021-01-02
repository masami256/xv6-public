#ifndef _VIRTIO_H
#define _VIRTIO_H

#include "types.h"
#include "defs.h"
#include "pci.h"

// spec 4.1.2 PCI Device Discovery https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-1020002
#define VIRTIO_VENDOR 0x1AF4

// spec 4.1.2.1 Device Requirements: PCI Device Discovery  https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-1020002
#define VIRTIO_PCI_DEVICE_BLOCK_DEVICE 0x1001

// spec 5 Device Types https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-1930005
#define VIRTIO_DEVICE_TYPE_BLOCK_DEVICE 0x02

// spec 2.1 Device Status Field https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-100001
#define VIRTIO_STATUS_ACKNOWLEDGE 1
#define VIRTIO_STATUS_DRIVER 2
#define VIRTIO_STATUS_FAILD 128
#define VIRTIO_STATUS_FEATURES_OK 8
#define VIRTIO_DRIVER_OK 4
#define VIRTIO_DEVICE_NEEDS_RESET 64

void virtio_init(void);

#endif // _VIRTIO_H
