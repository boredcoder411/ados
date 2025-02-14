#include "code16gcc.h"
#include "wad.h"
#include "bios.h"
#include "types.h"
#include <stdint.h>
#include <stdbool.h>

__asm__ ("jmpl  $0, $main\n");

extern void __NORETURN HALT();
extern void install_keyboard();

void __NOINLINE __REGPARM print(const char *s){
  while(*s){
    __asm__ __volatile__ ("int  $0x10" : : "a"(0x0E00 | *s), "b"(7));
    s++;
  }
}

uint8_t __NOINLINE __REGPARM perform_load(const DiskAddressPacket* dap, uint16_t disk_number) {
  uint16_t status;
  __asm__ __volatile__ (
    "int $0x13"
    : "=a"(status)
    : "a"(0x4200), "d"(disk_number), "S"(dap)
    : "cc", "memory"
  );
  return status >> 8; // BIOS places status in AH
}

uint16_t detect_hardware() {
  uint16_t equipment_code;
  // uses int 0x11 to get equipment list
  // and returns the equipment code
  __asm__ __volatile__ (
    "int $0x11"
    : "=a"(equipment_code)
    :
    : "cc"
  );

  return equipment_code;
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

void hexdump(void* data, int size){
  uint8_t* ptr = (uint8_t*)data;
  for (int i = 0; i < size; i++){
    print(hextoa(ptr[i]));
    print(" ");
  }
  print("\r\n");
}

void __NORETURN main() {
  install_keyboard();

  MasterBootRecord* mbr = (MasterBootRecord*)0x7C00;

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
  // from disk to 0x7E00
  DiskAddressPacket dap = {
    .size = 0x10,
    .reserved = 0,
    .num_sectors = 1,
    .offset = 0x7E00,
    .segment = 0,
    .lba = partition->first_lba
  };

  uint8_t status = perform_load(&dap, 0x80);
  if (status != 0) {
    print("Failed to load partition\r\n");
    HALT();
  }

  // check if filesystem is a valid WAD
  WADHeader* wad_header = (WADHeader*)0x7E00;

  if (strncmp(wad_header->identifier, "IWAD", 4) != 0) {
    print("Invalid WAD header\r\n");
    HALT();
  }

  // loop over lumps until you find one called "comd.hex"
  for(uint32_t i = 0; i < wad_header->num_lumps; i++) {
    LumpEntry* lump = (LumpEntry*)(0x7E00 + wad_header->directory_offset + i * sizeof(LumpEntry));

    if(strncmp(lump->name, "comd.hex", 8) == 0) {
      // jump to the start of the lump
      // create a function pointer to the start of the lump
      void (*entry)() = (void (*)())(0x7E00 + lump->offset);
      entry();
      HALT();
    }
  }

  print("comd.hex not found\r\n");
  HALT();

  while(1);
}

