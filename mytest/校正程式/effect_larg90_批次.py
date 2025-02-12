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

    org_positive_count = 0
    org_effect_rate = 0
    positive_count = 0 #有效次數
    ppb_bias = 0 #當次偏置
    ppb_sample_list = []
    ppb_bfadj_list = []
    ppb_afadj_list = []
    ppb_effect_rate = 0 #當天有效值
    ppb_Effect_rate = 0 #當天有效值

    #第一份資料
    ppb_one_day_list = [] #當天資料暫存



    #第二份資料
    org_effect_list = [] #當天原始資料有效值
    time_list = []
    ppb_midian_list = [] #當天調整前的中位數
    ppb_Midian_list = [] #當天調整後的中位數
    ppb_effect_list = [] #當天調整前的有效值
    ppb_Effect_list = [] #當天調整後的有效值
    ppb_bias_list = [] #偏置列表
    ppb_Bias_list = [] #偏置列表




    #PPB CAlCULATE
    for count, item in enumerate(data['pid_ppb']):
        if data['Time'][count][:-8] not in time_list:
            time_list.append(data['Time'][count][:-8])

            if ppb_sample_list != []:
                org_effect_rate = org_positive_count / len(ppb_sample_list) *100
                ppb_effect_rate = positive_count / len(ppb_sample_list) *100
                ppb_Effect_rate = ppb_effect_rate
                ppb_effect_list.append(ppb_effect_rate)
                ppb_midian_list.append(int(np.quantile(ppb_sample_list, 0.5, interpolation = 'linear')))
                ppb_bias_list.append(ppb_bias)
                positive_count = 0
                org_positive_count = 0

                if ppb_Effect_rate < 90:
                    while ppb_Effect_rate <= 91:
                        ppb_bias += 1
                        for i, k in enumerate(ppb_sample_list):
                            ppb_sample_list[i] = k+1
                            if ppb_sample_list[i] >= 0:
                                positive_count += 1
                        ppb_Effect_rate = positive_count / len(ppb_sample_list) *100
                        positive_count = 0

                org_effect_list.append(org_effect_rate)
                ppb_afadj_list.extend(ppb_sample_list)
                ppb_Effect_list.append(ppb_Effect_rate)
                ppb_Midian_list.append(int(np.quantile(ppb_sample_list, 0.5, interpolation = 'linear')))
                ppb_Bias_list.append(ppb_bias)

                ppb_sample_list.clear()

        if item+ppb_bias >= 0:
            positive_count += 1
        
        if item >= 0:
            org_positive_count += 1

        ppb_sample_list.append(item + ppb_bias)
        ppb_bfadj_list.append(item + ppb_bias)

    org_effect_rate = org_positive_count / len(ppb_sample_list) *100
    ppb_effect_rate = positive_count / len(ppb_sample_list) *100
    ppb_Effect_rate = ppb_effect_rate
    ppb_effect_list.append(ppb_effect_rate)
    ppb_midian_list.append(int(np.quantile(ppb_sample_list, 0.5, interpolation = 'linear')))
    ppb_bias_list.append(ppb_bias)
    positive_count = 0
    org_positive_count = 0

    if ppb_Effect_rate < 90:
        while ppb_Effect_rate <= 91:
            ppb_bias += 1
            for i, k in enumerate(ppb_sample_list):
                ppb_sample_list[i] = k+ppb_bias
                if ppb_sample_list[i] >= 0:
                    positive_count += 1
            ppb_Effect_rate = positive_count / len(ppb_sample_list) *100
            positive_count = 0

    org_effect_list.append(org_effect_rate)
    ppb_afadj_list.extend(ppb_sample_list)
    ppb_Effect_list.append(ppb_Effect_rate)
    ppb_Midian_list.append(int(np.quantile(ppb_sample_list, 0.5, interpolation = 'linear')))
    ppb_Bias_list.append(ppb_bias)


    # 第一份資料輸出
    new_df = pd.DataFrame({
        'time': data['Time'],
        'nh3_ppb(org)': data['pid_ppb'],
        'nh3_ppb(bf)': ppb_bfadj_list
    })
    output_file_1 = f'{file_path[:-4]}_adjusted.csv'
    new_df.to_csv(output_file_1, index=False)
    print(f'輸出檔案: {output_file_1}')
    
    # 第二份資料輸出
    new_df_2 = pd.DataFrame({
        'date': time_list,
        'effective value(org)': org_effect_list,
        'effective value(bf)': ppb_effect_list,
        'midian(bf)': ppb_midian_list,
        'bias(bf)': ppb_bias_list,
        'effective value(af)': ppb_Effect_list,
        'midian(af)': ppb_Midian_list,
        'bias(af)': ppb_Bias_list
    })
    output_file_2 = f'{file_path[:-4]}_summary.csv'
    new_df_2.to_csv(output_file_2, index=False)
    print(f'輸出檔案: {output_file_2}\n')

print("所有 CSV 檔案處理完畢！")




