#include "fat32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

//=================================
// PART 1 Implementation
//=================================

// Opens and validates FAT32 image
FAT32FileSystem* mount_fat32_image(const char *image_path) {
    // Check if file exists
    struct stat st;
    if (stat(image_path, &st) != 0) {
        fprintf(stderr, "Error: File '%s' does not exist.\n", image_path);
        return NULL;
    }

    // Allocate memory for file system structure
    FAT32FileSystem *fs = (FAT32FileSystem *)malloc(sizeof(FAT32FileSystem));
    if (!fs) {
        fprintf(stderr, "Error: Failed to allocate memory for file system.\n");
        return NULL;
    }

    // Open the image file
    fs->image_file = fopen(image_path, "r+b");
    if (!fs->image_file) {
        fprintf(stderr, "Error: Failed to open image file '%s'.\n", image_path);
        free(fs);
        return NULL;
    }

    // Store image name (extract from path)
    const char *name_start = strrchr(image_path, '/');
    if (name_start) {
        name_start++; // Skip the '/'
    } else {
        name_start = image_path;
    }
    fs->image_name = strdup(name_start);

    // Get image size
    fseek(fs->image_file, 0, SEEK_END);
    fs->image_size = ftell(fs->image_file);
    fseek(fs->image_file, 0, SEEK_SET);

    // Parse boot sector
    if (!parse_boot_sector(fs)) {
        fprintf(stderr, "Error: Failed to parse boot sector.\n");
        fclose(fs->image_file);
        free(fs->image_name);
        free(fs);
        return NULL;
    }

    // Initialize current path to root
    strcpy(fs->current_path, "/");
    fs->current_cluster = fs->boot_sector.BPB_RootClus;

    return fs;
}

// Closes the FAT32 image and frees resources
void unmount_fat32_image(FAT32FileSystem *fs) {
    if (!fs) return;
    
    if (fs->image_file) {
        fclose(fs->image_file);
    }
    if (fs->image_name) {
        free(fs->image_name);
    }
    free(fs);
}

// Reads and validates the boot sector
bool parse_boot_sector(FAT32FileSystem *fs) {
    if (!fs || !fs->image_file) {
        return false;
    }

    // Read boot sector
    fseek(fs->image_file, 0, SEEK_SET);
    size_t bytes_read = fread(&fs->boot_sector, sizeof(FAT32BootSector), 1, fs->image_file);
    
    if (bytes_read != 1) {
        fprintf(stderr, "Error: Failed to read boot sector.\n");
        return false;
    }

    // Verify it's a FAT32 file system
    if (fs->boot_sector.BPB_FATSz16 != 0) {
        fprintf(stderr, "Error: Not a FAT32 file system (BPB_FATSz16 should be 0).\n");
        return false;
    }

    if (fs->boot_sector.BPB_RootEntCnt != 0) {
        fprintf(stderr, "Error: Not a FAT32 file system (BPB_RootEntCnt should be 0).\n");
        return false;
    }

    // Calculate derived values
    fs->first_data_sector = get_first_data_sector(fs);
    fs->data_region_clusters = get_data_region_clusters(fs);
    fs->fat_entries = get_fat_entries(fs);

    return true;
}

// Calculates the first data sector
uint32_t get_first_data_sector(FAT32FileSystem *fs) {
    uint32_t RootDirSectors = ((fs->boot_sector.BPB_RootEntCnt * 32) + 
                               (fs->boot_sector.BPB_BytsPerSec - 1)) / 
                               fs->boot_sector.BPB_BytsPerSec;
    
    uint32_t FATSz = (fs->boot_sector.BPB_FATSz16 != 0) ? 
                     fs->boot_sector.BPB_FATSz16 : 
                     fs->boot_sector.BPB_FATSz32;
    
    return fs->boot_sector.BPB_RsvdSecCnt + 
           (fs->boot_sector.BPB_NumFATs * FATSz) + 
           RootDirSectors;
}

// Calculates total clusters in data region
uint32_t get_data_region_clusters(FAT32FileSystem *fs) {
    uint32_t TotSec = (fs->boot_sector.BPB_TotSec16 != 0) ? 
                      fs->boot_sector.BPB_TotSec16 : 
                      fs->boot_sector.BPB_TotSec32;
    
    uint32_t DataSec = TotSec - fs->first_data_sector;
    uint32_t CountofClusters = DataSec / fs->boot_sector.BPB_SecPerClus;
    
    return CountofClusters;
}

// Calculates FAT table size
uint32_t get_fat_entries(FAT32FileSystem *fs) {
    uint32_t FATSz = fs->boot_sector.BPB_FATSz32;
    uint32_t BytsPerSec = fs->boot_sector.BPB_BytsPerSec;
    
    // Each FAT entry is 4 bytes in FAT32
    return (FATSz * BytsPerSec) / 4;
}

// Prints boot sector information
void print_info(FAT32FileSystem *fs) {
    if (!fs) {
        fprintf(stderr, "Error: File system not mounted.\n");
        return;
    }

    printf("\n");
    printf("Position of root cluster: %u\n", fs->boot_sector.BPB_RootClus);
    printf("Bytes per sector: %u\n", fs->boot_sector.BPB_BytsPerSec);
    printf("Sectors per cluster: %u\n", fs->boot_sector.BPB_SecPerClus);
    printf("Total # of clusters in data region: %u\n", fs->data_region_clusters);
    printf("# of entries in one FAT: %u\n", fs->fat_entries);
    printf("Size of image (in bytes): %lu\n", fs->image_size);
    printf("\n");
}

//=================================
// PART 2 Implementation
//=================================

// Gets the byte offset of a cluster in the image file
uint32_t get_cluster_offset(FAT32FileSystem *fs, uint32_t cluster) {
    // Cluster numbers start at 2
    if (cluster < 2) {
        return 0;
    }
    
    uint32_t cluster_offset = cluster - 2;
    uint32_t cluster_size = fs->boot_sector.BPB_SecPerClus * fs->boot_sector.BPB_BytsPerSec;
    uint32_t data_offset = fs->first_data_sector * fs->boot_sector.BPB_BytsPerSec;
    
    return data_offset + (cluster_offset * cluster_size);
}

// Reads a cluster from the image file into buffer
bool read_cluster(FAT32FileSystem *fs, uint32_t cluster, void *buffer) {
    if (!fs || !buffer || cluster < 2) {
        return false;
    }
    
    uint32_t offset = get_cluster_offset(fs, cluster);
    uint32_t cluster_size = fs->boot_sector.BPB_SecPerClus * fs->boot_sector.BPB_BytsPerSec;
    
    fseek(fs->image_file, offset, SEEK_SET);
    size_t bytes_read = fread(buffer, 1, cluster_size, fs->image_file);
    
    return bytes_read == cluster_size;
}

// Gets the next cluster in the FAT chain
uint32_t get_next_cluster(FAT32FileSystem *fs, uint32_t current_cluster) {
    if (!fs || current_cluster < 2) {
        return CLUSTER_EOC;
    }
    
    // Calculate FAT offset
    uint32_t fat_offset = fs->boot_sector.BPB_RsvdSecCnt * fs->boot_sector.BPB_BytsPerSec;
    uint32_t fat_entry_offset = current_cluster * 4;
    
    // Read the FAT entry
    uint32_t next_cluster;
    fseek(fs->image_file, fat_offset + fat_entry_offset, SEEK_SET);
    fread(&next_cluster, sizeof(uint32_t), 1, fs->image_file);
    
    // Mask off the upper 4 bits (they're reserved)
    next_cluster &= 0x0FFFFFFF;
    
    return next_cluster;
}

// Gets the first cluster number from a directory entry
uint32_t get_entry_cluster(FAT32DirectoryEntry *entry) {
    return ((uint32_t)entry->DIR_FstClusHI << 16) | entry->DIR_FstClusLO;
}

// Checks if a directory entry is a directory
bool is_directory(FAT32DirectoryEntry *entry) {
    return (entry->DIR_Attr & ATTR_DIRECTORY) != 0;
}

// Checks if a directory entry is valid (not deleted, not volume label, etc.)
bool is_valid_entry(FAT32DirectoryEntry *entry) {
    // Check if deleted (first byte is 0xE5)
    if (entry->DIR_Name[0] == 0xE5) {
        return false;
    }
    
    // Check if end of directory (first byte is 0x00)
    if (entry->DIR_Name[0] == 0x00) {
        return false;
    }
    
    // Skip long file name entries
    if ((entry->DIR_Attr & ATTR_LONG_NAME) == ATTR_LONG_NAME) {
        return false;
    }
    
    // Skip volume labels (unless it's also a directory, which shouldn't happen)
    if ((entry->DIR_Attr & ATTR_VOLUME_ID) && !(entry->DIR_Attr & ATTR_DIRECTORY)) {
        return false;
    }
    
    return true;
}

// Formats a directory name from the 8.3 format to a readable string
void format_directory_name(const uint8_t *dir_name, char *output) {
    // Handle "." and ".."
    if (dir_name[0] == '.' && dir_name[1] == ' ') {
        strcpy(output, ".");
        return;
    }
    if (dir_name[0] == '.' && dir_name[1] == '.' && dir_name[2] == ' ') {
        strcpy(output, "..");
        return;
    }
    
    // Extract name part (first 8 bytes)
    int name_len = 0;
    for (int i = 0; i < 8 && dir_name[i] != ' '; i++) {
        output[name_len++] = dir_name[i];
    }
    
    // Extract extension part (last 3 bytes)
    int ext_start = 8;
    if (dir_name[ext_start] != ' ') {
        output[name_len++] = '.';
        for (int i = 0; i < 3 && dir_name[ext_start + i] != ' '; i++) {
            output[name_len++] = dir_name[ext_start + i];
        }
    }
    
    output[name_len] = '\0';
}

// Finds a directory entry by name in the specified directory cluster
FAT32DirectoryEntry* find_directory_entry(FAT32FileSystem *fs, uint32_t dir_cluster, const char *name) {
    if (!fs || !name) {
        return NULL;
    }
    
    uint32_t cluster_size = fs->boot_sector.BPB_SecPerClus * fs->boot_sector.BPB_BytsPerSec;
    uint8_t *cluster_buffer = malloc(cluster_size);
    if (!cluster_buffer) {
        return NULL;
    }
    
    uint32_t current_cluster = dir_cluster;
    static FAT32DirectoryEntry found_entry; // Static so it persists after function return
    
    // Traverse the cluster chain
    while (current_cluster >= 2 && current_cluster < CLUSTER_EOC) {
        if (!read_cluster(fs, current_cluster, cluster_buffer)) {
            free(cluster_buffer);
            return NULL;
        }
        
        // Each cluster contains multiple directory entries
        uint32_t entries_per_cluster = cluster_size / sizeof(FAT32DirectoryEntry);
        FAT32DirectoryEntry *entries = (FAT32DirectoryEntry *)cluster_buffer;
        
        for (uint32_t i = 0; i < entries_per_cluster; i++) {
            // End of directory
            if (entries[i].DIR_Name[0] == 0x00) {
                free(cluster_buffer);
                return NULL;
            }
            
            if (!is_valid_entry(&entries[i])) {
                continue;
            }
            
            char entry_name[13];
            format_directory_name(entries[i].DIR_Name, entry_name);
            
            // Compare names (case-insensitive)
            if (strcasecmp(entry_name, name) == 0) {
                memcpy(&found_entry, &entries[i], sizeof(FAT32DirectoryEntry));
                free(cluster_buffer);
                return &found_entry;
            }
        }
        
        current_cluster = get_next_cluster(fs, current_cluster);
    }
    
    free(cluster_buffer);
    return NULL;
}

// Lists all entries in the current directory
void list_directory(FAT32FileSystem *fs) {
    if (!fs) {
        fprintf(stderr, "Error: File system not mounted.\n");
        return;
    }
    
    uint32_t cluster_size = fs->boot_sector.BPB_SecPerClus * fs->boot_sector.BPB_BytsPerSec;
    uint8_t *cluster_buffer = malloc(cluster_size);
    if (!cluster_buffer) {
        fprintf(stderr, "Error: Failed to allocate memory.\n");
        return;
    }
    
    uint32_t current_cluster = fs->current_cluster;
    
    // Traverse the cluster chain
    while (current_cluster >= 2 && current_cluster < CLUSTER_EOC) {
        if (!read_cluster(fs, current_cluster, cluster_buffer)) {
            fprintf(stderr, "Error: Failed to read directory cluster.\n");
            free(cluster_buffer);
            return;
        }
        
        // Each cluster contains multiple directory entries
        uint32_t entries_per_cluster = cluster_size / sizeof(FAT32DirectoryEntry);
        FAT32DirectoryEntry *entries = (FAT32DirectoryEntry *)cluster_buffer;
        
        for (uint32_t i = 0; i < entries_per_cluster; i++) {
            // End of directory
            if (entries[i].DIR_Name[0] == 0x00) {
                free(cluster_buffer);
                return;
            }
            
            if (!is_valid_entry(&entries[i])) {
                continue;
            }
            
            char entry_name[13];
            format_directory_name(entries[i].DIR_Name, entry_name);
            printf("%s\n", entry_name);
        }
        
        current_cluster = get_next_cluster(fs, current_cluster);
    }
    
    free(cluster_buffer);
}

// Changes the current directory to the specified directory name
bool change_directory(FAT32FileSystem *fs, const char *dirname) {
    if (!fs || !dirname) {
        return false;
    }
    
    // Handle root directory
    if (strcmp(dirname, "/") == 0) {
        fs->current_cluster = fs->boot_sector.BPB_RootClus;
        strcpy(fs->current_path, "/");
        return true;
    }
    
    // Find the directory entry
    FAT32DirectoryEntry *entry = find_directory_entry(fs, fs->current_cluster, dirname);
    
    if (!entry) {
        fprintf(stderr, "Error: Directory '%s' does not exist.\n", dirname);
        return false;
    }
    
    if (!is_directory(entry)) {
        fprintf(stderr, "Error: '%s' is not a directory.\n", dirname);
        return false;
    }
    
    // Update current cluster
    uint32_t new_cluster = get_entry_cluster(entry);
    fs->current_cluster = new_cluster;
    
    // Update path
    if (strcmp(dirname, "..") == 0) {
        // Go up one directory
        char *last_slash = strrchr(fs->current_path, '/');
        if (last_slash && last_slash != fs->current_path) {
            *last_slash = '\0';
        } else {
            strcpy(fs->current_path, "/");
        }
        
        // If we're back at root, use the actual root cluster
        if (strcmp(fs->current_path, "/") == 0) {
            fs->current_cluster = fs->boot_sector.BPB_RootClus;
        }
    } else if (strcmp(dirname, ".") != 0) {
        // Don't change path for "."
        // Add directory to path
        if (strcmp(fs->current_path, "/") != 0) {
            strcat(fs->current_path, "/");
        }
        strcat(fs->current_path, dirname);
    }
    
    return true;
}

//=================================
// PART 3 Implementation
//=================================

// Gets current date and time in FAT format
void get_current_datetime(uint16_t *date, uint16_t *time) {
    // For simplicity, use a fixed date/time
    // FAT Date: bits 0-4=day, 5-8=month, 9-15=year from 1980
    // FAT Time: bits 0-4=seconds/2, 5-10=minutes, 11-15=hours
    
    // January 1, 2024 = (2024-1980) << 9 | 1 << 5 | 1
    *date = (44 << 9) | (1 << 5) | 1;  // 2024-01-01
    
    // 12:00:00 = 12 << 11 | 0 << 5 | 0
    *time = (12 << 11);  // 12:00:00
}

// Converts a filename to FAT32 8.3 format
void convert_to_fat32_name(const char *name, uint8_t *fat32_name) {
    // Initialize with spaces
    memset(fat32_name, ' ', 11);
    
    // Find the dot (if any)
    const char *dot = strchr(name, '.');
    
    int name_len = dot ? (int)(dot - name) : (int)strlen(name);
    int ext_len = dot ? (int)strlen(dot + 1) : 0;
    
    // Copy name part (max 8 chars)
    for (int i = 0; i < name_len && i < 8; i++) {
        fat32_name[i] = toupper((unsigned char)name[i]);
    }
    
    // Copy extension part (max 3 chars)
    if (dot && ext_len > 0) {
        for (int i = 0; i < ext_len && i < 3; i++) {
            fat32_name[8 + i] = toupper((unsigned char)dot[1 + i]);
        }
    }
}

// Checks if an entry with the given name already exists
bool entry_exists(FAT32FileSystem *fs, uint32_t dir_cluster, const char *name) {
    FAT32DirectoryEntry *entry = find_directory_entry(fs, dir_cluster, name);
    return entry != NULL;
}

// Allocates a free cluster from the FAT table
uint32_t allocate_cluster(FAT32FileSystem *fs) {
    if (!fs) {
        return 0;
    }
    
    uint32_t fat_offset = fs->boot_sector.BPB_RsvdSecCnt * fs->boot_sector.BPB_BytsPerSec;
    
    // Search for a free cluster (starting from cluster 2)
    for (uint32_t cluster = 2; cluster < fs->data_region_clusters + 2; cluster++) {
        uint32_t fat_entry_offset = cluster * 4;
        uint32_t entry_value;
        
        // Read FAT entry
        fseek(fs->image_file, fat_offset + fat_entry_offset, SEEK_SET);
        fread(&entry_value, sizeof(uint32_t), 1, fs->image_file);
        entry_value &= 0x0FFFFFFF;
        
        // Check if free
        if (entry_value == CLUSTER_FREE) {
            // Mark as end of chain
            update_fat_entry(fs, cluster, CLUSTER_EOC);
            return cluster;
        }
    }
    
    fprintf(stderr, "Error: No free clusters available.\n");
    return 0;
}

// Updates a FAT entry with a new value
bool update_fat_entry(FAT32FileSystem *fs, uint32_t cluster, uint32_t value) {
    if (!fs || cluster < 2) {
        return false;
    }
    
    // Mask to 28 bits
    value &= 0x0FFFFFFF;
    
    uint32_t fat_offset = fs->boot_sector.BPB_RsvdSecCnt * fs->boot_sector.BPB_BytsPerSec;
    uint32_t fat_size = fs->boot_sector.BPB_FATSz32 * fs->boot_sector.BPB_BytsPerSec;
    uint32_t fat_entry_offset = cluster * 4;
    
    // Update first FAT
    fseek(fs->image_file, fat_offset + fat_entry_offset, SEEK_SET);
    fwrite(&value, sizeof(uint32_t), 1, fs->image_file);
    
    // Update second FAT (mirror)
    fseek(fs->image_file, fat_offset + fat_size + fat_entry_offset, SEEK_SET);
    fwrite(&value, sizeof(uint32_t), 1, fs->image_file);
    
    fflush(fs->image_file);
    
    return true;
}

// Writes data to a cluster
bool write_cluster(FAT32FileSystem *fs, uint32_t cluster, const void *buffer) {
    if (!fs || !buffer || cluster < 2) {
        return false;
    }
    
    uint32_t offset = get_cluster_offset(fs, cluster);
    uint32_t cluster_size = fs->boot_sector.BPB_SecPerClus * fs->boot_sector.BPB_BytsPerSec;
    
    fseek(fs->image_file, offset, SEEK_SET);
    size_t bytes_written = fwrite(buffer, 1, cluster_size, fs->image_file);
    fflush(fs->image_file);
    
    return bytes_written == cluster_size;
}

// Creates "." and ".." entries for a new directory
void create_dot_entries(FAT32FileSystem *fs, uint32_t new_cluster, uint32_t parent_cluster) {
    uint32_t cluster_size = fs->boot_sector.BPB_SecPerClus * fs->boot_sector.BPB_BytsPerSec;
    uint8_t *cluster_buffer = calloc(1, cluster_size);
    
    if (!cluster_buffer) {
        return;
    }
    
    FAT32DirectoryEntry *entries = (FAT32DirectoryEntry *)cluster_buffer;
    uint16_t date, time;
    get_current_datetime(&date, &time);
    
    // Create "." entry (points to self)
    memset(&entries[0], 0, sizeof(FAT32DirectoryEntry));
    memcpy(entries[0].DIR_Name, ".          ", 11);
    entries[0].DIR_Attr = ATTR_DIRECTORY;
    entries[0].DIR_FstClusHI = (new_cluster >> 16) & 0xFFFF;
    entries[0].DIR_FstClusLO = new_cluster & 0xFFFF;
    entries[0].DIR_CrtDate = date;
    entries[0].DIR_CrtTime = time;
    entries[0].DIR_WrtDate = date;
    entries[0].DIR_WrtTime = time;
    
    // Create ".." entry (points to parent)
    // Special case: if parent is root, use cluster 0
    uint32_t parent_for_dotdot = (parent_cluster == fs->boot_sector.BPB_RootClus) ? 0 : parent_cluster;
    
    memset(&entries[1], 0, sizeof(FAT32DirectoryEntry));
    memcpy(entries[1].DIR_Name, "..         ", 11);
    entries[1].DIR_Attr = ATTR_DIRECTORY;
    entries[1].DIR_FstClusHI = (parent_for_dotdot >> 16) & 0xFFFF;
    entries[1].DIR_FstClusLO = parent_for_dotdot & 0xFFFF;
    entries[1].DIR_CrtDate = date;
    entries[1].DIR_CrtTime = time;
    entries[1].DIR_WrtDate = date;
    entries[1].DIR_WrtTime = time;
    
    // Write the cluster
    write_cluster(fs, new_cluster, cluster_buffer);
    
    free(cluster_buffer);
}

// Adds a directory entry to the specified directory cluster
bool add_directory_entry(FAT32FileSystem *fs, uint32_t dir_cluster, FAT32DirectoryEntry *entry) {
    if (!fs || !entry) {
        return false;
    }
    
    uint32_t cluster_size = fs->boot_sector.BPB_SecPerClus * fs->boot_sector.BPB_BytsPerSec;
    uint8_t *cluster_buffer = malloc(cluster_size);
    if (!cluster_buffer) {
        return false;
    }
    
    uint32_t current_cluster = dir_cluster;
    
    // Traverse cluster chain looking for empty slot
    while (current_cluster >= 2 && current_cluster < CLUSTER_EOC) {
        if (!read_cluster(fs, current_cluster, cluster_buffer)) {
            free(cluster_buffer);
            return false;
        }
        
        uint32_t entries_per_cluster = cluster_size / sizeof(FAT32DirectoryEntry);
        FAT32DirectoryEntry *entries = (FAT32DirectoryEntry *)cluster_buffer;
        
        // Look for empty slot
        for (uint32_t i = 0; i < entries_per_cluster; i++) {
            if (entries[i].DIR_Name[0] == 0x00 || entries[i].DIR_Name[0] == 0xE5) {
                // Found empty slot
                memcpy(&entries[i], entry, sizeof(FAT32DirectoryEntry));
                write_cluster(fs, current_cluster, cluster_buffer);
                free(cluster_buffer);
                return true;
            }
        }
        
        // Move to next cluster in chain
        uint32_t next_cluster = get_next_cluster(fs, current_cluster);
        
        // If end of chain, allocate new cluster
        if (next_cluster >= CLUSTER_EOC) {
            uint32_t new_cluster = allocate_cluster(fs);
            if (new_cluster == 0) {
                free(cluster_buffer);
                return false;
            }
            
            // Link current cluster to new cluster
            update_fat_entry(fs, current_cluster, new_cluster);
            
            // Initialize new cluster
            memset(cluster_buffer, 0, cluster_size);
            memcpy(&((FAT32DirectoryEntry *)cluster_buffer)[0], entry, sizeof(FAT32DirectoryEntry));
            write_cluster(fs, new_cluster, cluster_buffer);
            free(cluster_buffer);
            return true;
        }
        
        current_cluster = next_cluster;
    }
    
    free(cluster_buffer);
    return false;
}

// Creates a new directory in the current working directory
bool make_directory(FAT32FileSystem *fs, const char *dirname) {
    if (!fs || !dirname) {
        return false;
    }
    
    // Check if name already exists
    if (entry_exists(fs, fs->current_cluster, dirname)) {
        fprintf(stderr, "Error: A file or directory named '%s' already exists.\n", dirname);
        return false;
    }
    
    // Allocate a cluster for the new directory
    uint32_t new_cluster = allocate_cluster(fs);
    if (new_cluster == 0) {
        fprintf(stderr, "Error: Failed to allocate cluster for directory.\n");
        return false;
    }
    
    // Create the directory entry
    FAT32DirectoryEntry entry;
    memset(&entry, 0, sizeof(FAT32DirectoryEntry));
    
    convert_to_fat32_name(dirname, entry.DIR_Name);
    entry.DIR_Attr = ATTR_DIRECTORY;
    
    uint16_t date, time;
    get_current_datetime(&date, &time);
    entry.DIR_CrtDate = date;
    entry.DIR_CrtTime = time;
    entry.DIR_WrtDate = date;
    entry.DIR_WrtTime = time;
    
    entry.DIR_FstClusHI = (new_cluster >> 16) & 0xFFFF;
    entry.DIR_FstClusLO = new_cluster & 0xFFFF;
    entry.DIR_FileSize = 0;
    
    // Add entry to parent directory
    if (!add_directory_entry(fs, fs->current_cluster, &entry)) {
        fprintf(stderr, "Error: Failed to add directory entry.\n");
        return false;
    }
    
    // Create "." and ".." entries in the new directory
    create_dot_entries(fs, new_cluster, fs->current_cluster);
    
    return true;
}

// Creates a new file in the current working directory with size 0
bool create_file(FAT32FileSystem *fs, const char *filename) {
    if (!fs || !filename) {
        return false;
    }
    
    // Check if name already exists
    if (entry_exists(fs, fs->current_cluster, filename)) {
        fprintf(stderr, "Error: A file or directory named '%s' already exists.\n", filename);
        return false;
    }
    
    // Create the directory entry (no cluster allocated for 0-byte file)
    FAT32DirectoryEntry entry;
    memset(&entry, 0, sizeof(FAT32DirectoryEntry));
    
    convert_to_fat32_name(filename, entry.DIR_Name);
    entry.DIR_Attr = ATTR_ARCHIVE;  // Regular file
    
    uint16_t date, time;
    get_current_datetime(&date, &time);
    entry.DIR_CrtDate = date;
    entry.DIR_CrtTime = time;
    entry.DIR_WrtDate = date;
    entry.DIR_WrtTime = time;
    
    // 0-byte file has no cluster
    entry.DIR_FstClusHI = 0;
    entry.DIR_FstClusLO = 0;
    entry.DIR_FileSize = 0;
    
    // Add entry to current directory
    if (!add_directory_entry(fs, fs->current_cluster, &entry)) {
        fprintf(stderr, "Error: Failed to add file entry.\n");
        return false;
    }
    
    return true;
}