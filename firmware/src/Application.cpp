#include <FreeRTOS/FreeRTOS.h>
#include "Application.h"
#include <I2SMEMSSampler.h>
#include "transports/TCPSocketTransport.h"
#include "config.h"
#include <cmath> // For filter calculations
#include <algorithm> // For std::max

// #include "dsp/dsp.h"

void Application::begin()
{
  this->input = new I2SMEMSSampler(I2S_NUM_0, i2s_mic_pins, i2s_mic_Config, true);
  this->transport2 = new TCPSocketTransport();
  this->input->start();
  this->transport2->begin();
  TaskHandle_t task_handle;
  xTaskCreate(Application::streamer_task, "task", 65536, this, 0, &task_handle);
}

//#define EQ_I_0 0.73252859386304,-0.0148503406882401,-0.0037036673440813,-0.3042439147995185,0.0182185006302371
//#define EQ_I_1 1.0017473301489501,-1.9895098088248417,0.987859421801493,-1.9895310228247831,0.989585537950502

void Application::streamer_task(void *param)
{
  Application *app = (Application *)param;
  // now just read from the microphone and send to the clients
  int16_t *samples_out = (int16_t *)malloc(sizeof(int16_t) * 5000);
  int32_t *samples = (int32_t *)malloc(sizeof(int32_t) * 5000);
//  int32_t *samples2 = (int32_t *)malloc(sizeof(int32_t) * 5000);

//  biquad(eq_bq_0)
//  biquad(eq_bq_1)

  int32_t mean[128];
  int mean_index = 0;
  int mean_length = 0;

  while (true)
  {
    // read from the microphone
    int samples_read = 0;
    for (int i = 0; i < 1; i++)
      samples_read += app->input->read(&samples[samples_read], 256);

    // convert to fixed point formats
    for (int i = 0; i < samples_read; i++)
      samples[i] >>= 1; // divide by 2, allow some headroom in case the filters need it

    // Calculate the mean of the samples (DC offset)
    int32_t mean_sample = 0;
    for (int i = 0; i < samples_read; i++)
      mean_sample += samples[i] >> 8; // divide by 256 to avoid overflow
    mean_sample /= samples_read;

    mean[mean_index++] = mean_sample;
    if (mean_index >= 128)
      mean_index = 0;

    mean_sample = 0;
    if (mean_length < 128)
      mean_length++;
    for (int i = 0; i < mean_length; i++)
      mean_sample += mean[i];
    mean_sample /= mean_length;
    mean_sample <<= 8;

    // apply DC offset
    for (int i = 0; i < samples_read; i++)
        samples[i] -= mean_sample;

    // process the samples with the biquad filters
//    process_biquad(&eq_bq_0, biquadconstsfx(EQ_I_0), samples_read, samples, samples2);
//    process_biquad(&eq_bq_1, biquadconstsfx(EQ_I_1), samples_read, samples2, samples);

    // convert back to 16-bit
    for (int i = 0; i < samples_read; i++)
      samples_out[i] = std::max(-32768, std::min(32767, (samples[i] >> 15))); // multiply by 2 to get back to 16-bit

    // send to the transport
    app->transport2->send(samples_out, samples_read * sizeof(int16_t));
  }
}
