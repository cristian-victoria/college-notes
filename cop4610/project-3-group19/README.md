# FAT32 File System Utility

This project delves into the intricacies of the FAT32 file system, including its cluster-based storage, FAT tables, sectors, and directory structure, with the purpose of designing and implementing a user-space, shell-like utility capable of interpreting FAT32 file system images. 

## Group Members
- **Cristian Victoria**: cjv22c@fsu.edu
- **Guillermo Marr**: @fsu.edu

## Division of Labor

### Part 1: Mounting the Image
- **Responsibilities**: Implement functionality to greet the user and allow him or her to read the image file, store it, and navigate it, and output any error messages if they occur.
- **Assigned to**: Cristian Victoria

### Part 2: Navigation
- **Responsibilities**: Implement the `cd` and `ls` commands.
- **Assigned to**: Cristian Victoria

### Part 3: Create
- **Responsibilities**: Implement the `mkdir` and `creat` commands.
- **Assigned to**: Cristian Victoria

### Part 4: Read
- **Responsibilities**: [Description]
- **Assigned to**: Guillermo Marr

### Part 5: Update
- **Responsibilities**: [Description]
- **Assigned to**: Guillermo Marr

### Part 6: Delete
- **Responsibilities**: [Description]
- **Assigned to**: Guillermo Marr

## File Listing
```
project-3-group19/
│
├── src/
│ └── fat32.c
│ ├── lexer.c
│ └── main.c
│
├── include/
│ └── fat32.h
│ └── lexer.h
│
├── obj/
│ └── (output_files.o)
│
├── filesys     # program executable
├── test.img    # test image to use on program
├── README.md
└── Makefile
```
## How to Compile & Execute

### Requirements
- **Compiler**: `gcc` for C

- **Dependencies**: 
- For Mac Users:
    - Install Homebrew (if not installed already)
    - Once Homebrew is installed, use the following command to install the dosfstools package: `brew install dosfstools`
        - This will be used to get the mkfs.vat command which will allow us to create a File Allocation Table (FAT) filesystem within an image file.  

### Compilation
Using C:
```bash
make
```
This will build the executable 'filesys'

### Execution
```bash
./filesys (your_FAT32image.img)
```
This will run the program.

If you do not have a FAT32 image, create one on Linux/Mac. We performed the following code to create the main test image we used to debug our program. 
```bash
# Create a 50MB empty file
dd if=/dev/zero of=test.img bs=1m count=50

# Atach it to a device for formatting
hdiutil attach -imagekey diskimage-class=CRawDiskImage -nomount test.img

# Format it as a FAT32
sudo newfs_msdos -F 32 -v TEST /dev/disk5

# Detach the device
hdiutil detach /dev/disk5

# Mount the image to add files
hdiutil attach test.img

# Create test directories and files
mkdir /Volumes/TEST/FOLDER1
mkdir /Volumes/TEST/FOLDER2
echo "test content" > /Volumes/TEST/FILE.TXT
mkdir /Volumes/TEST/FOLDER1/SUBFOLDER
echo "subfolder data" > /Volumes/TEST/FOLDER1/SUBFOLDER/DATA.TXT

# Unmount
hdiutil detach /Volumes/TEST

# Test the program
./filesys test.img
```

Try the following commands:
```bash
your_FAT32image.img/>info
your_FAT32image.img/>ls
your_FAT32image.img/>cd <directory>
your_FAT32image.img/>cd ..
your_FAT32image.img/>mkdir <new_directory>
your_FAT32image.img/>ls
your_FAT32image.img/>cd <new_directory>
your_FAT32image.img/>create <new_file>
your_FAT32image.img/>ls
your_FAT32image.img/>exit
```

### Bugs
- **Bug1**: (Cristian) When I would perfom the `ls` command, the directories would not show. The problem was that in my mount_fat32_image() function in fat32.c, I was not initializing current_cluster. My solution was to add the following code: 
```bash
fs->current_cluster = fs->boot_sector.BPB_RootClus;
```
Without initialization, current_cluster would have a random/garbage value (probably empty). 

- **Bug2**: (Cristian) When I would perfom the `cd ..` command, the path would update correctly but the code was using the cluster from the ".." directory entry, instead of pointing for the actual root cluster for the root diretory. My solution was to add the following code in my change_directory() function: 
```bash
if (strcmp(fs->current_path, "/") == 0) {
    fs->current_cluster = fs->boot_sector.BPB_RootClus;
}
```