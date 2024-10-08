#include "config.h"

// i2s config for reading from I2S
i2s_config_t i2s_mic_Config = {
#if CONFIG_IDF_TARGET_ESP32
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
#endif
#if CONFIG_IDF_TARGET_ESP32S3
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
#endif
    .sample_rate = SAMPLE_RATE,
#if CONFIG_IDF_TARGET_ESP32
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
#endif
#if CONFIG_IDF_TARGET_ESP32S3
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
#endif
    .channel_format = I2S_MIC_CHANNEL,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
#if CONFIG_IDF_TARGET_ESP32
    .dma_buf_count = 64,
    .dma_buf_len = 128,
#endif
#if CONFIG_IDF_TARGET_ESP32S3
    .dma_buf_count = 16,
    .dma_buf_len = 128,
#endif
    .use_apll = true,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0};

// i2s microphone pins
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
#if CONFIG_IDF_TARGET_ESP32
    .data_out_num = I2S_OUT_SERIAL_DATA,
#endif
#if CONFIG_IDF_TARGET_ESP32S3
    .data_out_num = I2S_PIN_NO_CHANGE,
#endif
    .data_in_num = I2S_MIC_SERIAL_DATA};

#if CONFIG_IDF_TARGET_ESP32S3
i2s_config_t i2s_out_Config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_OUT_CHANNEL,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 64,
    .dma_buf_len = 128,
    .use_apll = true,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0};

i2s_pin_config_t i2s_out_pins = {
    .bck_io_num = I2S_OUT_SERIAL_CLOCK,
    .ws_io_num = I2S_OUT_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_OUT_SERIAL_DATA,
    .data_in_num = I2S_PIN_NO_CHANGE};
#endif
