#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#if FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2
#define FIRST_DATA_BLOCK (FIRST_INODE_BLOCK + NUM_INODE_BLOCKS)

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

void knprintf(char *s, int l) {
  intmask mask = disable();
  int i=0;
  for (i=0; i<l; i++)
    kprintf("%c", s[i]);
 restore(mask);
}

void print_inode(struct inode* in) {
  kprintf("INode: %d\n", in->id);
  kprintf("size: %d\n", in->size);
  kprintf("blocks:");
  
  int i=0;
  for (; i<in->next_free_block; i++) {
    kprintf(" %d,", in->blocks[i]);
  }
  kprintf("\n");
}

void print_oft(int fd) {
  kprintf("state of fd=%d: %d\n", fd, oft[fd].state);
  kprintf("fileptr: %d\n", oft[fd].fileptr);
  kprintf("Inode number: %d\n", oft[fd].in);
}

int get_next_free_block() {
  int i = 0;
  for (; i < fsd.nblocks; i++) {
    if (fs_getmaskbit(i) == 0)
      return i;
  }
  return -1;
}

int get_filename_index(char *filename) {
  intmask mask = disable();
  int i = 0;
  for (; i < fsd.root_dir.numentries; i++) {
    char * name = fsd.root_dir.entry[i].name;
    if (!strncmp(name, filename, FILENAMELEN)) {
      restore(mask);
      return i;
    }
  }
  restore(mask);
  return SYSERR;
}

int get_fd_by_inode(int inode) {

  intmask mask = disable();
  int i = 0;
  for (; i < NUM_FD; i++) {
    if (oft[i].in.id == inode && oft[i].state == FSTATE_OPEN) {
      restore(mask);
      return i;
    }
  }
  restore(mask);
  return NULL;
}

int get_free_fd_entry() {
  intmask mask = disable();
  int i = 0;
  for (; i < NUM_FD; i++) {
    if (oft[i].state == FSTATE_CLOSED) {
      restore(mask);
      return i;
    }
  }
  restore(mask);
  return SYSERR;
}

int fs_open(char *filename, int flags) {
  intmask mask = disable();
 
  int dir_index = get_filename_index(filename);
  if (dir_index != -1 && flags == O_WRONLY) {
    fs_delete(filename);
    int fd = fs_create(filename, O_CREAT);
    if (fd == SYSERR) {
      restore(mask);
      return SYSERR;
    }

    restore(mask);
    return fd;
  }
  if (dir_index == -1 && (flags == O_RDONLY || flags == O_RDWR)) {
    restore(mask);
    return SYSERR;
  }
  if (dir_index == -1 && flags == O_WRONLY) {
    int fd = fs_create(filename, O_CREAT);
    if (fd == SYSERR) {
      restore(mask);
      return SYSERR;
    }

    restore(mask);
    return SYSERR;
  }
  if (dir_index != -1 && (flags == O_RDONLY || flags == O_RDWR)) {
    int fd = get_free_fd_entry();
    if (fd == SYSERR) {
      restore(mask);
      return SYSERR;
    }
    
    int inode_num = fsd.root_dir.entry[dir_index].inode_num;
    struct inode inode_obj;
    fs_get_inode_by_num(0, inode_num, &inode_obj);

    struct directory *root = &fsd.root_dir; 
    oft[fd].state = FSTATE_OPEN;
    oft[fd].fileptr = 0;
    oft[fd].de = &root->entry[root->numentries];
    oft[fd].in = inode_obj;
    oft[fd].flags = flags;

    restore(mask);
    return fd;
  }
  restore(mask);
  return SYSERR;
}

int fs_delete(char *filename) {
  intmask mask = disable();

  int index = get_filename_index(filename);
  if (index == -1) {
    restore(mask);
    return 0;
  }
  int inode = fsd.root_dir.entry[index].inode_num;

  int fd = get_fd_by_inode(inode);
  if (fd != -1) {
    fs_close(fd);
  }

  struct inode inode_obj;
  fs_get_inode_by_num(0, inode, &inode_obj);
  int i = 0;
  for (; i < inode_obj.next_free_block; i++) {
    fs_clearmaskbit(inode_obj.blocks[i]);
  }
  inode_obj.next_free_block = 0;
  fs_put_inode_by_num(0, inode, &inode_obj);
  
  restore(mask);
  return 0;
}

int fs_create(char* filename, int flags) {
  intmask mask = disable();
  if (flags != O_CREAT) {
    kprintf("Only OCREAT supported.");
    restore(mask);
    return SYSERR;
  }
  if (get_filename_index(filename) != -1) {
    fs_delete(filename);
  }

  int new_inode = fsd.inodes_used;
  struct dirent entry;
  entry.inode_num = new_inode;
  strncpy(filename, entry.name, FILENAMELEN-1);

  struct directory* root = &fsd.root_dir;
  root->entry[root->numentries] = entry;

  struct inode inode_obj;
  inode_obj.id = new_inode;
  inode_obj.type = INODE_TYPE_FILE;
  inode_obj.device = 0;
  inode_obj.size = 0;
  inode_obj.next_free_block = 0;

  fs_put_inode_by_num(0, new_inode, &inode_obj);

  root->numentries++;
  fsd.inodes_used++;

  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));

  int fd = get_free_fd_entry();
  oft[fd].state = FSTATE_OPEN;
  oft[fd].fileptr = 0;
  oft[fd].de = &fsd.root_dir.entry[root->numentries];
  oft[fd].in = inode_obj;
  oft[fd].flags = O_WRONLY;
  

  if (fd == SYSERR) {
    restore(mask);
    return SYSERR;
  }

  print_oft(fd);

  restore(mask);
  return fd;
}

int fs_close(int fd) {
  intmask mask = disable();
  if (oft[fd].state != FSTATE_OPEN) {
    restore(mask);
    return 0;
  }

  oft[fd].state = FSTATE_CLOSED;
  oft[fd].fileptr = 0;
  oft[fd].de = NULL;

  restore(mask);
  return 1;
}

int fs_read(int fd, void* buf, int len) {
  char *buffer = (char*)buf;
  intmask mask = disable();
  
  if (oft[fd].state != FSTATE_OPEN) {
    restore(mask);
    return SYSERR;
  }

  print_oft(fd);
  struct inode inode_obj;
  fs_get_inode_by_num(0, oft[fd].in.id, &inode_obj);

  print_inode(&inode_obj);

  int total_bytes_read = 0;
  while (len > 0 && oft[fd].fileptr < inode_obj.size) {
    int cur_block = oft[fd].fileptr / fsd.blocksz;
    int offset = oft[fd].fileptr % fsd.blocksz;

    int more_bytes = inode_obj.size - oft[fd].fileptr + 1;
    int more_bytes_block = fsd.blocksz - offset;
    
    int bytes_to_read = more_bytes_block > more_bytes? more_bytes: more_bytes_block;
    int disk_block = FIRST_DATA_BLOCK + inode_obj.blocks[cur_block];

    bs_bread(dev0, disk_block, 0, &block_cache[0], fsd.blocksz);
    memcpy(buffer, &block_cache[offset], bytes_to_read);
    
    buffer += bytes_to_read;
    len -= bytes_to_read;
    oft[fd].fileptr += bytes_to_read;
    total_bytes_read += bytes_to_read;

    knprintf(block_cache, fsd.blocksz);
  }
  restore(mask);
  return total_bytes_read;
}

int fs_seek(int fd, int offset) {
  intmask mask = disable();

  if (oft[fd].state != FSTATE_OPEN) {
    restore(mask);
    return 0;
  }
  if (offset > oft[fd].in.size) {
    restore(fd);
    return 0;
  }
  oft[fd].fileptr = offset;
  restore(mask);
  return 1;
}


int fs_write(int fd, void *buf, int len) {
  char *buffer = (char*)buf;
  intmask mask = disable();

  if (oft[fd].state != FSTATE_OPEN) {
    restore(mask);
    return SYSERR;
  }
  struct inode inode_obj;
  fs_get_inode_by_num(0, oft[fd].in.id, &inode_obj);
  int total_bytes_written = 0;
  int size_increase = 0;
  while (len > 0) {
    int cur_block = oft[fd].fileptr / fsd.blocksz;
    int offset = oft[fd].fileptr % fsd.blocksz;
    int bytes_to_write = 0;
    int disk_block = 0; 

    if (oft[fd].fileptr < inode_obj.size) {
      int more_bytes_block = fsd.blocksz - offset;
      bytes_to_write = more_bytes_block > len ? len: more_bytes_block;
      disk_block = FIRST_DATA_BLOCK + inode_obj.blocks[cur_block];
    } else {
      bytes_to_write = fsd.blocksz > len ? len : fsd.blocksz;
      int new_disk_block = get_next_free_block();
      
      size_increase += bytes_to_write;

      inode_obj.blocks[inode_obj.next_free_block] = new_disk_block;
      inode_obj.next_free_block++;

      fsd.inodes_used++;
      fs_setmaskbit(new_disk_block);
      disk_block = FIRST_DATA_BLOCK + new_disk_block;
    }

    
    bs_bread(dev0, disk_block, 0, &block_cache[0], fsd.blocksz);
    memcpy(&block_cache[offset], buffer, bytes_to_write);
    bs_bwrite(dev0, disk_block, 0, &block_cache[0], fsd.blocksz);

    //kprintf("Written %d bytes to block num: %d\n", bytes_to_write, disk_block);
    //kprintf("Data:\n");
    //knprintf(block_cache, fsd.blocksz);
    //kprintf("\n");
     
    len -= bytes_to_write;
    buffer += bytes_to_write;
    oft[fd].fileptr += bytes_to_write;
    total_bytes_written += bytes_to_write;
  }

  inode_obj.size += size_increase;
  
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);
  fs_put_inode_by_num(dev0, inode_obj.id, &inode_obj);

  print_inode(&inode_obj);

  return total_bytes_written;
}

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

#endif /* FS */
