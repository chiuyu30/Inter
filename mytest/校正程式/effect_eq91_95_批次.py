import os
import re
import pandas as pd
import numpy as np

# 取得目前資料夾內所有 CSV 檔案
csv_files = [f for f in os.listdir() if f.endswith('.csv')]
if not csv_files:
  print("資料夾內沒有 CSV 檔案，請確認檔案是否存在。")
  exit()

print(f'找到 {len(csv_files)} 個 CSV 檔案: {csv_files}\n')

for file_path in csv_files:
  print(f'正在處理: {file_path}')
    #INITIALIZE
  data = pd.read_csv(file_path, encoding = 'utf-8')
  print(f'原始數據: {data.head()}')


  positive_count_91 = 0 #有效次數
  positive_count_95 = 0 #有效次數
  positive_count_org = 0
  count_91 = 0
  count_95 = 0

  ppb_sample_org_list = []
  ppb_effect_org_rate = 0

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

  ppb_effect_org_list = []

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
  for count, item in enumerate(data['ppb']):
    if data['Time'][count][:-5] not in time_list:
      time_list.append(data['Time'][count][:-5])

      if ppb_sample_org_list != []:
        ppb_effect_org_rate = positive_count_org / len(ppb_sample_org_list) *100
        ppb_effect_org_list.append(ppb_effect_org_rate)
        positive_count_org = 0
        ppb_sample_org_list.clear()

      
      
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
    
    if item+ppb_bias_95 >= 0:
      positive_count_95 += 1

    if item >= 0:
      positive_count_org += 1

    ppb_sample_91_list.append(item + ppb_bias_91)
    ppb_bfadj_91_list.append(item + ppb_bias_91)

    ppb_sample_95_list.append(item + ppb_bias_95)
    ppb_bfadj_95_list.append(item + ppb_bias_95)

    ppb_sample_org_list.append(item)

  ppb_effect_org_rate = positive_count_org / len(ppb_sample_org_list) *100
  ppb_effect_org_list.append(ppb_effect_org_rate)
  positive_count_org = 0
  ppb_sample_org_list.clear()

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
  new_df = pd.DataFrame({
    'time': data['Time'],
    'nh3_ppb(org)' : data['ppb'],
    'nh3_ppb(91%)' : ppb_bfadj_91_list,
    'nh3_ppb(95%)' : ppb_bfadj_95_list
  })

  output_file_1 = f'{file_path[:-4]}_adjusted.csv'
  new_df.to_csv(output_file_1, index=False)
  print(f'輸出檔案: {output_file_1}')


  #第二份資料
  new_df_2 = pd.DataFrame({
    'date': time_list,
    'effective value(org)': ppb_effect_org_list,
    'effective value(bf91)': ppb_effect_91_list,
    'midian(bf91)': ppb_midian_91_list,
    'bias(bf91)': ppb_bias_91_list,
    'effective value(af91)': ppb_Effect_91_list,
    'midian(af91)': ppb_Midian_91_list,
    'bias(af91)': ppb_Bias_91_list,
    'effective value(bf95)': ppb_effect_95_list,
    'midian(bf95)': ppb_midian_95_list,
    'bias(bf95)': ppb_bias_95_list,
    'effective value(af95)': ppb_Effect_95_list,
    'midian(af95)': ppb_Midian_95_list,
    'bias(af95)': ppb_Bias_95_list
  })
  output_file_2 = f'{file_path[:-4]}_summary.csv'
  new_df_2.to_csv(output_file_2, index=False)
  print(f'輸出檔案: {output_file_2}\n')

print("所有 CSV 檔案處理完畢！")



