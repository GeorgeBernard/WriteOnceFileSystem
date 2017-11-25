# Write Once File System

This project implements an archival write-once file system (WOFS) using the [FUSE](https://github.com/libfuse/libfuse "FUSE Documentation") library. 

## Usage

### Mastering (master.cpp)

![Program Flow](./pipeline.png "Overview")

### Tree Script (tree.cpp)

### Mounting (mounter.c)

## Testing 

### Stress-test.py 

Input: 
* VERBOSE_FLAG: a boolean flag to indicate whether or not to print the results of each comparisson as the test is run.
* ITERATIONS: number of times to traverse the mounted file structure. 
* mount_point: relative path from the location of the test to the mounted file directory.
* true_path: relative path from the location of the test to the directory that was imaged. 

stres-test.py is a script that tests the functionality and validity of the mounted image. It does so by first getting and comparing the structures of the orginal and mounted directories. It then traverses this structure and ensures that all desired attributes about the original and mounted directories are the same. The test ensures: 

1. The structures of the original and mounted file directories are the same. 
2. Files between the two have identical content and size.
3. Directories between the two have the same list of children. 

## Limitations 

#### File sizes 

The mastering and mountering programs in theory are constrained by the memory available to read and write the necessary files. However, FUSE reads files in 131,072 bytes blocks. In other words, a complete read of a 1 MB file causes FUSE to trigger ~8 reads. As one would expect this puts a serious performance limitation on large files. Therefore, the WOFS is intended for maximum file sizes < 100 MB. Efforts were made to expand this 131,072 block size limit - however, it appears this is the current upperbound in FUSE's implementation. 

#### Hard Links and Soft Links

The WOFS is hard and soft link blind. This means mastering will follow the structure of the links and pull the contents of the links in as normal. For example, if a directory *dir* has a soft link to a directory *dir2* and a hard link to file *hl.txt* the mastering program will write *h1.txt* and its contents as well as *dir2* **and** all of its subdirectories. 


## Libraries/Dependencies

* [Command Line Parser](https://github.com/jarro2783/cxxopts "cxxopts")
* [ECC](https://github.com/ArashPartow/schifra): Reed Solomon Library  
* [FUSE](https://github.com/libfuse/libfuse "FUSE Documentation"): Fuse 3 required
* [Crytogrophy Library](https://www.openssl.org/docs/man1.0.2/crypto/hmac.html "HMAC Library")






This is very important

https://stackoverflow.com/questions/8347592/sizeof-operator-returns-incorrect-size


TODO:

1. ECC integration
2. Cleanup shared code
3. Test (find max point of failure)
4. Consider header file (should elements be different sizes) 
5. General code cleanup 
6. Benchmarking 
7. Stream implementation on write in Master       
  **DONE**
8. Alignment
9. Hard link and soft link investigation          
  **DONE - tested on the links test folder** 
  Ignores concept of soft links and just treats them as "normal" 
10. Safe traversal - to prevent segfaults 
  **DONE in mount**

Know limitations:

* Size
* ECC 
* Hard-soft 
