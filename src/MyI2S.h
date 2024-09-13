#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#define SAMPLE_RATE (44100)

class MyI2S
{
public:
  //以接收I2S的数据进行初始化
  bool InitInput(i2s_bits_per_sample_t BPS,
                 int bckPin,
                 int wsPin,
                 int dataInPin);

  //以接收ADC通道的数据进行初始化
  bool InitAdcInput(i2s_bits_per_sample_t BPS, adc1_channel_t adcChannel);

  size_t Read(char* data, int numData);

  size_t Write(char* data, int numData);

  void End();
};