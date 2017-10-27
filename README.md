# Buffer-Manager
Advance Database Organization - CS 525

# USAGE

run `make` to produce the `test_assign1` binary

Execution will simply run the tests defined in `test_assign1_1.c`

## EXPLAINING THE CODE

Most of the functionality exists in `storage_mgr.c`. dberror, test_assign1 and test_helpers are all helper functionality; dberror defined our general ERROR CODE int `RC`, and defined PAGE_SIZE as 4096 [bytes].

The Storage Manager handles the usage of files as a BLOCK STORE, and all functions related to reading, writing and adding new blocks.

Some important assumptions are made:
    The user of the storage_mgr must close/open the pagefile using the provided openPageFile and closePageFile functions. All block functions assume the pagefile is currently open for reading and writing (in r+ mode).

    Most Block functions will change the curPos variable as relevant (ie readNextBLock will move the curPos to read block). However, ensureCapacity, appendEmptyBlock and getBlockPos will NEVER modify the curPos variable.

createPageFile:
    creates a new file with the given filename. this also adds one empty block the PageFile.

openPageFile:
    opens the PageFile in r+ mode, and initializes the provided SM_FileHandle struct.

closePageFile:
    closes the PageFile, and sets the file pointer to NULL.

destroyPageFile:
    deletes the provided PageFile from the system.

readBlock:
    reads the given block from the PageFile. returns -1 if the read fails  (no appropriate RC error was available), and otherwise returns an RC error if anything goes wrong.

getBlockPos:
    returns the current page position

read*Block functions:
    various convenience wrappers around the readBlock function.

writeBlock:
    writes the SM_PageHandle data to the given block. If the block number exceeds current capacity, extend the page to that capacity and then write. moves the curPagePos to the written block.

writeCurrentBlock:
    wrapper around writeBlock

appendEmptyBlock:
    adds a new block to the end of the PageFile. DOES NOT MODIFY curPagePos. returns -1 if memory allocation failed (no appropriate RC error was available)

ensureCapacity:
    if the given number of pages is less than the total pages available to the file, append empty blocks until total-pages == number-of-pages.


# Testing
all tests can be found in test_assign1_1.c

testCreateOpenClose() and testSinglePageContent were provided by the professor, and so do not require further explanation.

testReadBlock() is a helper function.

testMultiPageContent tries the following:
    - CreatePageFile()
    - OpenCreateFile()
    - readFirstBlock()
    - runs writeBlock() 8 times, and fills each page 4096 characters of the block #
    - jumps to block #0
    - runs readNextBlock() 8 times
    - readBlock(5)
    - readNextBlock(), block #6
    - readPreviousBlock(), block #5
    - jumpts to block #4
    - readCurrentBlock(), block #4
    - readLastBlock(), block #6
    

testSinglPageContent2 tries the following:
    - CreatePageFile()
    - OpenCreateFile()
    - readFirstBlock()
    - writeBlock() to block #0
    - readFirstBlock(), what was just written
    - appendEmptyBlock()
    - jumps to block #1 (the newly appended block)
    - readCurrentBlock()
    - writeCurrentBlock()
    - readCurrentBlock()
    - ensureCapacity(3), from 2 blocks to 3
    - readNextBlock(), the newly appended block
    - readPreviousBlock()
    - readNextBlock()
    - readLastBlock()
    - closePageFile()
    - destroyPageFile()
