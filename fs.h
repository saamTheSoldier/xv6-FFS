// On-disk file system format.
// Both the kernel and user programs use this header file.


#define ROOTINO 1  // root i-number
#define BSIZE 512  // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block

  uint nbgs; // ffs: Number of block groups
  uint bgstart;  // ffs: Block number of the first block of the first block group
  uint bgsize;   // ffs: Size of a block group
  uint inodesperbg; // ffs: Inodes per block group
  uint inodeblocksperbg; // ffs: Inode blocks per block group
  uint bmapblocksperbg; // ffs: Bit map blocks per block group
  uint datablocksperbg; // ffs: Number of data blocks per block group

  uint bgmeta; // ffs: Number of blocks for metadata per block group = inodeblocksperbgroup + bmapblocksperbgroup
};

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEV only)
  short minor;          // Minor device number (T_DEV only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+1];   // Data block addresses
};

//ffs: return Block Group containing inode i
#define IBLOCKGROUP(i, sb)   ((i) / sb.inodesperbg)

//ffs: return start of bg containing indoe i
#define IBGSTART(i, sb)      ((sb.bgstart + (IBLOCKGROUP(i, sb)) * sb.bgsize))

//ffs: return start block of a bg
#define BBGSTART(b, sb)       ((sb.bgstart + ((b) * sb.bgsize)))

//ffs: is the block number valid
#define ISBVALID(b, sb)       ((sb.bgstart < (b) && ((b) - sb.bgstart) % sb.bgsize >= (sb.bgmeta)) ? 1 : 0)

//ffs: 
#define BG(b, sb)             ((b - sb.bgstart) / sb.bgstart) 

// Inodes per block.
#define IPB           (BSIZE / sizeof(struct dinode))

// Block containing inode i
// ffs: #define IBLOCK(i, sb)     ((i) / IPB + sb.inodestart)
#define IBLOCK(i, sb)         (IBGSTART(i, sb) + (i % sb.inodeblocksperbg))

// Bitmap bits per block
#define BPB           (BSIZE*8)

// ffs: Offset bit inside bitmap block that contains bit for block b
#define BOFFSET(b, sb)  (((b) - sb.bgstart - sb.bgmeta) % BPB)

// Block of free map containing bit for block b
// ffs: #define BBLOCK(b, sb) (b/BPB + sb.bmapstart)
#define BBLOCK(b, sb)   (BG(b, sb) + sb.inodeblocksperbg + (((b) - 32) % sb.bgsize) / BPB)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

// ffs: return first inode number of given bg
#define FINODEOFBG(b, sb) ((b) * (sb.inodesperbg))
struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

