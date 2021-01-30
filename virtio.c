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

void virtio_init_driver_common(struct virtio_device *vdev)
{
	uint device_features;
	uint status = 0;

	virtio_init(vdev);

	vdev->bar = get_device_base_address(vdev);
	cprintf("[+]I/O address space: 0x%x\n", vdev->bar);

	cprintf("[+]found virtio block device\n");

	cprintf("[+]initialize virtio driver(device:0x%x, subsystem:0x%x)\n", vdev->pci->device_id, vdev->pci->subsystem_id);
	cprintf("[+]initial device status 0x%x\n", inb(vdev->bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS));
	cprintf("[+]reset the device\n");

	status = VIRTIO_STATUS_DEVICE_NEEDS_RESET;
	set_device_status(vdev, status);

	// step1: Set ACKNOWLEDGE status bit
	status = VIRTIO_STATUS_ACKNOWLEDGE;
	set_device_status(vdev, status);

	// step2: Set DRIVER status bit
	status |= VIRTIO_STATUS_DRIVER;
	set_device_status(vdev, status);

	// step3: read device features bit
	device_features = get_device_feature(vdev);
	cprintf("[+]device features: 0x%x\n", device_features);

	// step4 set FEATURES_OK status bia
	status |= VIRTIO_STATUS_FEATURES_OK;
	set_device_status(vdev, status);

	// step5: read device status field and check FEATURES_OK bit still on
	status = get_device_status(vdev);
	if (!(status & VIRTIO_STATUS_FEATURES_OK))
		panic("virtio unusable\n");

	virt_queue_init(vdev);

	// step6: setup queue address
	set_queue_address(vdev);
	cprintf("[+]virtio queue physical address 0x%x\n", get_queue_size(vdev));

	// step7: write dirvers ok bit
	status |= VIRTIO_STATUS_DRIVER_OK;
	set_device_status(vdev, status);

	status = get_device_status(vdev);
	if ((status == VIRTIO_STATUS_FAILD) || !(status & VIRTIO_STATUS_FEATURES_OK))
		panic("failed to setup virtio driver\n");

	cprintf("[+]final device status: 0x%x\n", status);
	cprintf("[+]virtio driver initialize done.\n");
}
void virt_queue_init(struct virtio_device *vdev)
{
	struct virtq *q = &vdev->queue;
	short q_size;
	uint q_select;
	uint total_size = 0;
	uint desc_size = 0;
	uint avail_size = 0;
	uint total_pages = 0;

	for (q_select = 0; q_select < 8; q_select++) {
		q_size = -1;
		select_virt_queue(vdev, q_select);
		q_size = get_queue_size(vdev);
		if (q_size > 0) {
			cprintf("[+}q_select:0x%x, q_size:0x%x\n", q_select, q_size);
			break;
		}
	}

	if (q_size < 0) 
		panic("couldn't find virtqueue\n");

	cprintf("[+]queue size: %d:%d\n", q_select, q_size);

	desc_size = (PAGE_SIZE_ROUND_UP(16 * q_size));
	avail_size = PAGE_SIZE_ROUND_UP(6 + 2 * q_size);
	total_size = desc_size + avail_size + (PAGE_SIZE_ROUND_UP(6 + 8 * q_size));
	total_pages = total_size / PAGE_SIZE;
	cprintf("[+]total size: %d, desc_size: %d, required pages: %d\n", total_size, desc_size, total_pages);

	q->desc = (struct virtq_descriptor *) alloc_pages(total_pages);
	if (!q->desc)
		panic("failed to allocate virtq descriptor]n");

	q->avail = (struct virtq_available *) ((uint) q->desc + desc_size);
	q->used = (struct virtq_used *) ((uint) q->avail + avail_size);
        cprintf("[+]virtq->desc physical address 0x%x\n", V2P(q->desc));
        cprintf("[+]virtq->avail physical address 0x%x\n", V2P(q->avail));
        cprintf("[+]virtq->used physical address 0x%x\n", V2P(q->used));

	q->avail->flags = 0;
}


void virtio_init(struct virtio_device *vdev)
{
	vdev->pci = get_pci_device(vdev->pci_info.vendor, vdev->pci_info.device, 
			vdev->pci_info.subsystem_vendor, vdev->pci_info.subsystem_id);
	if (!vdev->pci)
		panic("[*]cannot found virtio pci device\n");
}


