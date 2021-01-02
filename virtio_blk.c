#include "pci.h"
#include "virtio_blk.h"
#include "virtio.h"

struct pci_device *virtio_pci_dev;

void virtio_blk_init(void)
{
	cprintf("[+]setup virtio block device\n");

	virtio_pci_dev = get_pci_device(VIRTIO_VENDOR, VIRTIO_PCI_DEVICE_BLOCK_DEVICE, VIRTIO_VENDOR, VIRTIO_DEVICE_TYPE_BLOCK_DEVICE);
	if (!virtio_pci_dev)
		panic("[*]cannot found virtio pci device\n");

	cprintf("[+]found virtio block device\n");

	virtio_init();
}
