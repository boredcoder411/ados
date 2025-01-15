#include "code16gcc.h"
#include <stdint.h>

__asm__ ("jmpl  $0, $main\n");

#define __NOINLINE  __attribute__((noinline))
#define __REGPARM   __attribute__((regparm(3)))
#define __NORETURN  __attribute__((noreturn))
#define __PACKED    __attribute__((packed))

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

void __NOINLINE __REGPARM print(const char *s){
  while(*s){
    __asm__ __volatile__ ("int  $0x10" : : "a"(0x0E00 | *s), "b"(7));
    s++;
  }
}

uint8_t __NOINLINE __REGPARM perform_load(const DiskAddressPacket* dap, uint16_t disk_number) {
  uint8_t status;
  __asm__ __volatile__ (
    "int $0x13"
    : "=a"(status)
    : "a"(0x4200), "d"(disk_number), "S"(dap)
    : "cc", "memory"
  );
  return status >> 8; // BIOS places status in AH
}

void __NORETURN main(){
  DiskAddressPacket dap = {
    .size = 0x10,
    .reserved = 0,
    .num_sectors = 1,
    .offset = 0x7E00,
    .segment = 0,
    .lba = 0
  };

  uint8_t status = perform_load(&dap, 0x80);
  if (status != 0) {
    print("Disk read error\r\n");
    while (1);
  }

  MasterBootRecord* mbr = (MasterBootRecord*)0x7E00;

  // check if the mbr is valid
  if (mbr->signature != 0xAA55) {
    print("Invalid MBR signature\r\n");
    while (1);
  }

  // todo:
  // - read the first partition
  // - find a filesystem in the partition
  // - read the filesystem
  // - be able to load a file and drop it

  while (1);
}

