#include "code16gcc.h"
#include <stdint.h>

__asm__ ("jmpl  $0, $main\n");

#define __NOINLINE  __attribute__((noinline))
#define __REGPARM   __attribute__ ((regparm(3)))
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

void __NOINLINE __REGPARM print(const char *s){
  while(*s){
    __asm__ __volatile__ ("int  $0x10" : : "a"(0x0E00 | *s), "b"(7));
    s++;
  }
}

DiskAddressPacket create_disk_address_packet(
    uint64_t start_lba,
    uint16_t number_of_sectors,
    uint16_t target_offset,
    uint16_t target_segment
) {
    DiskAddressPacket dap;
    dap.size = 0x10;
    dap.reserved = 0;
    dap.num_sectors = number_of_sectors;
    dap.offset = target_offset;
    dap.segment = target_segment;
    dap.lba = start_lba;
    return dap;
}

void perform_load(const DiskAddressPacket* dap, uint16_t disk_number) {
    uint16_t self_addr = (uint16_t)(uintptr_t)dap; // Cast pointer to 16-bit segment offset

    __asm__ __volatile__ (
        "int $0x13"
        : // No output
        : "a"(0x4200 | dap->size), "b"(disk_number), "d"(self_addr)
        : "cc"
    );
}


void __NORETURN main(){
  // load the second sector of the disk to 0x7E00
  DiskAddressPacket dap = create_disk_address_packet(1, 1, 0x7E00, 0);
  perform_load(&dap, 0x80);

  print("h");

  while(1);
}
