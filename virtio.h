#ifndef _VIRTIO_H
#define _VIRTIO_H

#include "types.h"
#include "defs.h"
#include "pci.h"

#define VIRTIO_VENDOR 0x1AF4

void virtio_init(void);

#endif // _VIRTIO_H
