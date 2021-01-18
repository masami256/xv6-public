#include "pci.h"
#include "virtio_blk.h"
#include "virtio.h"
#include "x86.h"
#include "memlayout.h"

static struct virtio_device vdev;
static struct virtio_blk_info blk_info;

static void virtio_read_block_device_info(struct virtio_device *vdev, struct virtio_blk_info *blk_info)
{
	uint h, l;

	h = inl(vdev->bar + VIRTIO_BLK_TOTAL_SECTOR_COUNT + 4);
	l = inl(vdev->bar + VIRTIO_BLK_TOTAL_SECTOR_COUNT);

	blk_info->total_sector_count = h;
	blk_info->total_sector_count = (blk_info->total_sector_count << 32) | l;

	blk_info->segment_size = inl(vdev->bar + VIRTIO_BLK_MAX_SEGMENT_SIZE);
	blk_info->segment_count = inl(vdev->bar +  VIRTIO_BLK_MAX_SEGMENT_COUNT);
	blk_info->cylinder_count = inw(vdev->bar + VIRTIO_BLK_CYLINDER_COUNT);
	blk_info->head_count = inb(vdev->bar + VIRTIO_BLK_HEAD_COUNT);
	blk_info->sector_count = inb(vdev->bar + VIRTIO_BLK_SECTOR_COUNT);
	blk_info->block_length = inb(vdev->bar + VIRTIO_BLK_BLOCK_LENGTH);
	
	cprintf("[+]total sector: 0x%x\n", blk_info->total_sector_count);
	cprintf("[+]max segment size: 0x%x\n", blk_info->segment_size);
	cprintf("[+]max segment count: 0x%x\n", blk_info->segment_count);
	cprintf("[+]cylinder count: 0x%x\n", blk_info->cylinder_count);
	cprintf("[+]head count: 0x%x\n", blk_info->head_count);
	cprintf("[+]sector count: 0x%x\n", blk_info->sector_count);
	cprintf("[+]block length: 0x%x\n", blk_info->block_length);
}

static void virt_queue_init(struct virtio_device *vdev)
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

static void virtio_init_driver(struct virtio_device *vdev)
{
	uint device_features;
	cprintf("[+]setup virtio block device\n");
	uint status = 0;

	vdev->pci_info.vendor = VIRTIO_VENDOR;
	vdev->pci_info.device = VIRTIO_PCI_DEVICE_BLOCK_DEVICE;
	vdev->pci_info.subsystem_vendor = VIRTIO_VENDOR;
	vdev->pci_info.subsystem_id = VIRTIO_DEVICE_TYPE_BLOCK_DEVICE;
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

void virtio_blk_init(void)
{
	virtio_init_driver(&vdev);
	virtio_read_block_device_info(&vdev, &blk_info);

}
