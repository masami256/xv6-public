#ifndef _VIRTIO_BLK_H
#define _VIRTIO_BLK_H

#include "virtio.h"

struct virtio_blk {
	le64 total_sector_count;
	le32 segment_size;
	le32 segment_count;
	le16 cylinder_count;
	uchar head_count;
	uchar sector_count;
	uchar block_length;
};

void virtio_blk_init(void);

#endif // _VIRTIO_BLK_H
