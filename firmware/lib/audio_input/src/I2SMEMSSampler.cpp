#include "I2SMEMSSampler.h"
#include "soc/i2s_reg.h"

I2SMEMSSampler::I2SMEMSSampler(
    i2s_port_t i2s_port,
    i2s_pin_config_t &i2s_pins,
    i2s_config_t i2s_config,
    bool fixSPH0645) : I2SSampler(i2s_port, i2s_config)
{
    m_i2sPins = i2s_pins;
    m_fixSPH0645 = fixSPH0645;
}

void I2SMEMSSampler::configureI2S()
{
    if (m_fixSPH0645)
    {
        // FIXES for SPH0645
#if CONFIG_IDF_TARGET_ESP32
        //REG_SET_BIT(I2S_TIMING_REG(m_i2sPort), BIT(1));
        //REG_SET_BIT(I2S_CONF_REG(m_i2sPort), I2S_RX_MSB_SHIFT);
#endif
#if CONFIG_IDF_TARGET_ESP32S3
        // Delay by falling edge
        REG_SET_BIT(I2S_RX_TIMING_REG(m_i2sPort), BIT(1));
        // Force Philips mode
        REG_SET_BIT(I2S_RX_CONF1_REG(m_i2sPort), I2S_RX_MSB_SHIFT);
#endif
    }

    i2s_set_pin(m_i2sPort, &m_i2sPins);
}

int I2SMEMSSampler::read(int16_t *samples, int count)
{
    // read from i2s
    int32_t *raw_samples = (int32_t *)malloc(sizeof(int32_t) * count);
    size_t bytes_read = 0;
    i2s_read(m_i2sPort, raw_samples, sizeof(int32_t) * count, &bytes_read, portMAX_DELAY);
    int samples_read = bytes_read / sizeof(int32_t);
    for (int i = 0; i < samples_read; i++)
    {
        raw_samples[i] >>= 16;
        samples[i] = (int16_t)(raw_samples[i] & 0xFFFF);
//        samples[i] = (raw_samples[i] & 0xFFFFFFF0) >> 11;
    }
    free(raw_samples);
    return samples_read;
}

int I2SMEMSSampler::read(int32_t *samples, int count)
{
    // read from i2s
    size_t bytes_read = 0;
    i2s_read(m_i2sPort, samples, sizeof(int32_t) * count, &bytes_read, portMAX_DELAY);
    int samples_read = bytes_read / sizeof(int32_t);
    return samples_read;
}

int I2SMEMSSampler::write(int32_t *samples, int count)
{
    size_t bytes_written = 0;
    i2s_write(m_i2sPort, samples, sizeof(int32_t) * count, &bytes_written, portMAX_DELAY);
    int samples_written = bytes_written / sizeof(int32_t);
    return samples_written;
}
