#include "MyI2S.h"

bool MyI2S::InitInput(i2s_bits_per_sample_t BPS,
                      int bckPin,
                      int wsPin,
                      int dataInPin)
{
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = BPS,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 16,
    .dma_buf_len = 60,
    .use_apll = false
  };

  i2s_pin_config_t pin_config;
  memset(&pin_config,0,sizeof(i2s_pin_config_t));
  pin_config.bck_io_num = bckPin;
  pin_config.ws_io_num = wsPin;
  pin_config.data_out_num = I2S_PIN_NO_CHANGE;
  pin_config.data_in_num = dataInPin;

  if(ESP_OK!=i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL))
  {
    Serial.println("install i2s driver failed");
    return false;
  }
  if(ESP_OK!=i2s_set_pin(I2S_NUM_0, &pin_config))
  {
    Serial.println("i2s set pin failed");
    return false;
  }
  return true;
}

bool MyI2S::InitAdcInput(i2s_bits_per_sample_t BPS, adc1_channel_t dataInPin)
{
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = BPS,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 16,
    .dma_buf_len = 60,
    .use_apll = false
  };

  if(ESP_OK!=i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL))
  {
    Serial.println("install i2s driver failed");
    return false;
  }
  i2s_set_pin(I2S_NUM_0, NULL);
  //i2s_set_adc_mode(ADC_UNIT_1, dataInPin);
  //adc1_config_channel_atten(dataInPin, ADC_ATTEN_DB_11);
  i2s_adc_enable(I2S_NUM_0);
  return true;
}

size_t MyI2S::Read(char* data, int numData)
{
  size_t recvSize;
  i2s_read(I2S_NUM_0, (void*)data, numData, &recvSize, portMAX_DELAY);
  return recvSize;
}

size_t MyI2S::Write(char* data, int numData)
{
  size_t sendSize;
  i2s_write(I2S_NUM_0, (void*)data, numData, &sendSize, portMAX_DELAY);
  return sendSize;
}

void MyI2S::End()
{
  i2s_driver_uninstall(I2S_NUM_0);
}