#pragma once

#include <stdint.h>
#include <stdlib.h>

void list_wad_files(const char* wad_file_path);
int save_to_file(const char* file_path, const uint8_t* data, size_t size);
void show_wad_file(const char* wad_file_path, const char* lump_name);

// Struct for WAD header
typedef struct {
    char identifier[4];
    uint32_t num_lumps;
    uint32_t directory_offset;
} WADHeader;

// Struct for a lump directory entry
typedef struct {
    uint32_t offset;
    uint32_t size;
    char name[8];
} LumpEntry;
