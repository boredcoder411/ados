#include "code16gcc.h"
#include "wad.h"
#include "bios.h"
#include <stdint.h>

__asm__ ("jmpl  $0, $main\n");

extern void __NORETURN HALT();

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

char* hextoa(uint8_t hex){
  static char buf[3];
  buf[0] = "0123456789ABCDEF"[hex >> 4];
  buf[1] = "0123456789ABCDEF"[hex & 0xF];
  buf[2] = 0;
  return buf;
}

int strncmp(const char* a, const char* b, int n){
  for (int i = 0; i < n; i++){
    if (a[i] != b[i]){
      return 1;
    }
  }
  return 0;
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
    HALT();
  }

  MasterBootRecord* mbr = (MasterBootRecord*)0x7E00;

  // check if the mbr is valid
  if (mbr->signature != 0xAA55) {
    print("Invalid MBR signature\r\n");
    HALT();
  }

  // loop over partitions until you find one that is not empty
  PartitionTableEntry* partition = mbr->partitions;
  for (int i = 0; i < 4; i++) {
    if (partition->status != 0x80) {
      break;
    }
    partition++;
  }

  // read the partition into memory
  // from disk to 0x7F00
  dap.lba = partition->first_lba;
  dap.num_sectors = partition->num_sectors;
  dap.offset = 0x7F00;
  status = perform_load(&dap, 0x80);

  // check if we found a valid partition
  // should start with IWAD at first 4 bytes
  // so cast it to a wad header and check
  WADHeader* wad = (WADHeader*)0x7F00;

  // check if the partition is valid
  if (strncmp(wad->identifier, "IWAD", 4) != 0) {
    print("Invalid WAD header\r\n");
    HALT();
  }

  // showing some data about the filesystem
  print("WAD found:\r\n");
  print(wad->identifier);
  print(hextoa(wad->num_lumps));
  print(hextoa(wad->directory_offset));

  // loop over the lumps
  // print the name of the lump
  for (int i = 0; i < wad->num_lumps; i++) {
    LumpEntry* lump = (LumpEntry*)(0x7F00 + wad->directory_offset + i * sizeof(LumpEntry));
    print(lump->name);
    print("\r\n");
  }

  // todo:
  // - read the filesystem
  // - be able to load a file and drop it

  HALT(); 
}

