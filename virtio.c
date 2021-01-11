#include "virtio.h"
#include "memlayout.h"
#include "x86.h"

static struct virtq virtq;
#define PAGE_SIZE 4096
#define PAGE_SIZE_ROUND_UP(x) ((((x)) + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1))) 

#if 1
static void show_device_features(uint bar)
{
	uint f = inl(bar + VIRTIO_CFG_OFFSET_DEVICE_FEATURES_BITS);
	
	cprintf("[+]device features: ");
	if (f & VIRTIO_BLK_F_SIZE_MAX)
		cprintf("VIRTIO_BLK_F_SIZE_MAX ");
	if (f & VIRTIO_BLK_F_SEG_MAX)
		cprintf("VIRTIO_BLK_F_SEG_MAX ");
	if (f & VIRTIO_BLK_F_GEOMETRY)
		cprintf("VIRTIO_BLK_F_GEOMETRY ");
	if (f & VIRTIO_BLK_F_RO)
		cprintf("VIRTIO_BLK_F_RO ");
	if (f & VIRTIO_BLK_F_BLK_SIZE)
		cprintf("VIRTIO_BLK_F_BLK_SIZE ");
	if (f & VIRTIO_BLK_F_FLUSH)
		cprintf("VIRTIO_BLK_F_FLUSH ");
	if (f & VIRTIO_BLK_F_TOPOLOGY)
		cprintf("VIRTIO_BLK_F_TOPOLOGY ");
	if (f & VIRTIO_BLK_F_CONFIG_WCE)
		cprintf("VIRTIO_BLK_F_CONFIG_WCE ");
	if (f & VIRTIO_BLK_F_DISCARD)
		cprintf("VIRTIO_BLK_F_DISCARD ");
	if (f & VIRTIO_BLK_F_WRITE_ZEROES)
		cprintf("VIRTIO_BLK_F_WRITE_ZEROES ");
	if (f & VIRTIO_BLK_F_BARRIER)
		cprintf("VIRTIO_BLK_F_BARRIER ");
	if (f & VIRTIO_BLK_F_SCSI)
		cprintf("VIRTIO_BLK_F_SCSI ");

	cprintf("\n");
}
#endif 

void virtio_init(struct virtio_device_info *dev)
{
	uint bar;
	uchar status;
	uint device_features;
	short q_size;
	uint q_select;
	uint total_size = 0;
	uint desc_size = 0;
	uint avail_size = 0;
	uint total_pages = 0;


	// https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-1250008
	// https://wiki.osdev.org/Pci#Base_Address_Registers
	// step0: get i/o address
	bar = dev->pci.bar[0];
	if (!(bar & 0x01))
		panic("this device is somthing wrong\n");
	bar &= 0xfffffffc;
	cprintf("[+]I/O address space: 0x%x\n", bar);

	cprintf("[+]initialize virtio driver(device:0x%x, subsystem:0x%x)\n", dev->pci.device_id, dev->pci.subsystem_id);
	cprintf("[+]initial device status 0x%x\n", inb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS));
	cprintf("[+]reset the device\n");
	outb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS, 0);
	cprintf("[+]reset result 0x%x\n", inb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS));

	// step1: Set ACKNOWLEDGE status bit
	outb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS, VIRTIO_STATUS_ACKNOWLEDGE);

	// step2: Set DRIVER status bit
	outb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS, VIRTIO_STATUS_ACKNOWLEDGE | VIRTIO_STATUS_DRIVER);

	// step3: read device features bit
	device_features = inl(bar + VIRTIO_CFG_OFFSET_DEVICE_FEATURES_BITS);
	cprintf("[+]device features: 0x%x\n", device_features);

	// step4 set FEATURES_OK status bit
	outb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS, VIRTIO_STATUS_ACKNOWLEDGE | VIRTIO_STATUS_DRIVER | VIRTIO_STATUS_FEATURES_OK);

	// step5: read device status field and check FEATURES_OK bit still on
	status = inb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS);
	if (!(status & VIRTIO_STATUS_FEATURES_OK))
		panic("virtio unusable\n");

	cprintf("[+]status: 0x%x\n", status);

	for (q_select = 0; q_select < 8; q_select++) {
		q_size = -1;
		outl(bar + VIRTIO_CFG_OFFSET_QUEUE_ADDRESS, q_select);
		q_size = inw(bar + VIRTIO_CFG_OFFSET_QUEUE_SIZE);
		if (q_size > 0)
			break;
	}

	if (q_size < 0) 
		panic("couldn't find virtqueue\n");

	cprintf("[+]queue size: %d:%d\n", q_select, q_size);

	desc_size = (PAGE_SIZE_ROUND_UP(16 * q_size));
	avail_size = PAGE_SIZE_ROUND_UP(6 + 2 * q_size);
	total_size = desc_size + avail_size + (PAGE_SIZE_ROUND_UP(6 + 8 * q_size));
	total_pages = total_size / PAGE_SIZE;
	cprintf("[+]total size: %d, desc_size: %d, required pages: %d\n", total_size, desc_size, total_pages);

	virtq.desc = (struct virtq_descriptor *) alloc_pages(total_pages);
	if (!virtq.desc)
		panic("failed to allocate virtq descriptor]n");

	virtq.avail = (struct virtq_available *) ((uint) virtq.desc + desc_size);
	virtq.used = (struct virtq_used *) ((uint) virtq.avail + avail_size);
        cprintf("[+]virtq->desc physical address 0x%x\n", V2P(virtq.desc));
        cprintf("[+]virtq->avail physical address 0x%x\n", V2P(virtq.avail));
        cprintf("[+]virtq->used physical address 0x%x\n", V2P(virtq.used));

	// step6: setup queue address
	outl(bar + VIRTIO_CFG_OFFSET_QUEUE_ADDRESS, V2P(virtq.desc));
	cprintf("[+]virtio queue physical address 0x%x\n", inl(bar + VIRTIO_CFG_OFFSET_QUEUE_ADDRESS));

	virtq.avail->flags = 0;

	// step7: write dirvers ok bit
	outb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS, VIRTIO_STATUS_ACKNOWLEDGE | VIRTIO_STATUS_DRIVER | VIRTIO_STATUS_FEATURES_OK | VIRTIO_STATUS_DRIVER_OK);

	status = inb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS);
	if ((status == VIRTIO_STATUS_FAILD) || !(status & VIRTIO_STATUS_FEATURES_OK))
		panic("failed to setup virtio driver\n");

	cprintf("[+]final device status: 0x%x\n", inb(bar + VIRTIO_CFG_OFFSET_DEVICE_STATUS));
	cprintf("[+]virtio driver initialize done.\n");
	show_device_features(bar);

	cprintf("total sector: 0x%x\n", inl(bar + VIRTIO_DEVICE_SPECIFIC_REGISTER_OFFSET));
	cprintf("segment size: 0x%x\n", inl(bar + 0x1c));
	cprintf("segment count: 0x%x\n", inl(bar + 0x20));
}


