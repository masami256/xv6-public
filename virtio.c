#include "virtio.h"
#include "x86.h"
#include "memlayout.h"

uint get_queue_address(struct virtio_device *vdev)
{
	return inl(vdev->bar + VIRTIO_CFG_OFFSET_QUEUE_ADDRESS);
}

void set_queue_address(struct virtio_device *vdev)
{
	outl(vdev->bar + VIRTIO_CFG_OFFSET_QUEUE_ADDRESS, V2P(vdev->queue.desc));
}

short get_queue_size(struct virtio_device *vdev)
{
	return inw(vdev->bar + VIRTIO_CFG_OFFSET_QUEUE_SIZE);
}

void select_virt_queue(struct virtio_device *vdev, uint q_select)
{
	outl(vdev->bar + VIRTIO_CFG_OFFSET_QUEUE_ADDRESS, q_select);
}

void set_device_status(struct virtio_device *vdev, uint status)
{
	outb(vdev->bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS, status);
}

void set_driver_features(struct virtio_device *vdev, uint features)
{
	outl(vdev->bar + VIRTIO_CFG_OFFSET_DRIVER_FEATURES_BITS, features);
}

uint get_device_feature(struct virtio_device *vdev)
{
	return inl(vdev->bar + VIRTIO_CFG_OFFSET_DEVICE_FEATURES_BITS);
}

uchar get_device_status(struct virtio_device *vdev)
{
	return inb(vdev->bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS);
}

uint get_device_base_address(struct virtio_device *vdev)
{	
	// https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-1250008
	// https://wiki.osdev.org/Pci#Base_Address_Registers
	// step0: get i/o address
	uint bar = vdev->pci->bar[0];
	if (!(bar & 0x01))
		panic("this device is somthing wrong\n");
	return bar &= 0xfffffffc;
}

void virtio_init(struct virtio_device *vdev)
{
	vdev->pci = get_pci_device(vdev->pci_info.vendor, vdev->pci_info.device, 
			vdev->pci_info.subsystem_vendor, vdev->pci_info.subsystem_id);
	if (!vdev->pci)
		panic("[*]cannot found virtio pci device\n");
}


