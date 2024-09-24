#pragma once

#include <freertos/FreeRTOS.h>
#include <driver/i2s.h>

// network config
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASS"
#define MDNS_DOMAIN "microphone1"

// save to SPIFFS instead of SD Card?
// #define USE_SPIFFS 1

// sample rate for the system
#define SAMPLE_RATE 44100

// I2S Microphone Settings
//#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_RIGHT // bug in ESP_IDF, RIGHT actually means LEFT
//#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_23
//#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_22
//#define I2S_MIC_SERIAL_DATA GPIO_NUM_19
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_MIC_SERIAL_CLOCK 2
#define I2S_MIC_LEFT_RIGHT_CLOCK 6
#define I2S_MIC_SERIAL_DATA 4

#define I2S_OUT_CHANNEL I2S_CHANNEL_FMT_RIGHT_LEFT
#define I2S_OUT_SERIAL_CLOCK 15
#define I2S_OUT_LEFT_RIGHT_CLOCK 16
#define I2S_OUT_SERIAL_DATA 17

// i2s config for reading from of I2S
extern i2s_config_t i2s_mic_Config;
// i2s microphone pins
extern i2s_pin_config_t i2s_mic_pins;

extern i2s_config_t i2s_out_Config;
extern i2s_pin_config_t i2s_out_pins;
