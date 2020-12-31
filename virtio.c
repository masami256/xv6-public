#include "virtio.h"

void virtio_init(void)
{
	struct pci_device_info info;
	int r;

	memset(&info, 0, sizeof(info));
	cprintf("initialize virtio\n");

	r = search_pci_device(VIRTIO_VENDOR, &info);
	if (r) 
		panic("virtio device is not found\n");
			

	cprintf("[+]found virtio device(vendor:0x%x) at %d:%d\n", info.device.vendor, info.device.bus, info.device.devfn);
}

