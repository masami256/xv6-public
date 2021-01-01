#include "virtio.h"

struct pci_device *virtio_pci_dev;

void virtio_init(void)
{
	cprintf("[+]initialize virtio\n");

	virtio_pci_dev = get_pci_device(VIRTIO_VENDOR, VIRTIO_PCI_DEVICE_BLOCK_DEVICE, VIRTIO_VENDOR, VIRTIO_DEVICE_TYPE_BLOCK_DEVICE);
	if (!virtio_pci_dev)
		panic("[*]cannot found virtio pci device\n");

	cprintf("[+]found virtio pci device\n");
}

