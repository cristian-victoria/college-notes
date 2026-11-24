#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// FAT32 Boot Sector structure (first 512 bytes of image)
typedef struct {
    uint8_t  BS_jmpBoot[3];        // Jump instruction
    uint8_t  BS_OEMName[8];        // OEM Name
    uint16_t BPB_BytsPerSec;       // Bytes per sector (usually 512)
    uint8_t  BPB_SecPerClus;       // Sectors per cluster
    uint16_t BPB_RsvdSecCnt;       // Number of reserved sectors
    uint8_t  BPB_NumFATs;          // Number of FATs (usually 2)
    uint16_t BPB_RootEntCnt;       // Root entry count (0 for FAT32)
    uint16_t BPB_TotSec16;         // Total sectors (0 for FAT32)
    uint8_t  BPB_Media;            // Media descriptor
    uint16_t BPB_FATSz16;          // FAT size in sectors (0 for FAT32)
    uint16_t BPB_SecPerTrk;        // Sectors per track
    uint16_t BPB_NumHeads;         // Number of heads
    uint32_t BPB_HiddSec;          // Hidden sectors
    uint32_t BPB_TotSec32;         // Total sectors (32-bit)
    // FAT32 specific fields
    uint32_t BPB_FATSz32;          // FAT size in sectors (32-bit)
    uint16_t BPB_ExtFlags;         // Extended flags
    uint16_t BPB_FSVer;            // File system version
    uint32_t BPB_RootClus;         // Root directory cluster
    uint16_t BPB_FSInfo;           // FSInfo sector
    uint16_t BPB_BkBootSec;        // Backup boot sector
    uint8_t  BPB_Reserved[12];     // Reserved
    uint8_t  BS_DrvNum;            // Drive number
    uint8_t  BS_Reserved1;         // Reserved
    uint8_t  BS_BootSig;           // Boot signature (0x29)
    uint32_t BS_VolID;             // Volume ID
    uint8_t  BS_VolLab[11];        // Volume label
    uint8_t  BS_FilSysType[8];     // File system type ("FAT32   ")
} __attribute__((packed)) FAT32BootSector;

// FAT32 Directory Entry structure (32 bytes)
typedef struct {
    uint8_t  DIR_Name[11];         // Short name (8.3 format)
    uint8_t  DIR_Attr;             // File attributes
    uint8_t  DIR_NTRes;            // Reserved for Windows NT
    uint8_t  DIR_CrtTimeTenth;     // Creation time, tenths of second
    uint16_t DIR_CrtTime;          // Creation time
    uint16_t DIR_CrtDate;          // Creation date
    uint16_t DIR_LstAccDate;       // Last access date
    uint16_t DIR_FstClusHI;        // High word of first cluster
    uint16_t DIR_WrtTime;          // Last write time
    uint16_t DIR_WrtDate;          // Last write date
    uint16_t DIR_FstClusLO;        // Low word of first cluster
    uint32_t DIR_FileSize;         // File size in bytes
} __attribute__((packed)) FAT32DirectoryEntry;

// Directory entry attributes
#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20
#define ATTR_LONG_NAME  (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

// Special cluster values
#define CLUSTER_FREE    0x00000000
#define CLUSTER_EOC     0x0FFFFFF8  // End of cluster chain (and higher)
#define CLUSTER_BAD     0x0FFFFFF7

// FAT32 file system structure
typedef struct {
    FILE *image_file;
    char *image_name;
    FAT32BootSector boot_sector;
    uint32_t first_data_sector;
    uint32_t data_region_clusters;
    uint32_t fat_entries;
    uint64_t image_size;
    char current_path[256];
    uint32_t current_cluster;      // Current directory cluster
} FAT32FileSystem;

// Function declarations - Part 1
FAT32FileSystem* mount_fat32_image(const char *image_path);
void unmount_fat32_image(FAT32FileSystem *fs);
bool parse_boot_sector(FAT32FileSystem *fs);
void print_info(FAT32FileSystem *fs);
uint32_t get_first_data_sector(FAT32FileSystem *fs);
uint32_t get_data_region_clusters(FAT32FileSystem *fs);
uint32_t get_fat_entries(FAT32FileSystem *fs);

// Function declarations - Part 2
void list_directory(FAT32FileSystem *fs);
bool change_directory(FAT32FileSystem *fs, const char *dirname);
uint32_t get_cluster_offset(FAT32FileSystem *fs, uint32_t cluster);
bool read_cluster(FAT32FileSystem *fs, uint32_t cluster, void *buffer);
uint32_t get_next_cluster(FAT32FileSystem *fs, uint32_t current_cluster);
FAT32DirectoryEntry* find_directory_entry(FAT32FileSystem *fs, uint32_t dir_cluster, const char *name);
void format_directory_name(const uint8_t *dir_name, char *output);
uint32_t get_entry_cluster(FAT32DirectoryEntry *entry);
bool is_directory(FAT32DirectoryEntry *entry);
bool is_valid_entry(FAT32DirectoryEntry *entry);

// Function declarations - Part 3
bool make_directory(FAT32FileSystem *fs, const char *dirname);
bool create_file(FAT32FileSystem *fs, const char *filename);
uint32_t allocate_cluster(FAT32FileSystem *fs);
bool write_cluster(FAT32FileSystem *fs, uint32_t cluster, const void *buffer);
bool update_fat_entry(FAT32FileSystem *fs, uint32_t cluster, uint32_t value);
bool add_directory_entry(FAT32FileSystem *fs, uint32_t dir_cluster, FAT32DirectoryEntry *entry);
void create_dot_entries(FAT32FileSystem *fs, uint32_t new_cluster, uint32_t parent_cluster);
void convert_to_fat32_name(const char *name, uint8_t *fat32_name);
bool entry_exists(FAT32FileSystem *fs, uint32_t dir_cluster, const char *name);
void get_current_datetime(uint16_t *date, uint16_t *time);