/**
 * @file i2c.c
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "i2c.h"

void i2c_init(I2C_Type *I2Cx, I2C_InitType *I2C_init) {
  // need to disable I2C before make any change to prescaler
  i2c_disable(I2Cx);

  //36000000 / (5 * 100000) - 1 
  I2Cx->PRESCAL_HI = 0;
  // I2Cx->PRESCAL_LO = 0x3F;  // 112kHz
  I2Cx->PRESCAL_LO = 71;  // 112kHz
  
  i2c_enable(I2Cx);
}

Status i2c_wait_for_flag(I2C_Type *I2Cx, I2C_Flag flag, State state, uint32_t timestart, uint32_t timeout) {
  while (i2c_get_flag(I2Cx, flag) != state) {
    if (timeout == 0UL) {
      continue;
    }
    if (CLINT_getTime() > (timestart + timeout)) {
      return TIMEOUT;
    }
  }
  return OK;
}

Status i2c_wait_for_transaction(I2C_Type *I2Cx, uint32_t timestart, uint32_t timeout) {
  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_TIP, RESET, timestart, timeout) != OK) {
    // generate STOP to release the bus
    I2Cx->STAT_CMD = I2C_STAT_CMD_BUSY_STO_MSK;
    return TIMEOUT;
  }

  /* 3. receive ACK from slave */
  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_RXACK, RESET, timestart, timeout) != OK) {
    // generate STOP to release the bus
    I2Cx->STAT_CMD = I2C_STAT_CMD_BUSY_STO_MSK;
    return ERROR;
  }
}

Status i2c_master_receive(I2C_Type *I2Cx, uint16_t device_addr, uint8_t *buffer, uint16_t size, uint64_t timeout) {
  uint64_t timestart = clint_get_time();
  Status status;

  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_BUSY, RESET, timestart, timeout) != OK) {
    return BUSY;
  }

  /* 1. generate START */
  /* 2. write slave address + read bit */
  I2Cx->DATA = (device_addr << 1U) | I2C_DATA_READ;
  I2Cx->STAT_CMD = I2C_STAT_CMD_WR_MSK | I2C_STAT_CMD_RXACK_STA_MSK;

  status = i2c_wait_for_transaction(I2Cx, timestart, timeout);
  if (status != OK) {
    return status;
  }

  /* 4. read byte from slave */
  for (uint16_t i=0; i<size-1; i+=1) {
    I2Cx->STAT_CMD = I2C_STAT_CMD_RD_MSK;
    if (i2c_wait_for_flag(I2Cx, I2C_FLAG_RXACK, RESET, timestart, timeout) != OK) {
      // generate STOP to release the bus
      I2Cx->STAT_CMD = I2C_STAT_CMD_RD_MSK | I2C_STAT_CMD_ACK_MSK | I2C_STAT_CMD_BUSY_STO_MSK;
      return ERROR;
    }
    
    // wait transfer to finish
    if (i2c_wait_for_flag(I2Cx, I2C_FLAG_TIP, RESET, timestart, timeout) != OK) {
      return TIMEOUT;
    }
    *buffer = I2Cx->DATA;
    buffer += sizeof(uint8_t);
  }
  
  /* 5. generate STOP */
  I2Cx->STAT_CMD = I2C_STAT_CMD_RD_MSK | I2C_STAT_CMD_ACK_MSK | I2C_STAT_CMD_BUSY_STO_MSK;
  // wait transfer to finish
  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_TIP, RESET, timestart, timeout) != OK) {
    return TIMEOUT;
  }
  *buffer = I2Cx->DATA;
  
  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_BUSY, RESET, timestart, timeout) != OK) {
    return ERROR;
  }
  
  return OK;
}

Status i2c_master_transmit(I2C_Type *I2Cx, uint16_t device_addr, uint8_t *buffer, uint16_t size, uint64_t timeout) {
  uint64_t timestart = clint_get_time();
  Status status;

  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_BUSY, RESET, timestart, timeout) != OK) {
    return BUSY;
  }
  
  /* 1. generate START */
  /* 2. write slave address + write bit */
  I2Cx->DATA = (device_addr << 1U) | I2C_DATA_WRITE;
  I2Cx->STAT_CMD = I2C_STAT_CMD_WR_MSK | I2C_STAT_CMD_RXACK_STA_MSK;
  
  status = i2c_wait_for_transaction(I2Cx, timestart, timeout);
  if (status != OK) {
    return status;
  }
  
  /* 4. write data */
  /* 5. receive ACK from slave */
  for (uint16_t i=0; i<size-1; i+=1) {
    I2Cx->DATA = *buffer;
    buffer += sizeof(uint8_t);
    I2Cx->STAT_CMD = I2C_STAT_CMD_WR_MSK;
    
    status = i2c_wait_for_transaction(I2Cx, timestart, timeout);
    if (status != OK) {
      return status;
    }
  }

  /* 6. generate stop command */
  I2Cx->DATA = *buffer;
  I2Cx->STAT_CMD = I2C_STAT_CMD_WR_MSK | I2C_STAT_CMD_BUSY_STO_MSK;
  
  status = i2c_wait_for_transaction(I2Cx, timestart, timeout);
  if (status != OK) {
    return status;
  }
  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_BUSY, RESET, timestart, timeout) != OK) {
    return ERROR;
  }

  return OK;
}

Status i2c_read_memory(I2C_Type *I2Cx, uint16_t device_addr, uint8_t mem_addr, uint8_t *buffer, uint16_t size, uint64_t timeout) {
  uint64_t timestart = clint_get_time();
  Status status;

  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_BUSY, RESET, timestart, timeout) != OK) {
    return BUSY;
  }
  
  /* 1. generate START */
  /* 2. write slave address + write bit */
  I2Cx->DATA = (device_addr << 1U) | I2C_DATA_WRITE;
  I2Cx->STAT_CMD = I2C_STAT_CMD_WR_MSK | I2C_STAT_CMD_RXACK_STA_MSK;

  status = i2c_wait_for_transaction(I2Cx, timestart, timeout);
  if (status != OK) {
    return status;
  }
  
  /* 4. write data */
  /* 5. receive ACK from slave */
  I2Cx->DATA = mem_addr;
  I2Cx->STAT_CMD = I2C_STAT_CMD_WR_MSK;
  
  status = i2c_wait_for_transaction(I2Cx, timestart, timeout);
  if (status != OK) {
    return status;
  }

  /* 6. generate RESTART */
  /* 7. write slave address + read bit */
  I2Cx->DATA = (device_addr << 1U) | I2C_DATA_READ;
  I2Cx->STAT_CMD = I2C_STAT_CMD_WR_MSK | I2C_STAT_CMD_RXACK_STA_MSK;

  status = i2c_wait_for_transaction(I2Cx, timestart, timeout);
  if (status != OK) {
    return status;
  }

  /* 4. read byte from slave */
  for (uint16_t i=0; i<size-1; i+=1) {
    I2Cx->STAT_CMD = I2C_STAT_CMD_RD_MSK;
    if (i2c_wait_for_flag(I2Cx, I2C_FLAG_RXACK, RESET, timestart, timeout) != OK) {
      // generate STOP to release the bus
      I2Cx->STAT_CMD = I2C_STAT_CMD_RD_MSK | I2C_STAT_CMD_ACK_MSK | I2C_STAT_CMD_BUSY_STO_MSK;
      return ERROR;
    }
    // wait transfer to finish
    if (i2c_wait_for_flag(I2Cx, I2C_FLAG_TIP, RESET, timestart, timeout) != OK) {
      return TIMEOUT;
    }
    *buffer = I2Cx->DATA;
    buffer += sizeof(uint8_t);
  }
  
  /* 5. generate STOP */
  I2Cx->STAT_CMD = I2C_STAT_CMD_RD_MSK | I2C_STAT_CMD_ACK_MSK | I2C_STAT_CMD_BUSY_STO_MSK;
  // wait transfer to finish
  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_TIP, RESET, timestart, timeout) != OK) {
    return TIMEOUT;
  }
  *buffer = I2Cx->DATA;
  
  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_BUSY, RESET, timestart, timeout) != OK) {
    return ERROR;
  }

  return OK;
}

Status i2c_write_memory(I2C_Type *I2Cx, uint16_t device_addr, uint8_t mem_addr, uint8_t *buffer, uint16_t size, uint32_t timeout) {
  uint64_t timestart = clint_get_time();
  Status status;

  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_BUSY, RESET, timestart, timeout) != OK) {
    return BUSY;
  }
  
  /* 1. generate START */
  /* 2. write slave address + write bit */
  I2Cx->DATA = (device_addr << 1U) | I2C_DATA_WRITE;
  I2Cx->STAT_CMD = I2C_STAT_CMD_WR_MSK | I2C_STAT_CMD_RXACK_STA_MSK;

  status = i2c_wait_for_transaction(I2Cx, timestart, timeout);
  if (status != OK) {
    return status;
  }
  
  /* 4. write data */
  /* 5. receive ACK from slave */
  I2Cx->DATA = mem_addr;
  I2Cx->STAT_CMD = I2C_STAT_CMD_WR_MSK;
  
  if (i2c_wait_for_transaction(I2Cx, timestart, timeout) != OK) {
    return ERROR;
  }

  for (uint16_t i=0; i<size-1; i+=1) {
    I2Cx->DATA = *buffer;
    buffer += sizeof(uint8_t);
    I2Cx->STAT_CMD = I2C_STAT_CMD_WR_MSK;
    
    status = i2c_wait_for_transaction(I2Cx, timestart, timeout);
    if (status != OK) {
      return status;
    }
  }

  /* 6. generate stop command */
  I2Cx->DATA = *buffer;
  I2Cx->STAT_CMD = I2C_STAT_CMD_WR_MSK | I2C_STAT_CMD_BUSY_STO_MSK;
  
  status = i2c_wait_for_transaction(I2Cx, timestart, timeout);
  if (status != OK) {
    return status;
  }
  if (i2c_wait_for_flag(I2Cx, I2C_FLAG_BUSY, RESET, timestart, timeout) != OK) {
    return ERROR;
  }
  
  return OK;
}