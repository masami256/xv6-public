#include "pci.h"
#include "x86.h"

// based on my simple x86 os
// https://github.com/masami256/miko/blob/master/drivers/pci/pci.c

#define PCI_BUS_MAX 255
#define PCI_DEVICE_MAX 31
#define PCI_FUNCTION_MAX 7

// PCI CONFIG_ADDRESS
#define PCI_CONFIG_ADDRESS 0x0cf8

// Config Data register range is 0x0cfc - 0x0cff.
#define CONFIG_DATA 0x0cfc

#define MAX_PCI_DEVICES 16
static struct pci_device pci_devices[MAX_PCI_DEVICES];
uint pci_device_idx;

/**
 * Write data to CONFIG_ADDRESS.
 */
static void write_pci_config_address(uchar bus, uchar dev, uchar func, uchar offset, uchar enable)
{
	uint data = 0;
	uchar reserved = 0;

	data = (enable << 31) |
		(reserved << 24) | 
		(bus << 16) | 
		(dev << 11) | 
		(func << 8) |
		offset;

	outl(PCI_CONFIG_ADDRESS, data);	
}

#if 0
static void finish_access_to_config_data(uchar bus, uchar dev, uchar func, uchar offset)
{
	write_pci_config_address(bus, dev, func, offset, 0);
}
#endif

/**
 * Read CONFIG_DATA.
 */
static uint read_pci_config(uchar bus, uchar dev, uchar func, uchar offset)
{
	uint data;

	// write data to CONFIG_ADDRESS.
	write_pci_config_address(bus, dev, func, offset, 1);
	
	data = inl(CONFIG_DATA);

	return data;
}

static void setup_pci_device(uint bus, uint dev)
{
	uint tmp;
	uint func = 0;
	struct pci_device *p; 

	tmp = read_pci_config(bus, dev, func, 0);
	if ((tmp & 0xffff) == 0xffff) 
		return ;

	pci_device_idx++;

	if (pci_device_idx >= MAX_PCI_DEVICES) {
		cprintf("too many pci devices. skip add pci device\n");
		return ;
	}

	p = &pci_devices[pci_device_idx];

	p->vendor_id = tmp & 0xffff;
	p->device_id = (tmp >> 16) & 0xffff;

	tmp = read_pci_config(bus, dev, func, 0x04);
	p->command = tmp & 0xffff;
	p->status = (tmp >> 16) & 0xfff;

	tmp = read_pci_config(bus, dev, func, 0x08);
	p->class = (tmp >> 24) & 0xff;
	p->subclass = (tmp >> 16) & 0xff;

	tmp = read_pci_config(bus, dev, func, 0x0c);
	p->header_type = (tmp >> 16) & 0xff;

	p->bar[0] = read_pci_config(bus, dev, func, 0x10);
	p->bar[1] = read_pci_config(bus, dev, func, 0x14);
	p->bar[2] = read_pci_config(bus, dev, func, 0x18);
	p->bar[3] = read_pci_config(bus, dev, func, 0x1c);
	p->bar[4] = read_pci_config(bus, dev, func, 0x20);
	p->bar[5] = read_pci_config(bus, dev, func, 0x24);

	tmp = read_pci_config(bus, dev, func, 0x2c);
	p->subsystem_vendor_id = tmp & 0xffff;
	p->subsystem_id = (tmp >> 16) & 0xffff;


}

static void show_pci_devices(void)
{
	int i;
	struct pci_device *p;

	for (i = 0; i < MAX_PCI_DEVICES; i++) {
		p = &pci_devices[i];
		if (p->vendor_id) {
			cprintf("[+]%s: vendor(0x%x):device(0x%x), header(0x%x), subsystem vendoer(0x%x), subsystem id(0x%x), bar0:0x%x, bar1:0x%x, bar2:0x%x, bar3:0x%x, bar4:0x%x, bar5:0x%x\n",
				__func__,
				p->vendor_id,
				p->device_id,
				p->header_type,
				p->subsystem_vendor_id,
				p->subsystem_id,
				p->bar[0], p->bar[1],
				p->bar[2], p->bar[3],
				p->bar[4], p->bar[5]);
		}
	}
}

struct pci_device *get_pci_device(ushort vendor, ushort device, ushort subsystem_vendor, ushort subsystem_id)
{
	int i;
	struct pci_device *p;

	for (i = 0; i < MAX_PCI_DEVICES; i++) {
		p = &pci_devices[i];
		if (p->vendor_id == vendor && 
			p->device_id == device &&
			p->subsystem_vendor_id == subsystem_vendor &&
			p->subsystem_id == subsystem_id)
			return p;
	}

	return NULL;
}

void pci_init(void)
{
	uint bus, dev;

	for (bus = 0; bus < PCI_BUS_MAX; bus++) {
		for (dev = 0; dev < PCI_DEVICE_MAX; dev++) {
			setup_pci_device(bus, dev);
		}
	}

	show_pci_devices();
}
