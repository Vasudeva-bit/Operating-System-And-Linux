#include <stdio.h>
#include <stdlib.h>

struct file {
    int startBlockFAT;
    int sizeBytes;
};

struct fatEntry {
    int busy;
    int next;
};

struct FAT {
    int noBlocks;
    struct fatEntry* fileBlock;
};

struct unit {
    int byteCount;
    char bytes[512];
};

struct disk {
    int totUnits;
    int usedUnits;
    int freeUnits;
    struct unit* units;
};

struct block {
    int usedUnits;
    struct unit* units;
};

struct blockedDisk {
    int noBlocks;
    // the base address of our collection of blocks that abstract an existing collection of units
    int noFiles;
    struct block* blocks;
    struct file* files; // files on disk - FAT
};

struct FAT conFAT(int noBlocks) {
    struct FAT temp;
    temp.noBlocks = noBlocks;
    temp.fileBlock = malloc(temp.noBlocks*sizeof(struct fatEntry));
    return temp;
}

// unit constructor
struct unit conUnit() {
    struct unit temp;
    temp.byteCount = 0;
    // byteCount of  unit, implicitly, implies that the unit is empty, hence no need to fill the char array with 0
    return temp;
}

// disk constructor
struct disk conDisk(int bytes) {
    struct disk temp;
    temp.totUnits = bytes/512; // but each unit is of size 516, including the byteCount attribute
    temp.units = malloc(sizeof(struct unit)*temp.totUnits);
    // fill the disk with units (use the unit constructor to initailize all the units of disk)
    for(int i=0;i<temp.totUnits;i++) {
        temp.units[i] = conUnit();
    }
    return temp;
}

struct block conBlock(struct unit* startUnit) { // check pointer here
    struct block temp;
    temp.usedUnits = 0;
    temp.units = startUnit;
    return temp;
}

// representation of file system
struct blockedDisk blockifyDisk(struct disk Disk) {
    struct blockedDisk temp;
    temp.noBlocks = Disk.totUnits/8;
    temp.blocks = malloc(temp.noBlocks*sizeof(struct block));
    temp.files = malloc(temp.noBlocks*sizeof(struct file));
    temp.noFiles = 0;
    int nextUnit = 0;
    // abstracting the disk into blocks (efficient memory representation than any naive unformatted disk, also doesn't bottleneck the # (memory) units)
    for(int i=0;i<temp.noBlocks;i++) {
        temp.blocks[i] = conBlock(&Disk.units[nextUnit]);
        nextUnit += 8;
    }
    return temp;
}

// Sequential Allocation
int AllocateFile(int noBlocks) {
    for(int i=0;i<bDisk.noBlocks-noBlocks;i++) {
        int flag = 1;
        for(int idx=i;idx<noBlocks;idx++) {
            if(fat.fileBlock[idx].busy) {
                flag = 0;
                break;
            }
        }
        if(flag) {
            int idx;
            for(idx=i;idx<i+noBlocks-1;idx++) {
                fat.fileBlock[idx].busy=1;
                fat.fileBlock[idx].next=idx+1;
            }
            fat.fileBlock[idx].busy=1;
            return i;
        }
    }
    return -1;
}

// Sequential Deallocation

// Linked Allocation

// Linked Deallocation

// Indexed Allocation

// Indexed Deallocation

struct file getFile() {
    printf("Enter the size of the file: \n");
    struct file File;
    scanf("%d", &File.sizeBytes);
    File.startBlockFAT = AllocateFile(File.sizeBytes/(512*8)+1); // throws ByteIncompatilbeException
    if(File.startBlockFAT != -1) {
        bDisk.noFiles++;
        bDisk.files[bDisk.noFiles] = File;
    }
    return File;
}

// could read and write data using char arrays in blocks
int Write(struct file File, int offset) {
    char *data;
    printf("Enter the data: \n");
    scanf("%s", data);
    int blocks = (File.sizeBytes/(512*8))+1;
    int size = File.sizeBytes;
    int startBlock = File.startBlockFAT;
    int curBlock = startBlock;
    int unitCount = 0;
    for(int i=0;i<size;i++) {
        if(offset%512 == 0) 
            unitCount++;
        if(unitCount%8 == 0) 
            curBlock = fat.fileBlock[curBlock].next;
        bDisk.blocks[curBlock].units[unitCount%8].bytes[++offset] = data[i];
    }
    bDisk.blocks[curBlock].usedUnits = unitCount;
    return startBlock;
}
int Read(struct disk Disk, int fileDes) {
    struct file File = bDisk.files[fileDes];
    int blocks = (File.sizeBytes/(512*8))+1;
    int startBlock = File.startBlockFAT;
    int curBlock = startBlock;
    int unitCount = 0;
    for(int i=0;i<blocks;i++) {
        for(int j=0;j<8;j++) {
            printf("%s", bDisk.blocks[curBlock].units[j].bytes);
        }
        curBlock = fat.fileBlock[curBlock].next;
    }
    return 0;
}

struct blockedDisk bDisk;
struct FAT fat;
struct file File;

int main(int argc, char** argv) {
    printf("Booting disk....\n");
    struct disk Disk = conDisk((512*8)*10); // The current disk is of 10 blocks | (1GB disk) 1*1024*1024*1024

    printf("Address of starting unit on disk %p \n", (void *) Disk.units);
    int i = 14;
    printf("Address of %d'th unit on the disk %p \n", i, (void *) Disk.units[i].bytes); // .bytes is used to get the hexa decimal address
    bDisk = blockifyDisk(Disk);
    printf("# blocks %d \n", bDisk.noBlocks);
    printf("Unit 8 %p\n", (void *) Disk.units[8].bytes);
    printf("blockedDisk block 3 %p are same\n", (void *) bDisk.blocks[1].units[0].bytes);

    // File Allocation Table
    fat = conFAT(bDisk.blocks);
    fat.noBlocks = bDisk.noBlocks;
    File = getFile();
    bDisk.files[bDisk.noFiles] = File;
    int a = Write(File, 0);
    int fileDes;
    printf("Enter the file descriptor: \n");
    scanf("%d", &fileDes);
    if(fileDes > bDisk.noFiles) {
        printf("Invalid file descriptor \n");
        return 0;
    }
    int b = Read(Disk, fileDes);
    return 0;
}