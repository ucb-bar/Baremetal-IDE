/** I2S Driver Development Notes
* Status: WORK IN PROGRESS. Not yet tested on hardware. DMA functions not yet implemented.
* For additional documentation, see:
* - https://bwrcrepo.eecs.berkeley.edu/ee290c_ee194_intech22/sp25-chips/-/wikis/digital/dsp25#i2s-audio-from-sp24
* - https://docs.google.com/document/d/1uYSpOAxziZh0n_d3nmsinIkP4hF5a70qZ7CPOzlTTTk/edit?tab=t.0
*
*
* - All watermark, full, and empty signals are based on 64-bit blocks, not individual samples.
*   This means that when configuring watermarks or checking full/empty status,
*   the values correspond to the number of 64-bit blocks in the FIFO.
*
* -  If the write queue is full and we try to write additional samples to the queue, the CPU
*    will hang until there is room in the queue and the memory write is allowed to complete.
*    So busy waiting isn’t necessary since the cpu will just stall until the memory access
*    can complete. Similar behavior for reading from an empty queue.
*
* - Samples are packed into a 64 bit read/write. So if using 32 bit samples, there will be 2 samples
*   packed into each 64 bit read/write transaction
*
* @author Nicholas Carpenedo; ncarpenedo@berkeley.edu
* Bringup 2025
*/

#ifndef hal_i2s_H
#define hal_i2s_H

#ifdef __cplusplus
extern "C" {
#endif

#include "chip_config.h"

// ================================
//  MMIO Address Definitions
// ================================

// Cast to uintptr_t to avoid compiler warnings

#define I2S_CONFIG(channel)                  (uintptr_t)(I2S_BASE + 0x2 * channel)
#define I2S_STATUS(channel)                  (uintptr_t)(I2S_BASE + 0x08 + channel)

// Watermark signal values (read only)
#define I2S_WATERMARK_RX_L(channel)          (uintptr_t)(I2S_BASE + 0x102 + 0x4 * channel)
#define I2S_WATERMARK_RX_R(channel)          (uintptr_t)(I2S_BASE + 0x103 + 0x4 * channel)
#define I2S_WATERMARK_TX_L(channel)          (uintptr_t)(I2S_BASE + 0x100 + 0x4 * channel)
#define I2S_WATERMARK_TX_R(channel)          (uintptr_t)(I2S_BASE + 0x101 + 0x4 * channel)

// Set watermark level (read/write)
#define I2S_TX_WATERMARK(channel)            (uintptr_t)(I2S_BASE + 0x0C + channel)
#define I2S_RX_WATERMARK(channel)            (uintptr_t)(I2S_BASE + 0x10 + channel)

#define I2S_CLKDIV(channel)                  (uintptr_t)(I2S_BASE + 0x14 + 0x2 * channel)

#define I2S_TX_L(channel)                    (uintptr_t)(I2S_BASE + 0x20 + 0x10 * channel)
#define I2S_TX_R(channel)                    (uintptr_t)(I2S_BASE + 0x28 + 0x10 * channel)
#define I2S_RX_L(channel)                    (uintptr_t)(I2S_BASE + 0x60 + 0x10 * channel)
#define I2S_RX_R(channel)                    (uintptr_t)(I2S_BASE + 0x68 + 0x10 * channel)

typedef enum {
    I2S_LEFT = 0,
    I2S_RIGHT = 1
} i2s_channel_side_t;

typedef enum {
    I2S_BITDEPTH_8 = 0,
    I2S_BITDEPTH_16 = 1,
    I2S_BITDEPTH_24 = 2,
    I2S_BITDEPTH_32 = 3
} i2s_bitdepth_t;

typedef struct i2s_params {
    int tx_en;                             // Enable I2S TX enable
    int rx_en;                             // Enable I2S RX enable
    i2s_bitdepth_t bitdepth_tx;            // Bitdepth for TX (0:8bit, 1:16bit, 2:24bit, 3:32bit)
    i2s_bitdepth_t bitdepth_rx;            // Bitdepth for RX (0:8bit, 1:16bit, 2:24bit, 3:32bit)
    int ws_len;                            // Word select length (0:16, 1:24, 2:32, 3:48). Should match bitdepth
    int clkdiv;                            // Clock divider for generating I2S clock from system clock
    int clkgen;                            // Enable internal clock generator
    int dacen;                             // Enable I2S DAC output
    int tx_fp;                             // Enable TX floating point
    int rx_fp;                             // Enable RX floating point
    int tx_force_left;                     // Force TX to left channel
    int rx_force_left;                     // Force RX to left channel
} i2s_params_t;



// ================================
//  Bitfield Structs
// ================================

typedef struct i2s_config {
    uint16_t clkgen_en : 1;
    uint16_t tx_en : 1;
    uint16_t rx_en : 1;
    uint16_t dac_en : 1;
    uint16_t ws_len : 2;
    uint16_t tx_bitdepth : 2;
    uint16_t rx_bitdepth : 2;
    uint16_t tx_fp_en : 1;
    uint16_t rx_fp_en : 1;
    uint16_t tx_force_left : 1;
    uint16_t rx_force_left : 1;
    uint16_t unused : 2;
} __attribute__((packed)) i2s_config_t;

typedef struct i2s_status {
    uint8_t l_tx_full : 1;
    uint8_t r_tx_full : 1;
    uint8_t l_rx_empty : 1;
    uint8_t r_rx_empty : 1;
    uint8_t unused : 4;
} __attribute__((packed)) i2s_status_t;

// ===================================
//  I2S Driver Function Prototypes
// ===================================

/**
 * @brief Configures the I2S peripheral parameters for a specified channel.
 *
 * This function sets up the basic operating parameters for the I2S interface,
 * including data format, sample rate, bit depth, etc.,
 * based on the provided configuration structure.
 *
 * @param channel The I2S channel number (e.g., 0-3).
 * @param params Pointer to the i2s_params_t structure containing the desired configuration.
 */
void config_I2S(int channel, i2s_params_t* params);

/**
 * @brief Sets the clock divider for the I2S interface on a specific channel.
 *
 * The clock divider is used to generate the necessary bit clock from the system clock.
 *
 * @param channel The I2S channel number.
 * @param clkdiv The 16-bit clock division value.
 */
void set_I2S_clkdiv(int channel, uint16_t clkdiv);

/**
 * @brief Sets the TX and RX watermark levels for a specific I2S channel.
 *
 * The level is based on **64-bit blocks**, not individual samples.
 *
 * @param channel The I2S channel number.
 * @param watermark_tx The TX FIFO level threshold (in 64-bit blocks).
 * @param watermark_rx The RX FIFO level threshold (in 64-bit blocks).
 */
void set_I2S_watermark(int channel, int watermark_tx, int watermark_rx);

/**
 * @brief Gets the current RX watermark status for a specific channel and channel side.
 *
 * @param channel The I2S channel number.
 * @param left_right The channel side to check (e.g., I2S_CHANNEL_LEFT or I2S_CHANNEL_RIGHT).
 */
int get_I2S_rx_watermark(int channel, i2s_channel_side_t left_right);

/**
 * @brief Gets the current TX watermark status for a specific channel and channel side.
 *
 * @param channel The I2S channel number.
 * @param left_right The channel side to check.
 */
int get_I2S_tx_watermark(int channel, i2s_channel_side_t left_right);

/**
 * @brief Checks if the TX FIFO queue is full for a specific channel side.
 *
 * The full signal is based on **64-bit blocks**. If true, a subsequent write will stall the CPU.
 *
 * @param channel The I2S channel number.
 * @param left_right The channel side to check.
 * @return int Non-zero if the TX queue is full, 0 otherwise.
 */
int get_I2S_tx_full(int channel, i2s_channel_side_t left_right);

/**
 * @brief Checks if the RX FIFO queue is empty for a specific channel side.
 *
 * The empty signal is based on **64-bit blocks**. If false, there is at least one 64-bit block ready to be read.
 *
 * @param channel The I2S channel number.
 * @param left_right The channel side to check.
 * @return int Non-zero if the RX queue is empty, 0 otherwise.
 */
int get_I2S_rx_empty(int channel, i2s_channel_side_t left_right);

/**
 * @brief Enables or disables the I2S Transmit (TX) and Receive (RX) paths.
 *
 * @param channel The I2S channel number.
 * @param tx_en Enable (1) or disable (0) the TX path.
 * @param rx_en Enable (1) or disable (0) the RX path.
 */
void set_I2S_en(int channel, int tx_en, int rx_en);

/**
 * @brief Reads a 64-bit block of audio data from the RX FIFO.
 *
 * Reading pops a single **64-bit block** from the queue. Samples are packed within the 64-bit value.
 * For example, if the bit depth is 32 bits, 2 samples are packed (32 * 2 = 64).
 *
 * @param channel The I2S channel number.
 * @param left The channel side to read from.
 * @return uint64_t The 64-bit packed audio data block.
 */
uint64_t read_I2S_rx(int channel, i2s_channel_side_t left);

/**
 * @brief Writes a 64-bit block of audio data to the TX FIFO.
 *
 * Writing enqueues a single **64-bit block**. Samples must be packed into the 64-bit value
 * according to the configured bit depth (e.g., 4 samples for 16-bit depth).
 * If the queue is full, the CPU will stall until space is available.
 *
 * @param channel The I2S channel number.
 * @param left The channel side to write to.
 * @param data The 64-bit packed audio data block to transmit.
 */
void write_I2S_tx(int channel, i2s_channel_side_t left, uint64_t data);

/**
 * @brief Configures the I2S peripheral's floating point conversion
 *
 * @param channel The I2S channel number.
 * @param tx_fp TX floating point configuration (1 to enable, 0 to disable).
 * @param rx_fp RX floating point configuration (1 to enable, 0 to disable).
 */
void set_I2S_fp(int channel, int tx_fp, int rx_fp);

/**
 * @brief Forces TX/RX data to the left channel.
 *
 * This setting is typically used to ensure data is always sent/received on the
 * left channel's time slot, regardless of the Word Select signal.
 *
 * @param channel The I2S channel number.
 * @param tx_force_left Force TX to left channel (1 to enable, 0 to disable).
 * @param rx_force_left Force RX to left channel (1 to enable, 0 to disable).
 */
void set_I2S_force_left(int channel, int tx_force_left, int rx_force_left);

void set_I2S_sample_freq(int channel, uint64_t sys_clk_freq, uint64_t target_sample_freq, uint8_t bits_per_sample);

// ===================================
//  DMA Functions (Work In Progress)
// ===================================

/*
// uint64_t write_I2S_tx_DMA(int channel, int dma_num, int length, uint64_t* read_addr, int left, int poll);
// uint64_t read_I2S_rx_DMA(int channel, int dma_num, int length, uint64_t* write_addr, int left, int poll);
*/


#ifdef __cplusplus
}
#endif

#endif // hal_i2s_H