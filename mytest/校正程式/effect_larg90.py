import os
import re
import pandas as pd
import numpy as np

#INPUT
print(f'該資料夾中的檔案:\n{os.listdir()}\n')
file_path = input('請輸入檔名:') + '.csv'
while file_path not in os.listdir():
    print('!!該檔案不在此資料夾內!!')
    file_path = input('請輸入檔名:') + '.csv'


#INITIALIZE
data = pd.read_csv(file_path, encoding = 'utf-8')
print(f'\n\n =============== OriginData ================ \n\n {data}')


positive_count = 0 #有效次數
ppb_bias = 0 #當次偏置
ppb_sample_list = []
ppb_bfadj_list = []
ppb_afadj_list = []
ppb_effect_rate = 0 #當天有效值
ppb_Effect_rate = 0 #當天有效值

#第一份資料
ppb_one_day_list = [] #當天資料暫存

new_df = {}

#第二份資料
time_list = []
ppb_midian_list = [] #當天調整前的中位數
ppb_Midian_list = [] #當天調整後的中位數
ppb_effect_list = [] #當天調整前的有效值
ppb_Effect_list = [] #當天調整後的有效值
ppb_bias_list = [] #偏置列表
ppb_Bias_list = [] #偏置列表

new_df_2 = {}


#PPB CAlCULATE
for count, item in enumerate(data['pid_ppb']):
    if data['Time'][count][:-5] not in time_list:
      time_list.append(data['Time'][count][:-5])

      if ppb_sample_list != []:
        ppb_effect_rate = positive_count / len(ppb_sample_list) *100
        ppb_Effect_rate = ppb_effect_rate
        ppb_effect_list.append(ppb_effect_rate)
        ppb_midian_list.append(int(np.quantile(ppb_sample_list, 0.5, interpolation = 'linear')))
        ppb_bias_list.append(ppb_bias)
        positive_count = 0

        if ppb_Effect_rate < 90:
          while ppb_Effect_rate <= 91:
            ppb_bias += 1
            for i, k in enumerate(ppb_sample_list):
              ppb_sample_list[i] = k+1
              if ppb_sample_list[i] >= 0:
                  positive_count += 1
            ppb_Effect_rate = positive_count / len(ppb_sample_list) *100
            positive_count = 0

        ppb_afadj_list.extend(ppb_sample_list)
        ppb_Effect_list.append(ppb_Effect_rate)
        ppb_Midian_list.append(int(np.quantile(ppb_sample_list, 0.5, interpolation = 'linear')))
        ppb_Bias_list.append(ppb_bias)

        ppb_sample_list.clear()

    if item+ppb_bias >= 0:
        positive_count += 1

    ppb_sample_list.append(item + ppb_bias)
    ppb_bfadj_list.append(item + ppb_bias)

ppb_effect_rate = positive_count / len(ppb_sample_list) *100
ppb_Effect_rate = ppb_effect_rate
ppb_effect_list.append(ppb_effect_rate)
ppb_midian_list.append(int(np.quantile(ppb_sample_list, 0.5, interpolation = 'linear')))
ppb_bias_list.append(ppb_bias)
positive_count = 0

if ppb_Effect_rate < 80:
    while ppb_Effect_rate <= 91:
        ppb_bias += 1
        for i, k in enumerate(ppb_sample_list):
            ppb_sample_list[i] = k+ppb_bias
            if ppb_sample_list[i] >= 0:
                positive_count += 1
        ppb_Effect_rate = positive_count / len(ppb_sample_list) *100
        positive_count = 0

ppb_afadj_list.extend(ppb_sample_list)
ppb_Effect_list.append(ppb_Effect_rate)
ppb_Midian_list.append(int(np.quantile(ppb_sample_list, 0.5, interpolation = 'linear')))
ppb_Bias_list.append(ppb_bias)


#第一份資料
new_df['time'] = data['Time']
new_df['nh3_ppb(org)'] = data['nh3_ppb']
new_df['nh3_ppb(bf)'] = ppb_bfadj_list

new_df = pd.DataFrame(new_df)
print(f'\n\n ================== ArrangedData =================== \n\n {new_df}')
outputFile = input('請輸入要輸出的檔名(第一份):')
print(f'已輸出檔案: {outputFile}')
new_df.to_csv(f'{outputFile}.csv', index = False)


#第二份資料
new_df_2['date'] = time_list
new_df_2['effective value(bf)'] = ppb_effect_list
new_df_2['midian(bf)'] = ppb_midian_list
new_df_2['bias(bf)'] = ppb_bias_list
new_df_2['effective value(af)'] = ppb_Effect_list
new_df_2['midian(af)'] = ppb_Midian_list
new_df_2['bias(af)'] = ppb_Bias_list

new_df_2 = pd.DataFrame(new_df_2)
print(f'\n\n ================== ArrangedData =================== \n\n {new_df_2}')
outputFile = input('請輸入要輸出的檔名(第二份):')
print(f'已輸出檔案: {outputFile}')
new_df_2.to_csv(f'{outputFile}.csv', index = False)



