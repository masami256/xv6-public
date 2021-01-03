#include "virtio.h"
#include "memlayout.h"
#include "x86.h"

void virtio_init(struct virtio_device_info *dev)
{
	uint bar;
	uchar status;
	uint device_features;

	cprintf("[+]initialize virtio driver(device:0x%x, subsystem:0x%x)\n", dev->pci.device_id, dev->pci.subsystem_id);

	// https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-1250008
	// https://wiki.osdev.org/Pci#Base_Address_Registers
	// step0: get i/o address
	bar = dev->pci.bar[0];
	if (!(bar & 0x01))
		panic("this device is somthing wrong\n");
	bar &= 0xfffffffc;
	cprintf("[+]I/O address space: 0x%x\n", bar);

	// step1: Set ACKNOWLEDGE status bit
	outb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS, VIRTIO_STATUS_ACKNOWLEDGE);
	status = inb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS);

	// step2: Set DRIVER status bit
	outb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS, status | VIRTIO_STATUS_DRIVER);

	// step3: read device features bit
	device_features = inl(bar + VIRTIO_CFG_OFFSET_DEVICE_FEATURES_BITS);
	cprintf("[+]device features: 0x%x\n", device_features);

	// step4 set FEATURES_OK status bit
	outb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS, status | VIRTIO_STATUS_FEATURES_OK);

	// step5: read device status field and check FEATURES_OK bit still on
	status = inb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS);
	if (!(status & VIRTIO_STATUS_FEATURES_OK))
		panic("virtio unusable\n");

	cprintf("[+]status: 0x%x\n", status);

}


