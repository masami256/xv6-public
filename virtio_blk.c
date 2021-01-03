#include "pci.h"
#include "virtio_blk.h"
#include "virtio.h"

struct virtio_device_info virtio_blk_dev;
struct pci_device *virtio_pci_dev;

void virtio_blk_init(void)
{
	cprintf("[+]setup virtio block device\n");

	virtio_pci_dev = get_pci_device(VIRTIO_VENDOR, VIRTIO_PCI_DEVICE_BLOCK_DEVICE, VIRTIO_VENDOR, VIRTIO_DEVICE_TYPE_BLOCK_DEVICE);
	if (!virtio_pci_dev)
		panic("[*]cannot found virtio pci device\n");

	cprintf("[+]found virtio block device\n");

	virtio_blk_dev.pci = *virtio_pci_dev;

	virtio_init(&virtio_blk_dev);
}
