#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

// test name
char *testName;

/* test output files */
#define TESTPF "test_pagefile.bin"

/* prototypes for test functions */
static void testCreateOpenClose(void);
static void testSinglePageContent(void);
static void testReadBlock(int, SM_PageHandle);
static void testMultiPageContent(void);
static void testSinglePageContent2(void);

/* main function running all tests */
int
main (void)
{
  testName = "";
  
  initStorageManager();

  testCreateOpenClose();
  testSinglePageContent();
  testMultiPageContent();
  testSinglePageContent2();

  return 0;
}


/* check a return code. If it is not RC_OK then output a message, error description, and exit */
/* Try to create, open, and close a page file */
void
testCreateOpenClose(void)
{
  SM_FileHandle fh;

  testName = "test create open and close methods";

  TEST_CHECK(createPageFile (TESTPF));
  
  TEST_CHECK(openPageFile (TESTPF, &fh));
  ASSERT_TRUE(strcmp(fh.fileName, TESTPF) == 0, "filename correct");
  ASSERT_TRUE((fh.totalNumPages == 1), "expect 1 page in new file");
  ASSERT_TRUE((fh.curPagePos == 0), "freshly opened file's page position should be 0");

  TEST_CHECK(closePageFile (&fh));

  TEST_CHECK(destroyPageFile (TESTPF));

  // after destruction trying to open the file should cause an error
  ASSERT_TRUE((openPageFile(TESTPF, &fh) != RC_OK), "opening non-existing file should return an error.");
  TEST_DONE();
}

/* Try to create, open, and close a page file */
void
testSinglePageContent(void)
{
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i;

  testName = "test single page content";

  ph = (SM_PageHandle) malloc(PAGE_SIZE);

  // create a new page file
  TEST_CHECK(createPageFile (TESTPF));
  TEST_CHECK(openPageFile (TESTPF, &fh));
  printf("created and opened file\n");
  
  // read first page into handle
  TEST_CHECK(readFirstBlock (&fh, ph));
  // the page should be empty (zero bytes)
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
  printf("first block was empty\n");
    
  // change ph to be a string and write that one to disk
  for (i=0; i < PAGE_SIZE; i++)
    ph[i] = (i % 10) + '0';
  TEST_CHECK(writeBlock (0, &fh, ph));
  printf("writing first block\n");

  // read back the page containing the string and check that it is correct
  TEST_CHECK(readFirstBlock (&fh, ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  printf("reading first block\n");

  // destroy new page file
  TEST_CHECK(destroyPageFile (TESTPF));  
  
  TEST_DONE();
}

void
testMultiPageContent(void) {
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i, j;

  testName = "test multi page content";

  ph = (SM_PageHandle) malloc(PAGE_SIZE);
  // create a new page file
  TEST_CHECK(createPageFile(TESTPF));
  TEST_CHECK(openPageFile(TESTPF, &fh));
  printf("created and opened file\n");

  // read first page into handle
  TEST_CHECK(readFirstBlock(&fh, ph));
  // the page should be empty (zero bytes)
  for (i = 0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
  printf("first block was empty\n");


  // write 8 blocks to disk
  for (j = 0; j < 8; j++) {
    // change ph to be a string and write that one to disk
    for (i = 0; i < PAGE_SIZE; i++)
      ph[i] = j + '0'; // j forces each block to be unique
    printf("b %d %s", j, ph);
    TEST_CHECK(writeBlock(j, &fh, ph));
    printf("writing %d block\n", j);
  }

  fh.curPagePos = 0;
  for (j = 0; j < 7; j++) {
    // read back the page containing the string and check that it is correct
    printf("reading %d block\n", j+1);
    TEST_CHECK(readNextBlock(&fh, ph));
    testReadBlock(j+1, ph);
  }
  // jump to 5
  readBlock(5, &fh, ph);
  testReadBlock(5, ph);
  // move to 6
  readNextBlock(&fh, ph);
  testReadBlock(6, ph);
  // move back to 5
  readPreviousBlock(&fh, ph);
  testReadBlock(5, ph);

  ASSERT_TRUE(getBlockPos(&fh) == 5, "Block position was correct");
  readBlock(2, &fh, ph);
  testReadBlock(2, ph);
  ASSERT_TRUE(getBlockPos(&fh) == 2, "Block position was correct");

  fh.curPagePos = 4;
  readCurrentBlock(&fh, ph);
  testReadBlock(4, ph);

  readLastBlock(&fh, ph);
  testReadBlock(6, ph);

  TEST_DONE();
}

void testReadBlock(int j, SM_PageHandle ph){
  for (int i = 0; i < PAGE_SIZE; i++) {
    ASSERT_TRUE(ph[i] == j + '0',"character in page read from disk is the one we expected.");
  }
  printf("\nBlock %d was correct.\n", j);
}

/* Try to create, open, and close a page file */
void
testSinglePageContent2(void)
{
    SM_FileHandle fh;
    SM_PageHandle ph;
    int i;

    testName = "test single page content";

    ph = (SM_PageHandle) malloc(PAGE_SIZE);

    // create a new page file
    TEST_CHECK(createPageFile (TESTPF));
    TEST_CHECK(openPageFile (TESTPF, &fh));
    printf("created and opened file\n");

    // read first page into handle
    TEST_CHECK(readFirstBlock (&fh, ph));
    // the page should be empty (zero bytes)
    for (i=0; i < PAGE_SIZE; i++)
        ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
    printf("first block was empty\n");

    // change ph to be a string and write that one to disk
    for (i=0; i < PAGE_SIZE; i++)
        ph[i] = (i % 10) + '0';
    TEST_CHECK(writeBlock (0, &fh, ph));
    printf("writing first block\n");

    // read back the page containing the string and check that it is correct
    TEST_CHECK(readFirstBlock (&fh, ph));
    for (i=0; i < PAGE_SIZE; i++)
        ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
    printf("reading first block\n");

    // appending page check
    TEST_CHECK(appendEmptyBlock(&fh));
    ASSERT_TRUE((fh.totalNumPages == 2),"after appending there are 2 pages in file");
    ASSERT_TRUE((fh.curPagePos == 0), "after appending we're still at the same page position");

    // read current block check

    fh.curPagePos = 1; // jump to the new block
    TEST_CHECK(readCurrentBlock(&fh,ph));
    printf("%s\n", ph);
    for(i=0;i<PAGE_SIZE;i++)
        ASSERT_TRUE((ph[i]==0),"size of appended page is 0");
    printf("reading current block after appending");

    //write current block
    // assigning string value to pointer and then writing it
    for (i=0;i<PAGE_SIZE;i++)
        ph[i] = 'W';
    TEST_CHECK(writeCurrentBlock(&fh,ph));
    printf("writing in current block");

    //reading current block
    TEST_CHECK(readCurrentBlock(&fh,ph));
    for(i=0;i<PAGE_SIZE;i++)
        ASSERT_TRUE((ph[i]=='W'),"reading current block with value 'W'");

    //checking ensure capacity i.e. 3 pages
    TEST_CHECK(ensureCapacity(3,&fh));
    ASSERT_TRUE((fh.totalNumPages == 3),"there were total 3 pages");

    //read next (appended) block check
    TEST_CHECK(readNextBlock(&fh, ph));
    for(i=0;i<PAGE_SIZE;i++)
        ASSERT_TRUE((ph[i] == '\0'),"reading next page for empty (0)");

    //read previous block check
    TEST_CHECK(readPreviousBlock(&fh,ph));
    for(i=0;i<PAGE_SIZE;i++)
        ASSERT_TRUE((ph[i] == 'W'),"reading previous page with 'WRITING'");
    printf("reading previous block ");

    //read next block with size 0
    TEST_CHECK(readNextBlock(&fh,ph));
    for(i=0;i<PAGE_SIZE;i++)
        ASSERT_TRUE((ph[i]==0),"appended page with 0 byte");
    printf("reading next block");

    //read last block check
    TEST_CHECK(readLastBlock(&fh,ph));

    for(i=0;i<PAGE_SIZE;i++)
        ASSERT_TRUE((ph[i]==0),"Last page was appended with 0 byte size");
    printf("reading last block");


    // destroy new page file
    TEST_CHECK(closePageFile(&fh));
    TEST_CHECK(destroyPageFile (TESTPF));
    free(ph);
    TEST_DONE();
}
