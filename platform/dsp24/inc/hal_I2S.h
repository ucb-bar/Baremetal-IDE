// I2S_utils.h
#ifndef I2S_UTILS_h
#define I2S_UTILS_h

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_DMA.h"
#include  "hal_mmio.h"
#include "chip_config.h"

#define I2S_BASE                    0x10042000U
#define I2S_STATUS                  I2S_BASE + 0x08

#define I2S_WATERMARK_RX_L          I2S_BASE + 0x102
#define I2S_WATERMARK_RX_R          I2S_BASE + 0x103
#define I2S_WATERMARK_TX_L          I2S_BASE + 0x100
#define I2S_WATERMARK_TX_R          I2S_BASE + 0x101

#define I2S_TX_WATERMARK            I2S_BASE + 0x0C
#define I2S_RX_WATERMARK            I2S_BASE + 0x10

#define I2S_CLKDIV                  I2S_BASE + 0x14

#define I2S_TX_L                    I2S_BASE + 0x20
#define I2S_TX_R                    I2S_BASE + 0x28
#define I2S_RX_L                    I2S_BASE + 0x60
#define I2S_RX_R                    I2S_BASE + 0x68

//#define I2S_FP_MODE                 I2S_BASE + 0xB8

void set_I2S_params(int channel, int tx_en, int rx_en, int bitdepth_tx, int bitdepth_rx, int clkgen, int dacen, int ws_len);

void set_I2S_clkdiv(int channel, int clkdiv);

void set_I2S_watermark(int channel, int watermark_tx, int watermark_rx);

void set_I2S_en(int channel, int tx_en, int rx_en);

uint64_t read_I2S_tx(int channel, int left);

void write_I2S_rx(int channel, int left, uint64_t data);

uint64_t write_I2S_tx_DMA(int channel, int dma_num, int length, uint64_t* read_addr, int left, int poll);

uint64_t read_I2S_rx_DMA(int channel, int dma_num, int length, uint64_t* write_addr, int left, int poll);

void set_I2S_fp(int channel, int tx_fp, int rx_fp);

void set_I2S_force_left(int channel, int tx_force_left, int rx_force_left);


#ifdef __cplusplus
}
#endif

#endif