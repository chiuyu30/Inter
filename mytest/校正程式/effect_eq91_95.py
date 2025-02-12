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


positive_count_91 = 0 #有效次數
positive_count_95 = 0 #有效次數

ppb_bias_91 = 0 #當次偏置
ppb_sample_91_list = []
ppb_effect_rate_91 = 0 #當天有效值
ppb_Effect_rate_91 = 0 #當天有效值


ppb_bias_95 = 0 #當次偏置
ppb_sample_95_list = []
ppb_effect_rate_95 = 0 #當天有效值
ppb_Effect_rate_95 = 0 #當天有效值

#第一份資料
ppb_bfadj_91_list = []
ppb_afadj_91_list = []
ppb_bfadj_95_list = []
ppb_afadj_95_list = []

new_df = {}

#第二份資料
time_list = []
ppb_midian_91_list = [] #當天調整前的中位數
ppb_Midian_91_list = [] #當天調整後的中位數
ppb_effect_91_list = [] #當天調整前的有效值
ppb_Effect_91_list = [] #當天調整後的有效值
ppb_bias_91_list = [] #偏置列表
ppb_Bias_91_list = [] #偏置列表

ppb_midian_95_list = [] #當天調整前的中位數
ppb_Midian_95_list = [] #當天調整後的中位數
ppb_effect_95_list = [] #當天調整前的有效值
ppb_Effect_95_list = [] #當天調整後的有效值
ppb_bias_95_list = [] #偏置列表
ppb_Bias_95_list = [] #偏置列表

new_df_2 = {}


#PPB CAlCULATE
for count, item in enumerate(data['nh3_ppb']):
  if data['Time'][count][:-5] not in time_list:
    time_list.append(data['Time'][count][:-5])

    if ppb_sample_91_list != []:
      ppb_effect_rate_91 = positive_count_91 / len(ppb_sample_91_list) *100
      ppb_Effect_rate_91 = ppb_effect_rate_91
      ppb_effect_91_list.append(ppb_effect_rate_91)
      ppb_midian_91_list.append(int(np.quantile(ppb_sample_91_list, 0.5, interpolation = 'linear')))
      ppb_bias_91_list.append(ppb_bias_91)
      positive_count_91 = 0

      while ppb_Effect_rate_91 >= 92 or ppb_Effect_rate_91 < 91:
        if count_91 >= 500:
          ppb_Effect_rate_91 = 92
          break

        if ppb_Effect_rate_91 > 92:
          ppb_bias_91 -= 1
          for i, k in enumerate(ppb_sample_91_list):
            ppb_sample_91_list[i] = k-1
            if ppb_sample_91_list[i] >= 0:
              positive_count_91 += 1

        if ppb_Effect_rate_91 < 91:
          ppb_bias_91 += 1
          for i, k in enumerate(ppb_sample_91_list):
            ppb_sample_91_list[i] = k+1
            if ppb_sample_91_list[i] >= 0:
              positive_count_91 += 1

        ppb_Effect_rate_91 = positive_count_91 / len(ppb_sample_91_list) *100
        positive_count_91 = 0
        count_91 += 1

      count_91 = 0
      ppb_afadj_91_list.extend(ppb_sample_91_list)
      ppb_Effect_91_list.append(ppb_Effect_rate_91)
      ppb_Midian_91_list.append(int(np.quantile(ppb_sample_91_list, 0.5, interpolation = 'linear')))
      ppb_Bias_91_list.append(ppb_bias_91)

      ppb_sample_91_list.clear()



    if ppb_sample_95_list != []:
      ppb_effect_rate_95 = positive_count_95 / len(ppb_sample_95_list) *100
      ppb_Effect_rate_95 = ppb_effect_rate_95
      ppb_effect_95_list.append(ppb_effect_rate_95)
      ppb_midian_95_list.append(int(np.quantile(ppb_sample_95_list, 0.5, interpolation = 'linear')))
      ppb_bias_95_list.append(ppb_bias_95)
      positive_count_95 = 0

      while ppb_Effect_rate_95 >= 96 or ppb_Effect_rate_95 < 95:
        if count_95 >= 500:
          ppb_Effect_rate_95 = 96
          break

        if ppb_Effect_rate_95 >= 96:
          ppb_bias_95 -= 1
          for i, k in enumerate(ppb_sample_95_list):
            ppb_sample_95_list[i] = k-1
            if ppb_sample_95_list[i] >= 0:
              positive_count_95 += 1

        if ppb_Effect_rate_95 < 95:
          ppb_bias_95 += 1
          for i, k in enumerate(ppb_sample_95_list):
            ppb_sample_95_list[i] = k+1
            if ppb_sample_95_list[i] >= 0:
              positive_count_95 += 1

        ppb_Effect_rate_95 = positive_count_95 / len(ppb_sample_95_list) *100
        positive_count_95 = 0
        count_95 += 1

      count_95 = 0
      ppb_afadj_95_list.extend(ppb_sample_95_list)
      ppb_Effect_95_list.append(ppb_Effect_rate_95)
      ppb_Midian_95_list.append(int(np.quantile(ppb_sample_95_list, 0.5, interpolation = 'linear')))
      ppb_Bias_95_list.append(ppb_bias_95)
      ppb_sample_95_list.clear()

  if item+ppb_bias_91 >= 0:
    positive_count_91 += 1

  ppb_sample_91_list.append(item + ppb_bias_91)
  ppb_bfadj_91_list.append(item + ppb_bias_91)

  if item+ppb_bias_95 >= 0:
    positive_count_95 += 1

  ppb_sample_95_list.append(item + ppb_bias_95)
  ppb_bfadj_95_list.append(item + ppb_bias_95)



ppb_effect_rate_91 = positive_count_91 / len(ppb_sample_91_list) *100
ppb_Effect_rate_91 = ppb_effect_rate_91
ppb_effect_91_list.append(ppb_effect_rate_91)
ppb_midian_91_list.append(int(np.quantile(ppb_sample_91_list, 0.5, interpolation = 'linear')))
ppb_bias_91_list.append(ppb_bias_91)
positive_count_91 = 0

while ppb_Effect_rate_91 >= 92 or ppb_Effect_rate_91 < 91:
    if count_91 >= 500:
        ppb_Effect_rate_91 = 92
        break

    if ppb_Effect_rate_91 > 92:
        ppb_bias_91 -= 1
        for i, k in enumerate(ppb_sample_91_list):
            ppb_sample_91_list[i] = k-1
            if ppb_sample_91_list[i] >= 0:
                positive_count_91 += 1

    if ppb_Effect_rate_91 < 91:
        ppb_bias_91 += 1
        for i, k in enumerate(ppb_sample_91_list):
            ppb_sample_91_list[i] = k+1
            if ppb_sample_91_list[i] >= 0:
                positive_count_91 += 1

    ppb_Effect_rate_91 = positive_count_91 / len(ppb_sample_91_list) *100
    positive_count_91 = 0
    count_91 += 1

count_91 = 0
ppb_afadj_91_list.extend(ppb_sample_91_list)
ppb_Effect_91_list.append(ppb_Effect_rate_91)
ppb_Midian_91_list.append(int(np.quantile(ppb_sample_91_list, 0.5, interpolation = 'linear')))
ppb_Bias_91_list.append(ppb_bias_91)


ppb_effect_rate_95 = positive_count_95 / len(ppb_sample_95_list) *100
ppb_Effect_rate_95 = ppb_effect_rate_95
ppb_effect_95_list.append(ppb_effect_rate_95)
ppb_midian_95_list.append(int(np.quantile(ppb_sample_95_list, 0.5, interpolation = 'linear')))
ppb_bias_95_list.append(ppb_bias_95)
positive_count_95 = 0

while ppb_Effect_rate_95 >= 96 or ppb_Effect_rate_95 < 95:
    if count_95 >= 500:
        ppb_Effect_rate_95 = 96
        break

    if ppb_Effect_rate_95 > 96:
        ppb_bias_95 -= 1
        for i, k in enumerate(ppb_sample_95_list):
            ppb_sample_95_list[i] = k-1
            if ppb_sample_95_list[i] >= 0:
                positive_count_95 += 1

    if ppb_Effect_rate_95 < 95:
        ppb_bias_95 += 1
        for i, k in enumerate(ppb_sample_95_list):
            ppb_sample_95_list[i] = k+1
            if ppb_sample_95_list[i] >= 0:
                positive_count_95 += 1

    ppb_Effect_rate_95 = positive_count_95 / len(ppb_sample_95_list) *100
    positive_count_95 = 0
    count_95 += 1

count_95 = 0
ppb_afadj_95_list.extend(ppb_sample_95_list)
ppb_Effect_95_list.append(ppb_Effect_rate_95)
ppb_Midian_95_list.append(int(np.quantile(ppb_sample_95_list, 0.5, interpolation = 'linear')))
ppb_Bias_95_list.append(ppb_bias_95)
'''****************end*********************'''

print(len(time_list))
print(len(ppb_effect_91_list))
print(len(ppb_midian_91_list))
print(len(ppb_bias_91_list))


#第一份資料
new_df['time'] = data['Time']
new_df['nh3_ppb(org)'] = data['nh3_ppb']
new_df['nh3_ppb(91%)'] = ppb_bfadj_91_list
new_df['nh3_ppb(95%)'] = ppb_bfadj_95_list

new_df = pd.DataFrame(new_df)
print(f'\n\n ================== ArrangedData =================== \n\n {new_df}')
outputFile = input('請輸入要輸出的檔名(第一份):')
print(f'已輸出檔案: {outputFile}')
new_df.to_csv(f'{outputFile}.csv', index = False)


#第二份資料
new_df_2['date'] = time_list
new_df_2['effective value(bf91)'] = ppb_effect_91_list
new_df_2['midian(bf91)'] = ppb_midian_91_list
new_df_2['bias(bf91)'] = ppb_bias_91_list
new_df_2['effective value(af91)'] = ppb_Effect_91_list
new_df_2['midian(af91)'] = ppb_Midian_91_list
new_df_2['bias(af91)'] = ppb_Bias_91_list
new_df_2['effective value(bf95)'] = ppb_effect_95_list
new_df_2['midian(bf95)'] = ppb_midian_95_list
new_df_2['bias(bf95)'] = ppb_bias_95_list
new_df_2['effective value(af95)'] = ppb_Effect_95_list
new_df_2['midian(af95)'] = ppb_Midian_95_list
new_df_2['bias(af95)'] = ppb_Bias_95_list

new_df_2 = pd.DataFrame(new_df_2)
print(f'\n\n ================== ArrangedData =================== \n\n {new_df_2}')
outputFile = input('請輸入要輸出的檔名(第二份):')
print(f'已輸出檔案: {outputFile}')
new_df_2.to_csv(f'{outputFile}.csv', index = False)



