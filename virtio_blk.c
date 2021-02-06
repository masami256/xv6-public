#include "pci.h"
#include "virtio_blk.h"
#include "virtio.h"
#include "x86.h"
#include "memlayout.h"

static struct virtio_device vdev;
static struct virtio_blk_info blk_info;

void virtio_blk_rw(struct buf *buf)
{
	cprintf("[+]%s\n", __func__);
}

void virtio_blk_read(struct virtio_device *vdev, struct virtio_blk_info *blk_info)
{
	vdev->queue.desc[0].len = 512;
	vdev->queue.desc[0].flags = VIRTQ_DESC_F_NEXT | VIRTQ_DESC_F_WRITE;

	set_queueno_notify(vdev, 0);
	cprintf("[+]ISR status: 0x%x\n", get_isr_status(vdev));

}

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

static void virtio_init_driver(struct virtio_device *vdev)
{
	cprintf("[+]setup virtio block device\n");

	vdev->pci_info.vendor = VIRTIO_VENDOR;
	vdev->pci_info.device = VIRTIO_PCI_DEVICE_BLOCK_DEVICE;
	vdev->pci_info.subsystem_vendor = VIRTIO_VENDOR;
	vdev->pci_info.subsystem_id = VIRTIO_DEVICE_TYPE_BLOCK_DEVICE;

	virtio_init_driver_common(vdev);
}

void virtio_blk_init(void)
{
	virtio_init_driver(&vdev);
	virtio_read_block_device_info(&vdev, &blk_info);

	// test
	virtio_blk_read(&vdev, &blk_info);
}

