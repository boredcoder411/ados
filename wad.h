#pragma once

#include <stdint.h>
#include "bios.h"

// Struct for WAD header
typedef struct {
    char identifier[4];
    uint32_t num_lumps;
    uint32_t directory_offset;
} __PACKED WADHeader;

// Struct for a lump directory entry
typedef struct {
    uint32_t offset;
    uint32_t size;
    char name[8];
} __PACKED LumpEntry;
