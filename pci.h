#ifndef _PCI_H
#define _PCI_H

#include "types.h"
#include "defs.h"

// Store PCI device information.
// https://wiki.osdev.org/PCI#Header_Type_0x00
struct pci_device {
	ushort vendor_id;
	ushort device_id;
	ushort command;
	ushort status;
	uchar revision_id;
	uchar prog_if;
	uchar subclass;
	uchar class;
	uchar cache_line_size;
	uchar latency_time;
	uchar header_type;
	uchar bist;
	uint bar[6];
	uint cardbus;
	ushort subsystem_vendor_id;
	ushort subsystem_id;
	uint expansion_rom_address;
	uint reserved1; // bit0-7 is capabilities pointer;
	uint reserved2;
	uchar interupt_line;
	uchar interupt_pin;
	uchar min_grant;
	uchar max_latency;
};

struct pci_device *get_pci_device(ushort vendor, ushort device, ushort subsystem_vendor, ushort subsystem_id);
void pci_init(void);

#endif // _PCI_H
