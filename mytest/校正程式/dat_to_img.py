import numpy as np
import matplotlib.pyplot as plt
import sys

def convert_pointwise_dat_to_png(dat_file, png_file):
    try:
        # 讀取 .dat 文件並解析每行數據
        points = []
        with open(dat_file, 'r') as f:
            for line in f:
                parts = line.strip().split()
                if len(parts) == 3:
                    x = int(parts[0])  # X 坐標
                    y = int(parts[1])  # Y 坐標
                    value = int(parts[2], 16)  # 16 進制灰度值轉換為整數
                    points.append((x, y, value))
        
        # 找到圖像的尺寸 (根據 x 和 y 的最大值)
        max_x = max(p[0] for p in points) + 1
        max_y = max(p[1] for p in points) + 1
        
        # 初始化圖像矩陣為零 (黑色背景)
        image = np.zeros((max_y, max_x), dtype=np.uint8)
        
        # 將每個點的灰度值填入矩陣
        for x, y, value in points:
            image[y, x] = value
        
        # 繪製黑白圖像
        plt.imshow(image, cmap='gray', interpolation='nearest')  # 使用灰階 colormap
        plt.colorbar()  # 添加顏色條
        plt.title("Pointwise Hex Data Visualization")  # 圖片標題

        # 保存為 .png 圖片
        plt.savefig(png_file, dpi=300, bbox_inches='tight')
        print(f"成功將 {dat_file} 轉換為黑白圖片 {png_file}")
    except Exception as e:
        print(f"發生錯誤: {e}")

if __name__ == "__main__":
    # 從命令列參數讀取文件名
    if len(sys.argv) != 3:
        print("用法: python convert_pointwise_dat_to_png.py <input.dat> <output.png>")
    else:
        input_dat = sys.argv[1]
        output_png = sys.argv[2]
        convert_pointwise_dat_to_png(input_dat, output_png)
