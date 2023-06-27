#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct file {
    int startBlockFAT;
    int sizeBytes;
    int des;
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

struct blockedDisk bDisk;
struct FAT fat;
struct file File;

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

int hole() {
    for(int i=0;i<bDisk.noBlocks;i++) {
        if(!fat.fileBlock[i].busy)
            return i;
    }
    return -1;
}

// Linked Allocation
int AllocateFile(int noBlocks) {
    int idx = hole();
    int start = idx;
    while(noBlocks--) {
        fat.fileBlock[idx].busy = 1;
        int temp = hole();
        if(temp == -1) {
            printf("No sufficient space\n");
            return 0;
        }
        fat.fileBlock[idx].next = temp;
        idx = temp;
    }
    fat.fileBlock[idx].next = -1;
    return start;
}

// Linked Deallocation
int DeAllocateFile(int fileDes) {
    if(fileDes < 0) {
        printf("Invalid file descriptor \n");
        return -1;
    }
    bDisk.noFiles--;
    struct file File = bDisk.files[fileDes];
    int startBlock = File.startBlockFAT;
    int curBlock = startBlock;
    while(curBlock != -1) {
        bDisk.blocks[curBlock].usedUnits = 0;
        fat.fileBlock[curBlock].busy = 0;
        int tempBlock = fat.fileBlock[curBlock].next;
        fat.fileBlock[curBlock].next = -1;
        curBlock = tempBlock;
    }
    return 0;
}

struct file getFile() {
    printf("Enter the size of the file: \n");
    struct file File;
    scanf("%d", &File.sizeBytes);
    File.startBlockFAT = AllocateFile(File.sizeBytes/(512*8)+1); // throws ByteIncompatilbeException
    if(File.startBlockFAT != -1) {
        bDisk.files[bDisk.noFiles] = File;
        bDisk.noFiles++;
        File.des = bDisk.noFiles;
    }
    return File;
}

int min(int a, int b) {
    if (a < b)
        return a;
    else
        return b;
}

int Write(int fileDes, int offset) {
    if(fileDes < 0) {
        printf("Invalid file descriptor \n");
        return -1;
    }
    struct file File = bDisk.files[fileDes];
    int size = File.sizeBytes;
    char data[size];
    printf("Enter the data: \n");
    scanf("%s", data);
    int blocks = (File.sizeBytes/(512*8))+1;
    int startBlock = File.startBlockFAT;
    int curBlock = startBlock;
    int unitCount = 1, i = 0;
    for(i=0;i<min(strlen(data), size);i++) {
        if(offset%513 == 0) {
          unitCount++;
          offset = 1;
        }
        if(unitCount%9 == 0) {
          bDisk.blocks[curBlock].usedUnits = 8;
          curBlock = fat.fileBlock[curBlock].next;
          unitCount = 1;
        }
        bDisk.blocks[curBlock].units[unitCount-1].bytes[offset-1] = data[i];
        offset++;
    }
    bDisk.blocks[curBlock].units[unitCount-1].bytes[offset-1] = '!';
    if(i != strlen(data)) {
        printf("The data is more than the file size! Hence stored to the limit. \n");
    }
    bDisk.blocks[curBlock].usedUnits = unitCount;
    return File.des;
       return 0;
}

int Read(int fileDes) {
    if(fileDes < 0) {
        printf("Invalid file descriptor \n");
        return -1;
    }
    struct file File = bDisk.files[fileDes];
    int blocks = (File.sizeBytes/(512*8))+1;
    int startBlock = File.startBlockFAT;
    if(!fat.fileBlock[startBlock].busy) {
        printf("Invalid file descriptor \n");
        return -1;
    }
    int curBlock = startBlock;
    for(int i=0;i<blocks;i++) {
        for(int j=0;j<8;j++) {
            int i = 0;
            char ch = bDisk.blocks[curBlock].units[j].bytes[i];
            while(i < 512) {
                if(ch == '!') {
                    return 0;
                }
                printf("%c", ch);
                i++;
                ch = bDisk.blocks[curBlock].units[j].bytes[i];
            }
        }
        curBlock = fat.fileBlock[curBlock].next;
    }
    return 0;
}

int main(int argc, char** argv) {
    printf("Booting disk....\n");
    struct disk Disk = conDisk((512*8)*10); // The current disk is of 10 blocks | (1GB disk) 1*1024*1024*1024
    bDisk = blockifyDisk(Disk);
    // File Allocation Table
    fat = conFAT(bDisk.noBlocks);
    fat.noBlocks = bDisk.noBlocks;
    char ch;
    printf("Enter character q to quit \n");
    scanf("%c", &ch);
    while(ch != 'q') {
        printf("Enter the option: \n1. Create a file\n2. Write to a file\n3. Read a file\n4. Delete a file\n");
        int choice;
        scanf("%d", &choice);
        int fileDes;
        if(choice == 1) {
            File = getFile();
        }
        else if(choice == 2) {
             printf("Enter the file descriptor to write the file: \n");
             scanf(" %d", &fileDes);
             int a = Write(fileDes-1, 1);
        }
        else if(choice == 3) {
            printf("Enter the file descriptor to read the file: \n");
            scanf(" %d", &fileDes);
            int b = Read(fileDes-1);
        }
        else if(choice == 4) {
            printf("\nEnter the file descriptor to delete the file: \n");
            scanf(" %d", &fileDes);
            int c = DeAllocateFile(fileDes-1);
            if(!c)
                printf("Deleted the file successfully \n");
        }
        printf("\nEnter character q to quit \n");
        scanf(" %c", &ch);
    }
    return 0;
}