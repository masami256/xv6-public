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
#define VIRTIO_STATUS_ACKNOWLEDGE	1
#define VIRTIO_STATUS_DRIVER		2
#define VIRTIO_STATUS_FAILD		128
#define VIRTIO_STATUS_FEATURES_OK	8
#define VIRTIO_DRIVER_OK		4
#define VIRTIO_DEVICE_NEEDS_RESET	64

// spec 4.1.4.8 Legacy Interfaces: A Note on PCI Device Layout https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-1250008
#define VIRTIO_CFG_OFFSET_DEVICE_FEATURES_BITS	0
#define VIRTIO_CFG_OFFSET_DRIVER_FEATURES_BITS 	4
#define VIRTIO_CFG_OFFSET_QUEUE_ADDRESS		8
#define VIRTIO_CFG_OFFSET_QUEUE_SIZE		12
#define VIRTIO_CFG_OFFSET_QUEUE_SELECT		14
#define VIRTIO_CFG_OFFSET_QUEUE_NOTIFY		16
#define VIRTIO_CFG_OFFSET_DEVICE_STATUS		18
#define VIRTIO_CFG_OFFSET_ISR_STATUS 		20

struct virtio_device_info {
	struct pci_device pci;
};

void virtio_init(struct virtio_device_info *dev);

#endif // _VIRTIO_H
