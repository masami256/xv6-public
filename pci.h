#ifndef _PCI_H
#define _PCI_H

#include "types.h"
#include "defs.h"

// This structure represents PCI's CONFIG_ADDRESS register.
struct pci_configuration_register {
	uint enable_bit;      // 31: enable bit.
	uint reserved;        // 24-30: reserved.
	uint bus_num;         // 16-23: bus number.
	uint dev_num;         // 11-15: device number.
	uint func_num;        // 8-10: function number.
	uint reg_num;         // 2-7: regster number.
	uint bit0;            // 0-1: always 0.
};

// Store PCI device information.
struct pci_device {
	uchar bus;		// bus number.
	uchar devfn;		// device number.
	uchar func;		// function number.
	// 0x0
	ushort vendor;		// vendor id.
	ushort devid;		// device id.

	// 0x08
	uchar revid;		// revision id.
	uchar pg_if;		// program interface.
	uint sub_class;		// sub class.
	uint base_class;	// base class.

	// 0x0c 
	uchar header_type;	// header type.
	uchar multi;		// multi device.

	// 0x2c
	ushort sub_vender;	// sub system vender id.
	ushort sub_devid;	// sub system device id.

	// 0x10-0x24
	uint base_address;	// base address.
};

struct pci_device_info {
	struct pci_configuration_register reg;
	struct pci_device device;
};

int search_pci_device(uint vendor, struct pci_device_info *info);

#endif // _PCI_H
