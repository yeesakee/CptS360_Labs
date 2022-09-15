// please put your main and code in this file. All contained.
// Use the provided mk script to compile your code. You are welcome to change the mk script if you'd like
// but know that is how we will be batch compiling code using mk on the back end so your code will need to be able
// to run when that bash script is ran.
// Most code can be grabbed from t.c in root of the github repo. 

#include <stdio.h>
#include <fcntl.h>

#include <sys/types.h>
#include <unistd.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

struct partition {
	u8 drive;             // drive number FD=0, HD=0x80, etc.

	u8  head;             // starting head 
	u8  sector;           // starting sector
	u8  cylinder;         // starting cylinder

	u8  sys_type;         // partition type: NTFS, LINUX, etc.

	u8  end_head;         // end head 
	u8  end_sector;       // end sector
	u8  end_cylinder;     // end cylinder

	u32 start_sector;     // starting sector counting from 0 
	u32 nr_sectors;       // number of of sectors in partition
};

char *dev = "vdisk";
int fd;
    
// read a disk sector into char buf[512]
int read_sector(int fd, int sector, char *buf)
{
    lseek(fd, sector*512, SEEK_SET);  // lssek to byte sector*512
    read(fd, buf, 512);               // read 512 bytes into buf[ ]
}

int main()
{
    struct partition *p;
    char buf[512];
    int count = 1;

    fd = open(dev, O_RDONLY);   // open dev for READ
    read(fd, buf, 512);         // read 512 bytes of fd into buf[ ] 
    p = (struct partition*)&buf[0x1BE];

    printf("    start_sector end_sector nr_sectors\n");
    while (p->sys_type) {
        if (p->sys_type != 5){
            printf("P%d:    %d      %d       %d\n", count, p->start_sector,(p->start_sector + p->nr_sectors - 1), p->nr_sectors);
        }
        else {
            printf("P%d:    %d      %d       %d\n", count, p->start_sector,(p->start_sector + p->nr_sectors - 1), p->nr_sectors);
            count++;
            int localMBR = p->start_sector;
            int extended = localMBR;
            while(p->sys_type) {
                read_sector(fd, extended, buf);
                p = (struct partition*)&buf[0x1BE];
                printf("P%d:    %d      %d       %d\n", count, p->start_sector + extended,(p->start_sector + p->nr_sectors - 1)+ extended, p->nr_sectors);
                p++;
                extended = p->start_sector + localMBR;
                count++;
            }
        }
        count++;
        p++;

    }


    // read_sector(fd, 0, buf);    // read in MBR at sector 0    
    // int localMBR = extStart;
    // loop:
    // read_sector(fd, localMBR, buf);

    // partition table of localMBR in buf[ ] has 2 entries: 
    //print entry 1's start_sector, nr_sector;
    //compute and print P5's begin, end, nr_sectors

    // if (entry 2's start_sector != 0){
    //     compute and print next localMBR sector;
    //     continue loop;
    // }
}