import sys

# path = sys.argv[1]
# print(path)

import os
import numpy as np
import wave

def concatenate_wav_files(directory, prefix):
    # 获取所有文件名
    file_names = [f for f in os.listdir(directory) if f.startswith(prefix) and f.endswith('.wav')]
    
    # 按文件名排序
    file_names.sort()
    
    # print
    # 初始化一个空的numpy数组来存储拼接后的音频数据
    concatenated_data = b''
    
    for file_name in file_names:
        file_path = os.path.join(directory, file_name)
        
        # 打开WAV文件
        with open(file_path, 'rb') as wav_file:
            # 读取音频数据
            audio_data = wav_file.read()
            audio_data = audio_data[44:]  # 去掉WAV文件的头部信息
            # 拼接音频数据
            concatenated_data += audio_data
    
    # 返回拼接后的音频数据
    return concatenated_data

# 指定目录和文件前缀
directory = '.'
prefix = 'record'

# 拼接音频文件
concatenated_audio = concatenate_wav_files(directory, prefix)

# 将拼接后的音频数据写入新的WAV文件
output_file = os.path.join(directory, 'concatenated.wav')
with wave.open(output_file, 'wb') as wav_file:
    # 设置音频参数
    wav_file.setnchannels(1)  # 单声道
    wav_file.setsampwidth(2)  # 16位
    wav_file.setframerate(44100)  # 44.1kHz采样率
    # 写入音频数据
    wav_file.writeframes(concatenated_audio)

print(f"音频文件已拼接并保存为 {output_file}")
