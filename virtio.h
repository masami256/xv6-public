#ifndef _VIRTIO_H
#define _VIRTIO_H

#include "types.h"
#include "defs.h"
#include "pci.h"

#define PAGE_SIZE 4096
#define PAGE_SIZE_ROUND_UP(x) ((((x)) + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1))) 

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
#define VIRTIO_STATUS_DRIVER_OK		4
#define VIRTIO_STATUS_DEVICE_NEEDS_RESET	64

// spec 4.1.4.8 Legacy Interfaces: A Note on PCI Device Layout https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-1250008
#define VIRTIO_CFG_OFFSET_DEVICE_FEATURES_BITS	0x0
#define VIRTIO_CFG_OFFSET_DRIVER_FEATURES_BITS 	0x04
#define VIRTIO_CFG_OFFSET_QUEUE_ADDRESS		0x08
#define VIRTIO_CFG_OFFSET_QUEUE_SIZE		0x0c
#define VIRTIO_CFG_OFFSET_QUEUE_SELECT		0x0e
#define VIRTIO_CFG_OFFSET_QUEUE_NOTIFY		0x10
#define VIRTIO_CFG_OFFSET_DEVICE_STATUS		0x12
#define VIRTIO_CFG_OFFSET_ISR_STATUS 		0x13

// device specific registart start offset
#define VIRTIO_DEVICE_SPECIFIC_REGISTER_OFFSET 0x14

// spec 1.4 Structure Specifications https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-80004
typedef short le16;
typedef int le32;
typedef long long le64;

typedef short be16;
typedef int be32;
typedef long long be64;


// spec 2.6.5 The Virtqueue Descriptor Table https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-320005
#define VIRTQ_DESC_F_NEXT		1
#define VIRTQ_DESC_F_WRITE		2
#define VIRTQ_DESC_F_INDIRECT	4
struct virtq_descriptor {
	le64 addr;
	le32 len;
	le16 flags;
	le16 next;
};

// spec 2.6.6 The Virtqueue Available Ring https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-380006
#define VIRTQ_AVAIL_F_NO_INTERRUPT	1
struct virtq_available {
	le16 flags;
	le16 idx;
	le16 ring[];
};

// spec 2.6.8 The Virtqueue Used Ring https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-430008
#define VIRTQ_USED_F_NO_NOTIFY		1
struct virtq_used_element {
	le32 id;
	le32 len;
};

struct virtq_used {
	le16 flags;
	le16 idx;
	struct virtq_used_element ring[];
};

// spec 2.6.2 Legacy Interfaces: A Note on Virtqueue Layout https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-260002
struct virtq {
	uint num;
	struct virtq_descriptor *desc;
	struct virtq_available *avail;
	struct virtq_used *used;
};

struct virtio_pci_device_info {
	ushort vendor;
	ushort device;
	ushort subsystem_vendor;
	ushort subsystem_id;
};

struct virtio_device {
	struct pci_device *pci;
	struct virtio_pci_device_info pci_info;
	uint bar; // base address for the device
	struct virtq queue;
};

uint get_device_base_address(struct virtio_device *vdev);
uint get_device_feature(struct virtio_device *vdev);
void set_driver_features(struct virtio_device *vdev, uint features);
uchar get_device_status(struct virtio_device *vdev);
void set_device_status(struct virtio_device *vdev, uint status);
void select_virt_queue(struct virtio_device *vdev, uint q_select);
short get_queue_size(struct virtio_device *vdev);
void set_queue_address(struct virtio_device *vdev);
uint get_queue_address(struct virtio_device *vdev);


void virtio_init_driver_common(struct virtio_device *vdev);
void virtio_init(struct virtio_device *vdev);

#endif // _VIRTIO_H
