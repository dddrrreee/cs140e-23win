----------------------------------------------------------------------------
Assume we have the bootsector (or Volume ID) for a FAT32 
file system.  From the wikipedia pge:

 1. Bytes 14-15: the reserved sectors (`reserved_area_nsec`).
 2. Byte 13  number of sectors cluster (or number of sectors per file 
            system block). (`sec_per_cluster`).
 3. byte 16 gives us the number of FATs (we expect 2).
 4. Bytes 36-39 give us the sectors per fat (`nsec_per_fat`)
 5. Bytes 44-47 give us the first cluster (block) in the FAT  table
    itself that is legal. (we expect 2)

Assume these fields have the following values:

 0. The logical disk address (in sectors) of the FAT partition is 8192.
 1. The number of reserved sectors = 4404.
 2. Number of sectors per cluster (data block) = 64
 3. There are two fats.
 4. Number of sectors per fat = 1894.
 5. The first cluster is 2.

--------------------------------------------------------------------------
Questions

Q1: The first FAT is at which sector?

  [Hint: sum of digits = 23]

Q2: The second FAT is at which sector?

  [Hint sum of digits = 18]

Q3: The sector number of the first datablock is:

  [Hint: Sum of digits = 22]

Q4: Hint: each entry in the fat is 32 bits (FAT32).
The total number of data blocks (clusters) is:

  [Hint: Sum of digits = 17]

Q5: Total number of data sectors is

  [Hint: sum of digits = 35]

Q6: Last sector of the FAT partition is:
  [Hint: sum of digits = 21]

Q7: If the first 9 entries in the FAT are
  //index = 0  1  2  3  4  5  6  7    8
  fat[] = { ., ., 3, 4, 5, 6, 7, 8, 0xFFFFFF7 .... }

  A. How many blocks are in the root directory?

  B. Which data blocks hold the root directory (inclusive)?

  C. If you wanted the shrink the root directory to two data blocks (clusters),
     what FAT index would you set to LAST_CLUSTER?

  D. What are all the indexs you would set to 0?
