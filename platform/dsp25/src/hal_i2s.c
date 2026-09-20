#include "hal_i2s.h"
#include  "hal_mmio.h"

void config_I2S(int channel, i2s_params_t* params) {
    // Define the I2S configuration structure
    volatile i2s_config_t* reg = (i2s_config_t*)I2S_CONFIG(channel);

    // Define and populate the config bitfield struct
    i2s_config_t config = {
        .clkgen_en      = params->clkgen,
        .tx_en          = params->tx_en,
        .rx_en          = params->rx_en,
        .dac_en         = params->dacen,
        .ws_len         = params->ws_len,
        .tx_bitdepth    = params->bitdepth_tx,
        .rx_bitdepth    = params->bitdepth_rx,
        .tx_fp_en       = params->tx_fp,
        .rx_fp_en       = params->rx_fp,
        .tx_force_left  = params->tx_force_left,
        .rx_force_left  = params->rx_force_left,
        .unused         = 0
    };

    // Write the configuration to the hardware register
    *reg = config;
    set_I2S_clkdiv(channel, params->clkdiv);
}

int get_I2S_tx_full(int channel, i2s_channel_side_t left_right) {
    volatile i2s_status_t* i2s_status_reg = (volatile i2s_status_t *)(I2S_STATUS(channel));
    i2s_status_t status = *i2s_status_reg;
    return (left_right == I2S_LEFT) ? status.l_tx_full : status.r_tx_full;
}

int get_I2S_rx_empty(int channel, i2s_channel_side_t left_right) {
    volatile i2s_status_t* i2s_status_reg = (volatile i2s_status_t *)(I2S_STATUS(channel));
    i2s_status_t status = *i2s_status_reg;
    return (left_right == I2S_LEFT) ? status.l_rx_empty : status.r_rx_empty;
}

int get_I2S_tx_watermark(int channel, i2s_channel_side_t left_right) {
    volatile int *watermark_reg;
    if (left_right == I2S_LEFT) {
        watermark_reg = (volatile int *)(I2S_WATERMARK_TX_L(channel));
    } else {
        watermark_reg = (volatile int *)(I2S_WATERMARK_TX_R(channel));
    }
    return *watermark_reg;
}

int get_I2S_rx_watermark(int channel, i2s_channel_side_t left_right) {
    volatile int *watermark_reg;
    if (left_right == I2S_LEFT) {
        watermark_reg = (volatile int *)(I2S_WATERMARK_RX_L(channel));
    } else {
        watermark_reg = (volatile int *)(I2S_WATERMARK_RX_R(channel));
    }
    return *watermark_reg;
}

void set_I2S_clkdiv(int channel, uint16_t clkdiv) {
    reg_write16(I2S_CLKDIV(channel), clkdiv);
}

void set_I2S_watermark(int channel, int watermark_tx, int watermark_rx) {
    reg_write8(I2S_TX_WATERMARK(channel), watermark_tx);
    reg_write8(I2S_RX_WATERMARK(channel), watermark_rx);
}

void set_I2S_en(int channel, int tx_en, int rx_en) {
    // Read current config
    volatile i2s_config_t* reg = (i2s_config_t*)I2S_CONFIG(channel);
    i2s_config_t config = *reg;

    // Write new enable values
    config.tx_en = tx_en;
    config.rx_en = rx_en;
    *reg = config;
}

uint64_t read_I2S_rx(int channel, i2s_channel_side_t left_right) {
    // Wait until RX queue is not empty (data available)
    while (get_I2S_rx_empty(channel, left_right)) {
        asm volatile("nop");
    }

    if (left_right == I2S_LEFT) {
        return reg_read64(I2S_RX_L(channel));
    } else {
        return reg_read64(I2S_RX_R(channel));
    }
}

void write_I2S_tx(int channel, i2s_channel_side_t left_right, uint64_t data) {
    // Wait until TX queue is not full
    while (get_I2S_tx_full(channel, left_right)) {
        asm volatile("nop");
    }

    if (left_right == I2S_LEFT) {
        return reg_write64(I2S_TX_L(channel), data);
    } else {
        return reg_write64(I2S_TX_R(channel), data);
    }
}

void set_I2S_fp(int channel, int tx_fp, int rx_fp) {
    volatile i2s_config_t* reg = (i2s_config_t*)I2S_CONFIG(channel);
    i2s_config_t config = *reg;
    config.tx_fp_en = tx_fp;
    config.rx_fp_en = rx_fp;
    *reg = config;
}

void set_I2S_force_left(int channel, int tx_force_left, int rx_force_left) {
    volatile i2s_config_t* reg = (i2s_config_t*)I2S_CONFIG(channel);
    i2s_config_t config = *reg;
    config.tx_force_left = tx_force_left;
    config.rx_force_left = rx_force_left;
    *reg = config;
}

void set_I2S_sample_freq(int channel, uint64_t sys_clk_freq, uint64_t target_sample_freq, uint8_t bits_per_sample) {
    // Master Clock (256LRCLK) should be `bit_depth * 2` (stereo)
    uint64_t mclk_freq = target_sample_freq * bits_per_sample * 2;

    // Actual clock divider = 2 * (N+1)
    uint16_t clkdiv = (uint16_t) ((sys_clk_freq / mclk_freq) / 2 - 1);
    set_I2S_clkdiv(channel, clkdiv);
}

// uint64_t write_I2S_tx_DMA(int channel, int dma_num, int length, uint64_t* read_addr, int left, int poll) {
//     if (left) {
//         //printf("Writing to left queue\n");
//         set_DMAP(dma_num, read_addr, I2S_TX_L(channel), I2S_WATERMARK_TX_L(channel), 8, 8, length, 3, poll);
//     } else {
//         //printf("Writing to right queue\n");
//         set_DMAP(dma_num, read_addr, I2S_TX_R(channel), I2S_WATERMARK_TX_R(channel), 8, 8, length, 3, poll);
//     }
//     start_DMA(dma_num);
//     return 0;
// }


// uint64_t read_I2S_rx_DMA(int channel, int dma_num, int length, uint64_t* write_addr, int left, int poll) {
//     if (left)
//         set_DMAP(dma_num, I2S_RX_L(channel), write_addr, I2S_WATERMARK_RX_L(channel), 0, 8, length, 3, poll);
//     else
//         set_DMAP(dma_num, I2S_RX_R(channel), write_addr, I2S_WATERMARK_RX_R(channel), 0, 8, length, 3, poll);
//     start_DMA(dma_num);
//     return 0;
// }


////////// WIP, Untested ///////////

void read_I2S_to_buffer(int channel, i2s_channel_side_t left_right, uint64_t* buffer, int num_blocks) {
    for (int i = 0; i < num_blocks; i++) {
        buffer[i] = read_I2S_rx(channel, left_right);
    }
}


//////////////////////////