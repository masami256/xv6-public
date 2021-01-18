#ifndef _VIRTIO_BLK_H
#define _VIRTIO_BLK_H

#include "virtio.h"

// virtio blk device features bits
// spec 5.2.3 Feature bits https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-2420003
#define VIRTIO_BLK_F_SIZE_MAX		1
#define VIRTIO_BLK_F_SEG_MAX		2
#define VIRTIO_BLK_F_GEOMETRY		4
#define VIRTIO_BLK_F_RO			5
#define VIRTIO_BLK_F_BLK_SIZE		6
#define VIRTIO_BLK_F_FLUSH		9
#define VIRTIO_BLK_F_TOPOLOGY		10
#define VIRTIO_BLK_F_CONFIG_WCE 	11
#define VIRTIO_BLK_F_DISCARD		13
#define VIRTIO_BLK_F_WRITE_ZEROES	14
#define VIRTIO_BLK_F_BARRIER		0
#define VIRTIO_BLK_F_SCSI		7

// virtio device specific register
#define VIRTIO_BLK_TOTAL_SECTOR_COUNT VIRTIO_DEVICE_SPECIFIC_REGISTER_OFFSET
#define VIRTIO_BLK_MAX_SEGMENT_SIZE	0x1c
#define VIRTIO_BLK_MAX_SEGMENT_COUNT	0x20
#define VIRTIO_BLK_CYLINDER_COUNT	0x24
#define VIRTIO_BLK_HEAD_COUNT		0x26
#define VIRTIO_BLK_SECTOR_COUNT		0x27
#define VIRTIO_BLK_BLOCK_LENGTH		0x28

// request type https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-2500006
#define VIRTIO_BLK_T_IN			0
#define VIRTIO_BLK_T_OUT		1
#define VIRTIO_BLK_T_FLUSH		4
#define VIRTIO_BLK_T_DISCARD		11
#define VIRTIO_BLK_T_WRITE_ZEROES	13

// https://wiki.osdev.org/Virtio#Block_Device_Registers
struct virtio_blk_info {
	le64 total_sector_count;
	le32 segment_size;
	le32 segment_count;
	le16 cylinder_count;
	uchar head_count;
	uchar sector_count;
	uchar block_length;
};

// https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-2500006
struct virtio_blk_req {
	le32 type;
	le32 reserved;
	le64 sector;
	uchar *data;
	uchar status;
};

void virtio_blk_init(void);

#endif // _VIRTIO_BLK_H
