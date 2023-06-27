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
    int noFiles;
    // the base address of our collection of blocks that abstract an existing collection of units
    struct block* blocks;
    struct file* files; // files on disk - FAT
};

struct blockedDisk bDisk;
struct FAT fat;
struct file File;

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
    temp.freeUnits = temp.totUnits;
    temp.usedUnits = 0;
    temp.units = malloc(sizeof(struct unit)*temp.totUnits);
    // fill the disk with units (use the unit constructor to initailize all the units of disk)
    for(int i=0;i<temp.totUnits;i++) {
        temp.units[i] = conUnit();
    }
    return temp;
}

struct FAT conFAT(int noBlocks) {
    struct FAT temp;
    temp.noBlocks = noBlocks;
    temp.fileBlock = malloc(temp.noBlocks*sizeof(struct fatEntry));
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
    temp.noFiles = 0;
    temp.blocks = malloc(temp.noBlocks*sizeof(struct block));
    temp.files = malloc(temp.noBlocks*sizeof(struct file));
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
            int idx=i;
            for(;idx<i+noBlocks-1;idx++) {
                fat.fileBlock[idx].busy=1;
                fat.fileBlock[idx].next=idx+1;
            }
            fat.fileBlock[idx].busy=1;
            fat.fileBlock[idx].next=-1;
            return i;
        }
    }
    return -1;
}
struct file getFile() {
    printf("Enter the size of the file: \n");
    struct file File;
    scanf("%d", &File.sizeBytes);
    File.startBlockFAT = AllocateFile(File.sizeBytes/(512*8)+1); // throws ByteIncompatilbeException
    if(File.startBlockFAT != -1) {
        bDisk.files[bDisk.noFiles] = File;
        bDisk.noFiles++;
    }
    return File;
}
int main(int argc, char** argv) {
    printf("Booting disk....\n");
    struct disk Disk = conDisk(1*512*8*4); // 1GB disk
    printf("Address of starting unit on disk %p \n", (void *) Disk.units);
    int i = 14;
    printf("Address of %d'th unit on the disk %p \n", i, (void *) Disk.units[i].bytes); // .bytes is used to get the hexa decimal address
    bDisk = blockifyDisk(Disk);
    printf("# blocks %d \n", bDisk.noBlocks);
    printf("Unit 8 %p\n", (void *) Disk.units[8].bytes);
    printf("blockedDisk block 3 %p are same\n", (void *) bDisk.blocks[1].units[0].bytes);

    // File Allocation Table
    fat = conFAT(bDisk.noBlocks);

    File = getFile();
    printf("The startBlock file1 %d\n", File.startBlockFAT);
    printf("The nextBlock file1 %d\n", fat.fileBlock[File.startBlockFAT].next);

    File = getFile();
    printf("The startBlock file1 %d\n", File.startBlockFAT);
    printf("The nextBlock file1 %d\n", fat.fileBlock[File.startBlockFAT].next);
    printf("The no of files in disk %d\n", bDisk.noFiles);
    printf("Size of file1 %d\n", bDisk.files[0].sizeBytes);
    printf("Size of file2 %d\n", bDisk.files[1].sizeBytes);
    return 0;
}