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

// virtio blk device features bits
// spec 5.2.3 Feature bits https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-2420003
#define VIRTIO_BLK_F_SIZE_MAX		1
#define VIRTIO_BLK_F_SEG_MAX		2
#define VIRTIO_BLK_F_GEOMETRY		4
#define VIRTIO_BLK_F_RO				5
#define VIRTIO_BLK_F_BLK_SIZE		6
#define VIRTIO_BLK_F_FLUSH			9
#define VIRTIO_BLK_F_TOPOLOGY		10
#define VIRTIO_BLK_F_CONFIG_WCE 	11
#define VIRTIO_BLK_F_DISCARD		13
#define VIRTIO_BLK_F_WRITE_ZEROES	14
#define VIRTIO_BLK_F_BARRIER		0
#define VIRTIO_BLK_F_SCSI			7

// spec 1.4 Structure Specifications https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-80004
typedef short le16;
typedef int le32;
typedef long long le64;

typedef short be16;
typedef int be32;
typedef long long be64;

struct virtio_device_info {
	struct pci_device pci;
};

void virtio_init(struct virtio_device_info *dev);

#endif // _VIRTIO_H
