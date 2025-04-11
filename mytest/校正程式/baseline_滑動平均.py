import os
import re
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib

matplotlib.rc("font", family='Microsoft YaHei')

def moving_average(data, window_size):
    """ 計算滑動平均 """
    return np.convolve(data, np.ones(window_size)/window_size, mode='valid')

data = pd.read_csv('356993687545265.csv', encoding = 'utf-8')
x = data['x']
y = data['ppb']

window_size = 200
baseline_fit = moving_average(y, window_size)

# 調整長度，使 baseline_fit 和原始信號匹配
x_trimmed = x[:len(baseline_fit)]
corrected_signal = y[:len(baseline_fit)] - baseline_fit

# 繪圖
plt.figure(figsize=(10, 5))

plt.subplot(2, 1, 1)
plt.plot(x, y, label="原始信號 (帶基線漂移)", color='blue')
plt.plot(x_trimmed, baseline_fit, label="滑動平均基線", linestyle="--", color='red')
plt.legend()
plt.title("滑動平均基線擬合")

plt.subplot(2, 1, 2)
plt.plot(x_trimmed, corrected_signal, label="校正後信號", color='green')
plt.axhline(0, linestyle="--", color="gray")  # 參考線
plt.legend()
plt.title("基線校正後的信號")

plt.tight_layout()
plt.show()