#pragma once

#include <stdint.h>
#include "bios.h"

typedef struct {
  uint8_t size;
  uint8_t reserved;
  uint16_t num_sectors;
  uint16_t offset;
  uint16_t segment;
  uint64_t lba;
} __PACKED DiskAddressPacket;

typedef struct {
  uint8_t status;
  uint8_t first_chs[3];
  uint8_t type;
  uint8_t last_chs[3];
  uint32_t first_lba;
  uint32_t num_sectors;
} __PACKED PartitionTableEntry;

typedef struct {
  uint8_t bootstrap[446];
  PartitionTableEntry partitions[4];
  uint16_t signature;
} __PACKED MasterBootRecord;
