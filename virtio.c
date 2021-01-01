#include "virtio.h"

struct pci_device_info virtio_pci_dev;

void virtio_init(void)
{
	int r;

	cprintf("initialize virtio\n");
	memset(&virtio_pci_dev, 0, sizeof(virtio_pci_dev));

	r = search_pci_device(VIRTIO_VENDOR, &virtio_pci_dev);
	if (r) 
		panic("virtio device is not found\n");
			

	cprintf("[+]found virtio device(vendor:0x%x) at %d:%d\n", virtio_pci_dev.device.vendor, virtio_pci_dev.device.bus, virtio_pci_dev.device.devfn);
}

