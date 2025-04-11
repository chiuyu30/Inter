import os
import re
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib

matplotlib.rc("font", family='Microsoft YaHei')

#讀取資料
data = pd.read_csv('356993687545265.csv', encoding = 'utf-8')
x = data['x']
y = data['ppb']

# 進行多項式擬合 (選擇二次多項式)
poly_coeffs = np.polyfit(x, y, deg=40)  # 擬合二次多項式
baseline_fit = np.polyval(poly_coeffs, x)  # 計算擬合基線

# 基線校正
corrected_signal = y - baseline_fit

# 繪圖
plt.figure(figsize=(10, 5))

plt.subplot(2, 1, 1)
plt.plot(x, y, label="原始信號 (帶基線漂移)", color='blue')
plt.plot(x, baseline_fit, label="擬合基線", linestyle="--", color='red')
plt.legend()
plt.title("基線擬合")

plt.subplot(2, 1, 2)
plt.plot(x, corrected_signal, label="校正後信號", color='green')
plt.axhline(0, linestyle="--", color="gray")  # 參考線
plt.legend()
plt.title("基線校正後的信號")

plt.tight_layout()
plt.show()
