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
#define CONFIG_DATA_1 0x0cfc

/**
 * Write data to CONFIG_ADDRESS.
 * @param reg it should be set bus, device, function and so forth.
 */
static void write_pci_config_address(const struct pci_configuration_register *reg)
{
	uint data = 0;

	data = (reg->enable_bit << 31) |
		(reg->reserved << 24) | 
		(reg->bus_num << 16) | 
		(reg->dev_num << 11) | 
		(reg->func_num << 8) |
		reg->reg_num;

	outl(PCI_CONFIG_ADDRESS, data);	
}

/**
 * Set ENABLE bit to 0 and write data to CONFIG_ADDRESS.
 */
static void finish_access_to_config_data(struct pci_configuration_register *reg)
{
	reg->enable_bit = 0;
	write_pci_config_address(reg);
}


/**
 * Read CONFIG_DATA.
 * @param reg it should be set bus, device, function and so forth.
 * @return data from CONFIG_DATA.
 */
static uint read_pci_data(struct pci_configuration_register *reg)
{
	uint data;

	// Enable bit should be 1 before read PCI_DATA.
	reg->enable_bit = 1;

	// write data to CONFIG_ADDRESS.
	write_pci_config_address(reg);
	
	data = inl(CONFIG_DATA_1);

	finish_access_to_config_data(reg);

	return data;
}

/**
 * Read pci class.
 * @param reg it should be set bus, device, function and so forth.
 * @return PCI class.
 */
static uint read_pci_class(struct pci_configuration_register *reg)
{
	reg->reg_num = 0x8;
	return read_pci_data(reg);
}

/**
 * Read CONFIG_DATA by register 0x00.
 * @param reg it should be set bus, device, function and so forth.
 * @return vendor id and device id.
 */
static uint read_pci_reg00(struct pci_configuration_register *reg)
{
	reg->reg_num = 0;
	return read_pci_data(reg);
}

/**
 * Read CONFIG_DATA by register 0x04.
 * @param reg it should be set bus, device, function and so forth.
 * @return status.
 */
//static uint read_pci_command_register(struct pci_configuration_register *reg)
//{
//	reg->reg_num = 0x4;
//	return read_pci_data(reg);
//}

/**
 * Read CONFIG_DATA by register 0x0c to check if it's PCI brigdge or not.
 * @param reg it should be set bus, device, function and so forth.
 * @return vendor id and device id.
 */
static uint read_pci_header_type(struct pci_configuration_register *reg)
{
	reg->reg_num = 0xc;
	return read_pci_data(reg);
}

/**
 * Read CONFIG_DATA by register 0x2c to get its sub system data.
 * @param reg it should be set bus, device, function and so forth.
 * @return sub system.
 */
static uint read_pci_sub_system(struct pci_configuration_register *reg)
{
	reg->reg_num = 0x2c;
	return read_pci_data(reg);
}

static void store_pci_device_data(uchar bus, uchar devfn, 
			 uint data, uchar func,
			 struct pci_device_info *info)
{
	uint class = read_pci_class(&info->reg);
	uint header = read_pci_header_type(&info->reg);
	uint subsystem = read_pci_sub_system(&info->reg);

	memset(&info->device, 0, sizeof(info->device));

	info->device.bus = bus;
	info->device.devfn = devfn;
	info->device.vendor = data & 0xffff;
	info->device.devid = (data >> 16) & 0xffff;
	info->device.pg_if = (class >> 8) & 0xff;
	info->device.sub_class = (class >> 16) & 0xff;
	info->device.base_class = (class >> 24) & 0xff;
	info->device.func = func;
	info->device.header_type = ((header >> 16) & 0xff) & 0x7f;
	info->device.multi = (header & 0x800000) ? 1 : 0;
	info->device.sub_vender = subsystem & 0xffff;
	info->device.sub_devid = (subsystem >> 16) & 0xffff;
}

static int find_device(uint vendor, uint bus, uint dev, struct pci_device_info *info)
{
	struct pci_configuration_register reg;
	uint i;

	memset(&reg, 0, sizeof(reg));

	reg.bus_num = bus;
	reg.dev_num = dev;

	for (i = 0; i < PCI_FUNCTION_MAX; i++) {
		uint data;
		reg.func_num = i;
		data = read_pci_reg00(&reg);

		if ((data & 0xffff) == vendor) {
			info->reg = reg;
			store_pci_device_data(bus, dev, data, i, info);

			return 0;
		}
	}
	return -1;
}

int search_pci_device(uint vendor, struct pci_device_info *info)
{
	uint bus, dev;
	int r = -1;

	for (bus = 0; bus < PCI_BUS_MAX; bus++) {
		for (dev = 0; dev < PCI_DEVICE_MAX; dev++) {
			r = find_device(vendor, bus, dev, info);
			if (!r)
				return r;
		}
	}

	return r;
}
