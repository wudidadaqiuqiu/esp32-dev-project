#include <SPI.h>
#include "SdFat.h"
#include "MyI2S.h"
#include "wave.h"
#include <WiFi.h>
#include <EEPROM.h>            // read and write from flash memory

SdFs sd;      // sd卡
FsFile file;  // 录音文件

MyI2S mi;
const int record_time = 10;  // second
const char filename[] = "/test.wav";
const char *ssid = "linux";
const char *password = "12345678";


const int waveDataSize = record_time * 88200;
int32_t communicationData[1024];     //接收缓冲区
char partWavData[1024];
IPAddress serverIP(192, 168, 248, 110);
uint16_t serverPort = 3334;
uint16_t server_end_port = 3335;

WiFiClient client;
WiFiClient end_client;

const int gpio_ = 13;

void wifi_setup() {
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print(WiFi.localIP());
}

void reset_record() {
  EEPROM.begin(1);
  EEPROM.write(0, 0);
  EEPROM.commit();
}

void record_plus() {
  EEPROM.begin(1);
  EEPROM.write(0, EEPROM.read(0) + 1);
  EEPROM.commit();
}

void end_record() {
  if (!end_client.connected()) {
    while (!end_client.connect(serverIP, server_end_port, 1000)) {
      Serial.println("end port connecting.");
    }
  }
  Serial.println("end record");
  end_client.write("----------");
  end_client.flush();
  end_client.stop();
}

void save_record() {
  String path = "/record" + String(EEPROM.read(0)) + ".wav";
  if (!client.connected()) {
    while (!client.connect(serverIP, serverPort, 1000)) {
      Serial.println("client connecting.");
    }
  }

  //删除并创建文件
  sd.remove(path.c_str());
  file = sd.open(path.c_str(), O_WRITE|O_CREAT);
  if(!file)
  {
    Serial.println("crate file error");
    return;
  }

  auto header = CreateWaveHeader(1, 44100, 16);
  header.riffSize = waveDataSize + 44 - 8;
  header.dataSize = waveDataSize;
  client.write((uint8_t*)&header, 44);
  file.write(&header, 44);

  // if(!mi.InitInput(I2S_BITS_PER_SAMPLE_32BIT, 17, 21, 4))
  // {
  //   Serial.println("init i2s error");
  //   return;
  // }

  Serial.println("start");

  for (int j = 0; j < waveDataSize/1024; ++j) {
    // Serial.printf("read\n");
    auto sz = mi.Read((char*)communicationData, 4096);
    

    char*p =(char*)(communicationData);
    for(int i=0;i<sz/4;i++)
    {
      communicationData[i] *= 10;  //提高声音
      if(i%2 == 0)   //这里获取到的数据第一个Int32是右声道
      {
          partWavData[i] = p[4*i + 2];
          partWavData[i + 1] = p[4*i + 3];
      }
    }
    file.write((const byte*)partWavData, 1024);
    client.write((const byte*)partWavData, 1024);
  }
  file.close();
  client.flush();
  client.stop();
  Serial.println("finish save");
  record_plus();
}
void setup() {
  Serial.begin(115200);
  reset_record();
  delay(500);
  wifi_setup();

  // while (!end_client.connect(serverIP, server_end_port, 1000)) {
  //   Serial.println("end port connecting.");
  // }
  
  // Serial.println("connected");
  // 初始化SD卡
  if(!sd.begin(SdSpiConfig(5, DEDICATED_SPI, 18000000)))
  {
    Serial.println("init sd card error");
    return;
  }

  //删除并创建文件
  // sd.remove(filename);
  // file = sd.open(filename, O_WRITE|O_CREAT);
  // if(!file)
  // {
  //   Serial.println("crate file error");
  //   return;
  // }

  // auto header = CreateWaveHeader(1, 44100, 16);
  // header.riffSize = waveDataSize + 44 - 8;
  // header.dataSize = waveDataSize;
  // // client.write((uint8_t*)&header, 44);
  // file.write(&header, 44);

  if(!mi.InitInput(I2S_BITS_PER_SAMPLE_32BIT, 17, 21, 4))
  {
    Serial.println("init i2s error");
    return;
  }

  // Serial.println("start");

  // for (int j = 0; j < waveDataSize/1024; ++j) {
  //   Serial.printf("read\n");
  //   auto sz = mi.Read((char*)communicationData, 4096);
    

  //   char*p =(char*)(communicationData);
  //   for(int i=0;i<sz/4;i++)
  //   {
  //     communicationData[i] *= 10;  //提高声音
  //     if(i%2 == 0)   //这里获取到的数据第一个Int32是右声道
  //     {
  //         partWavData[i] = p[4*i + 2];
  //         partWavData[i + 1] = p[4*i + 3];
  //     }
  //   }
  //   file.write((const byte*)partWavData, 1024);
  //   // client.write((const byte*)partWavData, 1024);
  // }
  // file.close();
  // Serial.println("finish save");
  // client.flush();
  // client.stop();
  
  pinMode(gpio_, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  static int cnt = 0;
  // 读取引脚的电平状态
  int level = digitalRead(gpio_);
  static int last_level;
  // 输出引脚的电平状态
  Serial.print("GPIO pin ");
  Serial.print(gpio_);
  Serial.print(" level: ");
  Serial.println(level);

  
  if (level) {
    
    save_record();
    
  } else if (last_level) {
    end_record();
    last_level = 0;
  }
  last_level = level;

}