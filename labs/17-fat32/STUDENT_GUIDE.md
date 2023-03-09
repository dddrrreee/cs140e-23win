## Interrupts.

#### Introduction
In this lab, you implement a FAT32 file system. FAT32, which stands for File 
Allocation Table-32, is a disk format used to organize the files stored on a 
disk drive. The disk drive is marked up into addressable chunks called sectors 
and a “File Allocation Table” or FAT is created at the start of the drive so 
that each piece of information in the file can be found on the computer. The 
“32” part of the name refers to the amount of bits that the filing system uses 
to store these addresses and was added mainly to distinguish it from its 
predecessor, which was called FAT16. 

Below is a digaram of what the FAT-32 file system might look like when its 
laid out on the disk: 

<table><tr><td>
  <img src="images/fat32-overview-diagram.png" width="600"/>
</td></tr></table>




