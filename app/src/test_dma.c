#include "test_dma.h"

int checkval(int i, long unsigned int  poll, long unsigned int ref, long unsigned int addr) {
  if (poll != ref) {
    printf("[%d]Hardware result (1) %lx does not match reference value %lx at addr: [%lx]\r\n", i, poll, ref, addr);
    return 1;
  } else {
    printf("[%d]Success: (%lx)\r\n", i, poll, ref);
  }
  return 0;
}

int test_basic() {
  printf("\n[STARTING TEST: BASIC]\n\n");

  long unsigned int REFVAL1[10] = { 0xAAAAAAAAAAAAAAAA, 0xBBBBBBBBBBBBBBBB, 0xCCCCCCCCCCCCCCCC, 0xDDDDDDDDDDDDDDDD, 0xEEEEEEEEEEEEEEEE, 
                                        0xAAABAAABAAABAAAB, 0xAAACAAACAAACAAAC, 0xAAADAAADAAADAAAD, 0xAAAEAAAEAAAEAAAE, 0xAAAFAAAFAAAFAAAF};
  long unsigned int poll, ref;
  uint32_t size = 8;
  uint32_t logd = 6; // We need the log of the depth of the memory transaction: log2(8) = 3, log2(16) = 4

  // Write some reference data
  for (int i=0; i<10; i++) {
    reg_write64(DMA_ADDR1 + 8*i, REFVAL1[i]);
  }

  printf("DONE Setting Initial Values\n");


  set_DMAC(0, DMA_ADDR1, DMA_ADDR2, 8*size, 8*size, 10, logd);
  printf("DONE Prepping Addresses\n");
    
  start_DMA(0);
  printf("DONE Starting DMA\n");

  // Check DMA
  printf("CHECK RESULT 1\n");
  for(int i=0; i<10; i++) {
    poll = reg_read64(DMA_ADDR2 + i*8);
    ref = REFVAL1[i];
    if (checkval(i, poll, ref, DMA_ADDR2 + i*8))
      return 1;
  }
    
  printf("[All tests passed]!\n\n");
  return 0;
}


int test_stride() {
  printf("\n[STARTING TEST: STRIDE]\n\n");
  long unsigned int STRIDE_REFVAL1[10] = { 0xAAAAAAAAAAAAAAAA, 0xBBBBBBBBBBBBBBBB, 0xCCCCCCCCCCCCCCCC, 0xDDDDDDDDDDDDDDDD, 0xEEEEEEEEEEEEEEEE, 
                                    0xAAABAAABAAABAAAB, 0xAAACAAACAAACAAAC, 0xAAADAAADAAADAAAD, 0xAAAEAAAEAAAEAAAE, 0xAAAFAAAFAAAFAAAF};
  long unsigned int STRIDE_REFVAL1R[10] = {  0xAAAAAAAAAAAAAAAA, 0xBBBBBBBBBBBBBBBB, 
                                      0xEEEEEEEEEEEEEEEE, 0xAAABAAABAAABAAAB, 
                                      0xAAAEAAAEAAAEAAAE, 0xAAAFAAAFAAAFAAAF};
  long unsigned int STRIDE_REFVAL2[10] = { 0xFFFFFFFFFFFFFFFF, 0xABABABABABABABAB, 0xCDCDCDCDCDCDCDCD, 0xEFEFEFEFEFEFEFEF, 0xACACACACACACACAC, 
                                    0xBBBABBBABBBABBBA, 0xBBBCBBBCBBBCBBBC, 0xBBBDBBBDBBBDBBBD, 0xBBBEBBBEBBBEBBBE, 0xBBBFBBBFBBBFBBBF};
  long unsigned int STRIDE_REFVAL3[10] = { 0xADADADADADADADAD, 0xAEAEAEAEAEAEAEAE, 0xAFAFAFAFAFAFAFAF, 0xBCBCBCBCBCBCBCBC, 0xBDBDBDBDBDBDBDBD, 
                                    0xCCCACCCACCCACCCA, 0xCCCBCCCBCCCBCCCB, 0xCCCDCCCDCCCDCCCD, 0xCCCECCCECCCECCCE, 0xCCCFCCCFCCCFCCCF};
  long unsigned int poll, ref;
  uint32_t size = 1;
  uint32_t logd = 0;
  
  // Write some reference data
  for (int i=0; i<10; i++) 
    reg_write64(DMA_ADDR1 + 8*i, STRIDE_REFVAL1[i]);
  for (int i=0; i<10; i++) 
    reg_write64(DMA_ADDR2 + 8*i, STRIDE_REFVAL2[i]);
  for (int i=0; i<10; i++) 
    reg_write64(DMA_ADDR3 + 8*i, STRIDE_REFVAL3[i]);
    for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR4 + 8*i, 0);
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR5 + 8*i, 0);
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR6 + 8*i, 0);

  printf("DONE Setting Initial Values\n");
      
  // Start DMA for 64b, chunks of 2
  set_DMAC(0, DMA_ADDR1, DMA_ADDR4, 32, 16, 3, 4);    // 64b in chunks of 2
  set_DMAC(1, DMA_ADDR2, DMA_ADDR5, 0, 8, 10, 1);   // 16b
  set_DMAC(2, DMA_ADDR3, DMA_ADDR6, 8, 16, 10, 2);   // 32b
  printf("DONE reparing DMA data\n");
  sleep(1);
    
  start_DMA(0);
  start_DMA(1);
  start_DMA(2);
  printf("DONE Starting DMA\n");
  
  // Check DMA
  printf("CHECK RESULT 1\n");
  for(int i=0; i<6; i++) {
    poll = reg_read64(DMA_ADDR4 + i*8);
    ref = STRIDE_REFVAL1R[i];
    if (checkval(i, poll, ref, DMA_ADDR4 + i*8)){
      printf("CHECK RESULT 1 FAILED\n");
      return 1;
    }
  }
  
  printf("CHECK RESULT 2\n");
  for(int i=0; i<10; i++) {
    poll = reg_read64(DMA_ADDR5 + i*8);
    ref = (STRIDE_REFVAL2[0] & 0xFFFF);
    if (checkval(i, poll, ref, DMA_ADDR5 + i*8)){
      printf("CHECK RESULT 2 FAILED\n");
      return 1;
    }
  }
  
  printf("CHECK RESULT 3\n");
  for(int i=0; i<10; i++) {
    poll = reg_read64(DMA_ADDR6 + i*16);
    ref = (STRIDE_REFVAL3[i] & 0xFFFFFFFF);
    if (checkval(i, poll, ref, DMA_ADDR6 + i*16)) {
      printf("CHECK RESULT 3 FAILED\n");
      return 1;
    }
  }
  
  printf("[All tests passed]!\n\n");
  return 0;
}



int test_combined() {
  printf("\n[STARTING TEST: COMBINED]\n\n");

  long unsigned int COMB_REFVAL1[10] = { 0xAAAAAAAAAAAAAAAA, 0xBBBBBBBBBBBBBBBB, 0xCCCCCCCCCCCCCCCC, 0xDDDDDDDDDDDDDDDD, 0xEEEEEEEEEEEEEEEE, 
                                      0xAAABAAABAAABAAAB, 0xAAACAAACAAACAAAC, 0xAAADAAADAAADAAAD, 0xAAAEAAAEAAAEAAAE, 0xAAAFAAAFAAAFAAAF};
  long unsigned int COMB_REFVAL2[10] = { 0xFFFFFFFFFFFFFFFF, 0xABABABABABABABAB, 0xCDCDCDCDCDCDCDCD, 0xEFEFEFEFEFEFEFEF, 0xACACACACACACACAC, 
                                      0xBBBABBBABBBABBBA, 0xBBBCBBBCBBBCBBBC, 0xBBBDBBBDBBBDBBBD, 0xBBBEBBBEBBBEBBBE, 0xBBBFBBBFBBBFBBBF};
  long unsigned int COMB_REFVAL3[10] = { 0xADADADADADADADAD, 0xAEAEAEAEAEAEAEAE, 0xAFAFAFAFAFAFAFAF, 0xBCBCBCBCBCBCBCBC, 0xBDBDBDBDBDBDBDBD, 
                                      0xCCCACCCACCCACCCA, 0xCCCBCCCBCCCBCCCB, 0xCCCDCCCDCCCDCCCD, 0xCCCECCCECCCECCCE, 0xCCCFCCCFCCCFCCCF};
  long unsigned int poll, ref;
  uint32_t size = 1;
  uint32_t logd = 0;

  // Write some reference data
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR1 + 8*i, COMB_REFVAL1[i]);
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR2 + 8*i, COMB_REFVAL2[i]);
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR3 + 8*i, COMB_REFVAL3[i]);
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR4 + 8*i, 0);
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR5 + 8*i, 0);
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR6 + 8*i, 0);
    
  printf("DONE Setting Initial Values\n");
  
  
  // Start DMA for 64b, chunks of 2
  set_DMAC(0, DMA_ADDR1, DMA_ADDR4, 16, 16, 5, 4);    // 64b in chunks of 2
  set_DMAC(1, DMA_ADDR2, DMA_ADDR5, 8, 8, 10, 1);   // 16b
  set_DMAC(2, DMA_ADDR3, DMA_ADDR6, 8, 8, 10, 2);   // 32b
  printf("DONE reparing DMA data\n");
  sleep(1);
    
  start_DMA(0);
  start_DMA(1);
  start_DMA(2);
  printf("DONE Starting DMA\n");
  
  
  // Check DMA
  printf("CHECK RESULT 1\n");
  for(int i=0; i<10; i++) {
    poll = reg_read64(DMA_ADDR4 + i*8);
    ref = COMB_REFVAL1[i];
    if (checkval(i, poll, ref, DMA_ADDR4 + i*8))
      return 1;
  }
  
  printf("CHECK RESULT 2\n");
  for(int i=0; i<10; i++) {
    poll = reg_read64(DMA_ADDR5 + i*8);
    ref = (COMB_REFVAL2[i] & 0xFFFF);
    if (checkval(i, poll, ref, DMA_ADDR5 + i*8))
      return 1;
  }
  
  printf("CHECK RESULT 3\n");
  for(int i=0; i<10; i++) {
    poll = reg_read64(DMA_ADDR6 + i*8);
    ref = (COMB_REFVAL3[i] & 0xFFFFFFFF);
    if (checkval(i, poll, ref, DMA_ADDR6 + i*8))
      return 1;
  
  }
  
  printf("[All tests passed]!\n\n");
  return 0;
}




int test_width() {
  printf("\n[STARTING TEST: WIDTH]\n\n");

  long unsigned int WIDTH_REFVAL1[10] = { 0xBBAAAAAAAAAAAAAA, 0xCCBBBBBBBBBBBBBB, 0xDDCCCCCCCCCCCCCC, 0xEEDDDDDDDDDDDDDD, 0xFFEEEEEEEEEEEEEE, 
                                    0xAAABAAABAAABAAAB, 0xAAACAAACAAACAAAC, 0xAAADAAADAAADAAAD, 0xAAAEAAAEAAAEAAAE, 0xAAAFAAAFAAAFAAAF};
  long unsigned int poll, ref;
  uint32_t size = 1;
  uint32_t logd = 0; // We need the log of the depth of the memory transaction: log2(8) = 3, log2(16) = 4

  // Write some reference data
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR1 + 8*i, WIDTH_REFVAL1[i]);
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR2 + 8*i, 0);
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR3 + 8*i, 0);
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR4 + 8*i, 0);
  for (int i=0; i<10; i++)
    reg_write64(DMA_ADDR5 + 8*i, 0);
  printf("DONE Setting Initial Values\n");


  // Start DMA for 64b, chunks of 2
  set_DMAC(0, DMA_ADDR1, DMA_ADDR2, 16, 16, 5, 4);
  start_DMA(0);
  printf("DONE Starting DMA (1)\n");


  // Start DMA for 64b
  set_DMAC(0, DMA_ADDR1, DMA_ADDR3, 8, 8, 10, 3);
  start_DMA(0);
  printf("DONE Starting DMA (2)\n");


  // Start DMA for 32b
  set_DMAC(0, DMA_ADDR1, DMA_ADDR4+4, 8, 8, 10, 2);
  start_DMA(0);
  printf("DONE Starting DMA (3)\n");


  // Start DMA for 16b
  set_DMAC(0, DMA_ADDR1, DMA_ADDR5+6, 8, 8, 10, 1);
  start_DMA(0);
  printf("DONE Starting DMA (4)\n");


  // Start DMA for 8b
  set_DMAC(0, DMA_ADDR1, DMA_ADDR6+7, 8, 8, 10, 0);
  start_DMA(0);
  printf("DONE Starting DMA (5)\n");


  // Check DMA
  printf("CHECK RESULT 1\n");
  for(int i=0; i<10; i++) {
    poll = reg_read64(DMA_ADDR2 + i*8);
    ref = WIDTH_REFVAL1[i];
    if (checkval(i, poll, ref, DMA_ADDR2 + i*8))
      return 1;
  }
  printf("CHECK RESULT 2\n");
  for(int i=0; i<10; i++) {
    poll = reg_read64(DMA_ADDR3 + i*8);
    ref = WIDTH_REFVAL1[i];
    if (checkval(i, poll, ref, DMA_ADDR3 + i*8))
      return 1;
  }
  printf("CHECK RESULT 3\n");
  for(int i=0; i<10; i++) {
    poll = reg_read32(DMA_ADDR4 +4 + i*8) & 0xFFFFFFFF; 
    ref = (WIDTH_REFVAL1[i] & 0xFFFFFFFF);
    if (checkval(i, poll, ref, DMA_ADDR4 +4 + i*8))
      return 1;
  }
  printf("CHECK RESULT 4\n");
  for(int i=0; i<10; i++) {
    poll = reg_read16(DMA_ADDR5+6 + i*8);
    ref = (WIDTH_REFVAL1[i] & 0x0000FFFF);
    if (checkval(i, poll, ref, DMA_ADDR5+6 + i*8))
      return 1;
  }
  printf("CHECK RESULT 5\n");
  for(int i=0; i<10; i++) {
    poll = reg_read8(DMA_ADDR6+7 + i*8);
    ref = (WIDTH_REFVAL1[i] & 0x000000FF);
    if (checkval(i, poll, ref, DMA_ADDR6+7 + i*8))
      return 1;
  }

  printf("[All tests passed]!\n\n");
  return 0;
}