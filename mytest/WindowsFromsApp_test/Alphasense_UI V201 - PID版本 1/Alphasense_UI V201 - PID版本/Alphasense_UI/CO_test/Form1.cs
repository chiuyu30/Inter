using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Xml.Linq;
using System.IO;
using System.IO.Ports;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace CO_test
{

    public partial class Form1 : Form
    {
        

        public Form1()
        {
            InitializeComponent();
            InitializeLabelList();
            init_AllData();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Sabio_UpdateRate_Box.Text = Sabio_UpdateRate_Box.Items[0].ToString();
            AlphFix_UpdateRate_Box.Text = AlphFix_UpdateRate_Box.Items[0].ToString();
            AlphFix_lock();
        }

        /*****************************************************************************************************************************************************************
         *****************************************************************************************************************************************************************
                                                                                        SENSOR
         *****************************************************************************************************************************************************************
         *****************************************************************************************************************************************************************/
        int Sensor_Name = 0, WE = 1, AE = 2, ADC = 3, PPB = 4, Zero = 5, Span = 6, BSN = 7, FW = 8, RHT = 9;
        string[][] AllData = new string[10][];
        private void init_AllData()
        {
            for (int i = 0; i < 9; i++)
            {
                AllData[i] = new string[24];
                for (int j = 0; j < 24; j++)
                {
                    AllData[i][j] = "0";
                }
            }
            AllData[RHT] = new string[8];
            for (int i = 0; i < 8; i++)
            {
                AllData[RHT][i] = "0";
            }
        }
        /**********************************************************************************************************************
                                                            button
         **********************************************************************************************************************/
        private void btn_AlphFix_Connect_Click(object sender, EventArgs e)
        {
            if (btn_AlphFix_Connect.Text == "Connect")
            {
                try
                {
                    serialPort2.PortName = AlphFix_Port_box.Text;
                    serialPort2.BaudRate = 57600;

                    if (serialPort2.IsOpen)
                    {
                        label_AlphFix_State.Text = serialPort2.PortName + " port isn't open.";
                        btn_AlphFix_Connect.Text = "Disconnect";
                    }
                    else
                    {
                        serialPort2.Open();
                        if (serialPort2.IsOpen)
                        {
                            label_AlphFix_State.Text = serialPort2.PortName + " connection successful.";
                            btn_AlphFix_Connect.Text = "Disconnect";
                        }
                        else
                        {
                            label_AlphFix_State.Text = serialPort2.PortName + " Connection failed.";
                        }
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }

            else
            {
                serialPort2.Close();
                if (serialPort2.IsOpen)
                {
                    label_AlphFix_State.Text = serialPort2.PortName + " disconnection failed.";
                }
                else
                {
                    label_AlphFix_State.Text = "Choose port to connect.";
                    btn_AlphFix_Connect.Text = "Connect";
                }
            }
        }

        private void btn_AlphFix_stopORstart_Click(object sender, EventArgs e)
        {
            if (serialPort2.IsOpen)
            {
                if (btn_AlphFix_stopORstart.Text == "Start")
                {
                    AlphFix.pass_time = 1;
                    AlphFix.DisplayTimeCount = 1;
                    timer_getValue.Start();
                    btn_AlphFix_stopORstart.Text = "Stop";
                    AlphFix_Num_of_Sensor_box.Enabled = false;                    
                    AlphFix.NumOfSensor = Convert.ToInt16(AlphFix_Num_of_Sensor_box.Value);
                    AlphFix_unlock();
                }
                else
                {
                    AlphFix.pass_time = 0;
                    timer_getValue.Stop();
                    btn_AlphFix_stopORstart.Text = "Start";
                    btn_AlphFix_cap.Text = "Start";
                    label_AlphFix_time.Text = "00 : 00 : 00";
                    AlphFix.CapFlag = false;
                    AlphFix_Num_of_Sensor_box.Enabled = true;

                    AlphFix_lock();
                }
            }
            else
            {
                MessageBox.Show("未連線");
            }    
        }

        private void btn_AlphFix_cap_Click(object sender, EventArgs e)
        {
            if (btn_AlphFix_cap.Text == "Start")
            {
                AlphFix.FileName = AlphFix_FileName_box.Text;
                if (AlphFix.FileName == "")
                {
                    MessageBox.Show("請輸入檔名!");
                    return;
                }
                else
                {
                    AlphFix.pass_time = 0;
                    AlphFix.CsvTimeCount = 0;
                    AlphFix.CapTimeCount = 1;
                    AlphFix.CapFlag = true;
                    btn_AlphFix_cap.Text = "Stop";                   
                    string strResult = "";
                    strResult += "Computer_Time,time(min),";
                    for (int i = 0; i < 24; i++)
                    {
                        strResult += "we#" + AllData[Sensor_Name][i] + "_" + AllData[BSN][i] + ",";
                    }
                    for (int i = 0; i < 24; i++)
                    {
                        strResult += "ae#" + AllData[Sensor_Name][i] + "_" + AllData[BSN][i] + ",";
                    }
                    for (int i = 0; i < 24; i++)
                    {
                        strResult += "adc#" + AllData[Sensor_Name][i] + "_" + AllData[BSN][i] + ",";
                    }
                    strResult += "u4_Temp,u5_Temp,u6_Temp,u7_Temp,average_Temp,";
                    strResult += "u4_RH,u5_RH,u6_RH,u7_RH,average_RH,";
                    strResult += "u4_AH,u5_AH,u6_AH,u7_AH,average_AH\n";
                    using (StreamWriter strwrite = new StreamWriter("D:\\" + AlphFix.FileName + ".csv", false, Encoding.Default))
                    {
                        strwrite.Write(strResult);
                    }
                }
            }
            else
            {
                AlphFix.CapFlag = false;
                btn_AlphFix_cap.Text = "Start";
            }
        }
        /*********************************************************************************************************************************************************
                                                                           receive&send
        **********************************************************************************************************************************************************/
        private ManualResetEvent ResponseReceived2 = new ManualResetEvent(false);
        private void serialPort2_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {

            Console.WriteLine("DataReceived triggered!");
            if (btn_AlphFix_Connect.Text == "Disconnect")
            {

                AlphFix.RxString += serialPort2.ReadExisting();
                Console.WriteLine(AlphFix.RxString);

                AlphFix.RxInfo = AlphFix.RxString.Replace("\u0006", "[ACK]\n");
                AlphFix.RxInfo = AlphFix.RxInfo.Replace("\u000D", "[CR]\n");

                if (AlphFix.CommandMode == 0)
                {
                    if (AlphFix.RxString.Count(c => c == '\r') == 2)
                    {
                        ResponseReceived2.Set();
                    }
                }
                else
                {
                    if (AlphFix.RxString == "\u0006")
                    {
                        ResponseReceived2.Set();
                    }
                }
            }
            else { }
        }


        private int serialPort2_DataSend(string command, int mode)
        {
            AlphFix.CommandMode = mode;
            AlphFix.CmdStatus = 0;
            ResponseReceived2.Reset();
            int send_count = 0;

            while (send_count < 3)
            {
                if (serialPort2.IsOpen)
                {
                    Console.WriteLine(command);
                    AlphFix.RxString = serialPort2.ReadExisting();//清空buffer
                    AlphFix.RxString = "";
                    serialPort2.WriteLine(command + '\r' + '\n');
                    AlphFix_Info_box.AppendText(command + '\n');
                }
                else//結束流程
                {
                    timer_getValue.Stop();
                    MessageBox.Show("Alphasense未連線");
                    btn_AlphFix_stopORstart.Text = "Start";
                    btn_AlphFix_cap.Text = "Capture";
                    AlphFix_UpdateRate_Box.Enabled = true;
                    AlphFix_lock();                    
                    return 0;
                }

                if (mode == 0)//read
                {
                    if (ResponseReceived2.WaitOne(500))
                    {
                        AlphFix.CmdStatus = 1;
                        AlphFix_Info_box.AppendText(AlphFix.RxInfo);
                        return 1;  
                    }
                }
                else if (mode == 1)//write
                {
                    if (ResponseReceived2.WaitOne(5000))
                    {
                        AlphFix.CmdStatus = 1;
                        AlphFix_Info_box.AppendText(AlphFix.RxInfo);
                        return 1;  // 收到 \r，結束函式
                    }
                }
                send_count++;
            }

            MessageBox.Show("Alphasense沒有回應");
            return -1;
        }
        /*********************************************************************************************************************************************************
                                                                               tick
        **********************************************************************************************************************************************************/
        private void timer_getValue_Tick(object sender, EventArgs e)
        {
            timer_getValue.Interval = 1000;
            
            TimeSpan timeSpan = TimeSpan.FromSeconds(AlphFix.pass_time);
            if (AlphFix.DisplayTimeCount == 5)
            {
                //AlwaysRead();
                PageDispaly();
                AlphFix.DisplayTimeCount = 1;
            }
            else
            {
                AlphFix.DisplayTimeCount++;
            }
            if (AlphFix.CapFlag == true)
            {
                if (AlphFix.CapTimeCount == int.Parse(AlphFix_UpdateRate_Box.Text))
                {
                    AlphFix_Info_box.AppendText("cap\n");
                    AlphFix.CapTimeCount = 1;
                    float average = 0;
                    string strResult = "";
                    strResult += DateTime.Now.ToString("yyyy/mm/dd hh:mm:ss") + ',';
                    strResult += AlphFix.CsvTimeCount.ToString() + ',';
                    for (int i = 0; i < 24; i++)
                    {
                        strResult += AllData[WE][i] + ",";
                    }
                    for (int i = 0; i < 24; i++)
                    {
                        strResult += AllData[AE][i] + ",";
                    }
                    for (int i = 0; i < 24; i++)
                    {
                        strResult += AllData[ADC][i] + ",";
                    }
                    for (int i = 0; i < 4; i++)
                    {
                        strResult += AllData[RHT][2 * i] + ',';
                        average += float.Parse(AllData[RHT][2 * i]);
                    }
                    strResult += average.ToString() + ',';
                    average = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        strResult += AllData[RHT][2 * i + 1] + ',';
                        average += float.Parse(AllData[RHT][2 * i + 1]);
                    }
                    strResult += average.ToString() + ',';
                    average = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        strResult += label.AH[i].Text + ',';
                        average += float.Parse(label.AH[i].Text);
                    }
                    strResult += average.ToString() + '\n';
                    using (StreamWriter strwrite = new StreamWriter("D:\\" + AlphFix.FileName + ".csv", true, Encoding.Default))
                    {
                        strwrite.Write(strResult);
                    }

                    AlphFix.CsvTimeCount += float.Parse(AlphFix_UpdateRate_Box.Text) / 60;
                }
                else
                {
                    AlphFix.CapTimeCount++;
                }
                AlphFix.pass_time++;

                label_AlphFix_time.Text = $"{timeSpan.Hours:D2} : {timeSpan.Minutes:D2} : {timeSpan.Seconds:D2}";
            }
                
        }
        /*********************************************************************************************************************************************************
                                                                            other void
        **********************************************************************************************************************************************************/
        private void AlphFix_Port_box_DropDown(object sender, EventArgs e)
        {
            AlphFix_Port_box.Items.Clear();
            AlphFix_Port_box.Items.AddRange(SerialPort.GetPortNames());
        }

        string temp = "";
        private void AlwaysRead()
        {           
            if (serialPort2_DataSend("@getAlways", 0) != 1) { return; }
            else
            {
                temp = AlphFix.RxString.Replace("/r", "");
                string[] temp_list = temp.Split(':');
                AllData[Sensor_Name] = temp_list[0].Split(',');
                AllData[WE] = temp_list[1].Split(',');
                AllData[AE] = temp_list[2].Split(',');
                AllData[ADC] = temp_list[3].Split(',');
                AllData[PPB] = temp_list[4].Split(',');
                AllData[RHT] = temp_list[5].Split(',');
                for(int i = 0; i < 24; i++)
                {
                    AllData[Sensor_Name][i] = AlphFix.Sensor_Name[int.Parse(AllData[Sensor_Name][i])];
                }
                for (int i = 0; i < AllData[RHT].Length; i++)
                {
                    AllData[RHT][i] = (float.Parse(AllData[RHT][i]) / 100).ToString();
                }
            }

                       
        }

        private void PageDispaly()
        {
            for (int i = 0; i < 4; i++)
            {
                label.Temp[i].Text = (float.Parse(AllData[RHT][i * 2]) / 100).ToString() + "°C";
                label.RH[i].Text = (float.Parse(AllData[RHT][i * 2 + 1]) / 100).ToString();
                double Temp = double.Parse(AllData[RHT][i * 2]) / 100;
                double RH = double.Parse(AllData[RHT][i * 2 + 1]) / 100;
                double AH;
                AH = 6.112 * Math.Exp(17.62 * Temp / (243.12 + Temp)) * (RH / 100) * 100 / (461.52 * (273.15 + Temp));
                label.AH[i].Text = AH.ToString();
            }

            int num = AlphFix.NumOfSensor;
            switch (tabSelect.SelectedIndex)
            {
                case 0:
                    for (int i = 0; i < num; i++)
                    {
                        label.WE[i].Text = AllData[WE][i];
                        label.AE[i].Text = AllData[AE][i];
                    }
                    break;
                case 1:
                    for (int i = 0; i < num; i++)
                    {
                        label.ADC_Value[i].Text = AllData[PPB][i];
                        label.ADC[i].Text = AllData[ADC][i];
                    }
                    break;
                case 2:
                    if (serialPort2_DataSend("@getZero", 0) != 1) { return; }
                    else
                    {
                        temp = AlphFix.RxString.Replace("/r", "");
                        AllData[Zero] = temp.Split(',');
                        for (int i = 0; i < num; i++)
                        {
                            label.Zero_Value[i].Text = AllData[PPB][i];
                            label.Zero[i].Text = AllData[Zero][i];
                        }
                    }
                    break;
                case 3:
                    if (serialPort2_DataSend("@getSpan", 0) != 1) { return; }
                    else
                    {
                        temp = AlphFix.RxString.Replace("/r", "");
                        AllData[Span] = temp.Split(',');
                        for (int i = 0; i < num; i++)
                        {
                            label.Span_Value[i].Text = AllData[PPB][i];
                            label.Span[i].Text = AllData[Span][i];
                        }
                    }
                    break;
                case 4:
                    if (serialPort2_DataSend("@getBSN", 0) != 1) { return; }
                    else
                    {
                        temp = AlphFix.RxString.Replace("/r", "");
                        AllData[BSN] = temp.Split(',');
                        for (int i = 0; i < num; i++)
                        {
                            label.BSN_Value[i].Text = AllData[PPB][i];
                            label.BSN[i].Text = AllData[BSN][i];
                        }
                    }
                    break;
                case 5:
                    for (int i = 0; i < num; i++)
                    {
                        label.Sensor_Name[i].Text = AllData[Sensor_Name][i];
                    }
                    break;
                case 6:
                    if (serialPort2_DataSend("@getFW", 0) != 1) { return; }
                    else
                    {
                        temp = AlphFix.RxString.Replace("/r", "");
                        AllData[FW] = temp.Split(',');
                        for (int i = 0; i < num; i++)
                        {
                            label.FW[i].Text = AllData[FW][i];
                        }
                    }
                    break;
            }
        }

        private void AlphFix_lock()
        {
            btn_AlphFix_write.Enabled = true;
            btn_AlphFix_write.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_AutoSpan.Enabled = true;
            btn_AutoSpan.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_CalZero.Enabled = true;
            btn_CalZero.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_AlphFix_Log.Enabled = false;
            btn_AlphFix_Log.BackColor = SystemColors.ActiveBorder;
            btn_AlphFix_cap.Enabled = false;
            btn_AlphFix_cap.BackColor = SystemColors.ActiveBorder;
            for (int i = 0; i < 4; i++)
            {
                label.Temp[i].Text = "Temp";
                label.RH[i].Text = "Humidity";
                label.AH[i].Text = "AbsHumidity";
            }
            for (int i = 0; i < 24; i++)
            {
                label.WE[i].Text = "WE Value";
                label.WE[i].BackColor = SystemColors.ActiveBorder;
                label.AE[i].Text = "AE Value";
                label.AE[i].BackColor = SystemColors.ActiveBorder;
            }
        }

        private void AlphFix_unlock()
        {
            btn_AlphFix_write.Enabled = false;
            btn_AlphFix_write.BackColor = SystemColors.ActiveBorder;
            btn_AutoSpan.Enabled = false;
            btn_AutoSpan.BackColor = SystemColors.ActiveBorder;
            btn_CalZero.Enabled = false;
            btn_CalZero.BackColor = SystemColors.ActiveBorder;
            btn_AlphFix_Log.Enabled = true;
            btn_AlphFix_Log.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_AlphFix_cap.Enabled = true;
            btn_AlphFix_cap.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            for (int i = 0; i < AlphFix.NumOfSensor; i++)
            {
                label.WE[i].BackColor = Color.AntiqueWhite;
                label.AE[i].BackColor = Color.HotPink;
            }
        }

        private void AlphFix_Info_Box_TextChanged(object sender, EventArgs e)
        {
            AlphFix_Info_box.SelectionStart = this.AlphFix_Info_box.TextLength;
            AlphFix_Info_box.ScrollToCaret();
        }
        private void btn_test_Click(object sender, EventArgs e)
        {
            if (serialPort2_DataSend(test_box.Text + "\r\n", 0) != 1)
            {
                return;
            }
        }

        /*********************************************************************************************************************************************************
                                                                              initial
        **********************************************************************************************************************************************************/
        private void InitializeLabelList()
        {
            label.WE = new List<Label> { 
                label_WE0, label_WE1, label_WE2, label_WE3, label_WE4, label_WE5, label_WE6, label_WE7, label_WE8, label_WE9,
                label_WE10, label_WE11, label_WE12, label_WE13, label_WE14, label_WE15, label_WE16, label_WE17, label_WE18, label_WE19,
                label_WE20, label_WE21, label_WE22, label_WE23
            };

            label.AE = new List<Label> { 
                label_AE0, label_AE1, label_AE2, label_AE3, label_AE4, label_AE5, label_AE6, label_AE7, label_AE8, label_AE9,
                label_AE10, label_AE11, label_AE12, label_AE13, label_AE14, label_AE15, label_AE16, label_AE17, label_AE18, label_AE19,
                label_AE20, label_AE21, label_AE22, label_AE23
            };

            label.ADC_Value = new List<Label> { 
                label_ADC_Value0, label_ADC_Value1, label_ADC_Value2, label_ADC_Value3, label_ADC_Value4, label_ADC_Value5, label_ADC_Value6, label_ADC_Value7, label_ADC_Value8, label_ADC_Value9,
                label_ADC_Value10, label_ADC_Value11, label_ADC_Value12, label_ADC_Value13, label_ADC_Value14, label_ADC_Value15, label_ADC_Value16, label_ADC_Value17, label_ADC_Value18, label_ADC_Value19,
                label_ADC_Value20, label_ADC_Value21, label_ADC_Value22, label_ADC_Value23
            };

            label.ADC = new List<Label> { 
                label_ADC0, label_ADC1, label_ADC2, label_ADC3, label_ADC4, label_ADC5,label_ADC6,label_ADC7,label_ADC8,label_ADC9,
                label_ADC10, label_ADC11, label_ADC12, label_ADC13, label_ADC14, label_ADC15,label_ADC16,label_ADC17,label_ADC18,label_ADC19,
                label_ADC20, label_ADC21, label_ADC22, label_ADC23
            };

            label.Zero_Value = new List<Label> { 
                label_Zero_Value0, label_Zero_Value1, label_Zero_Value2, label_Zero_Value3, label_Zero_Value4, label_Zero_Value5, label_Zero_Value6, label_Zero_Value7, label_Zero_Value8, label_Zero_Value9,
                label_Zero_Value10, label_Zero_Value11, label_Zero_Value12, label_Zero_Value13, label_Zero_Value14, label_Zero_Value15, label_Zero_Value16, label_Zero_Value17, label_Zero_Value18, label_Zero_Value19,
                label_Zero_Value20, label_Zero_Value21, label_Zero_Value22, label_Zero_Value23
            };

            label.Zero = new List<Label> {
                label_Zero0, label_Zero1, label_Zero2, label_Zero3, label_Zero4, label_Zero5, label_Zero6, label_Zero7, label_Zero8, label_Zero9,
                label_Zero10, label_Zero11, label_Zero12, label_Zero13, label_Zero14, label_Zero15, label_Zero16, label_Zero17, label_Zero18, label_Zero19,
                label_Zero20, label_Zero21, label_Zero22, label_Zero23
            };

            label.Span_Value = new List<Label> { 
                label_Span_Value0, label_Span_Value1, label_Span_Value2, label_Span_Value3, label_Span_Value4, label_Span_Value5, label_Span_Value6, label_Span_Value7, label_Span_Value8, label_Span_Value9,
                label_Span_Value10, label_Span_Value11, label_Span_Value12, label_Span_Value13, label_Span_Value14, label_Span_Value15, label_Span_Value16, label_Span_Value17, label_Span_Value18, label_Span_Value19,
                label_Span_Value20, label_Span_Value21, label_Span_Value22, label_Span_Value23
            };

            label.Span = new List<Label> { 
                label_Span0, label_Span1, label_Span2, label_Span3, label_Span4, label_Span5, label_Span6, label_Span7, label_Span8, label_Span9,
                label_Span10, label_Span11, label_Span12, label_Span13, label_Span14, label_Span15, label_Span16, label_Span17, label_Span18, label_Span19,
                label_Span20, label_Span21, label_Span22, label_Span23
            };

            label.BSN_Value = new List<Label> { 
                label_BSN_Value0, label_BSN_Value1, label_BSN_Value2, label_BSN_Value3, label_BSN_Value4, label_BSN_Value5, label_BSN_Value6, label_BSN_Value7, label_BSN_Value8, label_BSN_Value9,
                label_BSN_Value10, label_BSN_Value11, label_BSN_Value12, label_BSN_Value13, label_BSN_Value14, label_BSN_Value15, label_BSN_Value16, label_BSN_Value17, label_BSN_Value8, label_BSN_Value19,
                label_BSN_Value20, label_BSN_Value21, label_BSN_Value22, label_BSN_Value23
            };

            label.BSN = new List<Label> { 
                label_BSN0, label_BSN1, label_BSN2,  label_BSN3, label_BSN4, label_BSN5, label_BSN6, label_BSN7, label_BSN8, label_BSN9,
                label_BSN10, label_BSN11, label_BSN12,  label_BSN13, label_BSN14, label_BSN15, label_BSN16, label_BSN17, label_BSN18, label_BSN19,
                label_BSN20, label_BSN21, label_BSN22,  label_BSN23
            };

            label.Sensor_Name = new List<Label> { 
                label_Sensor_Name0, label_Sensor_Name1,  label_Sensor_Name2, label_Sensor_Name3, label_Sensor_Name4, label_Sensor_Name5, label_Sensor_Name6, label_Sensor_Name7, label_Sensor_Name8, label_Sensor_Name9,
                label_Sensor_Name10, label_Sensor_Name11,  label_Sensor_Name12, label_Sensor_Name13, label_Sensor_Name14, label_Sensor_Name15, label_Sensor_Name16, label_Sensor_Name17, label_Sensor_Name18, label_Sensor_Name19,
                label_Sensor_Name20, label_Sensor_Name21,  label_Sensor_Name22, label_Sensor_Name23
            };

            label.Sensor_Name = new List<Label> { 
                label_FW_0, label_FW_1, label_FW_2, label_FW_3, label_FW_4, label_FW_5, label_FW_6, label_FW_7, label_FW_8, label_FW_9,
                label_FW_10, label_FW_11, label_FW_12, label_FW_13, label_FW_14, label_FW_15, label_FW_16, label_FW_17, label_FW_18, label_FW_19,
                label_FW_20, label_FW_21, label_FW_22, label_FW_23
            };
            label.Temp = new List<Label> { 
                label_Temp0, label_Temp1, label_Temp2, label_Temp3
            };

            label.RH = new List<Label> { 
                label_RH0, label_RH1, label_RH2, label_RH3
            };

            label.AH = new List<Label> { 
                label_AH0, label_AH1, label_AH2, label_AH3
            };

            label.WEAE_LED = new List<Label> {
                /*weae_LED0, weae_LED1, weae_LED1, weae_LED3, weae_LED4, weae_LED5, weae_LED6, weae_LED7, weae_LED8, weae_LED9, 
                weae_LED10, weae_LED11, weae_LED12, weae_LED13, weae_LED14, weae_LED15, weae_LED16, weae_LED17, weae_LED18, weae_LED19,
                weae_LED20, weae_LED21, weae_LED22, weae_LED23*/
            };
        }
        /*****************************************************************************************************************************************************************
         *****************************************************************************************************************************************************************
                                                                               SABIO
         *****************************************************************************************************************************************************************
         *****************************************************************************************************************************************************************/
        /*********************************************************************************************************************
                                                            button
        **********************************************************************************************************************/
        private void btn_Sabio_Connect_Click(object sender, EventArgs e)
        {
            if (btn_Sabio_Connect.Text == "Connect")
            {
                try
                {
                    serialPort1.PortName = Sabio_Port_box.Text;
                    serialPort1.BaudRate = int.Parse(Sabio_Baud_box.Text);

                    if (serialPort1.IsOpen)
                    {
                        label_Sabio_State.Text = serialPort1.PortName + " port isent openned.";
                        btn_Sabio_Connect.Text = "Disconnect";
                    }
                    else
                    {
                        serialPort1.Open();
                        if (serialPort1.IsOpen)
                        {
                            label_Sabio_State.Text = serialPort1.PortName + " connect success.";
                            btn_Sabio_Connect.Text = "Disconnect";
                        }
                        else
                        {
                            label_Sabio_State.Text = serialPort1.PortName + " Connect unsuccess.";
                        }
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }

            else
            {
                serialPort1.Close();
                if (serialPort1.IsOpen)
                {
                    label_Sabio_State.Text = serialPort1.PortName + " disconnect unsuccess.";
                }
                else
                {
                    label_Sabio_State.Text = "Coose port to connect.";
                    btn_Sabio_Connect.Text = "Connect";
                }
            }
        }
        private void btn_MS_Click(object sender, EventArgs e)
        {
            ListViewItem item = new ListViewItem(new string[] { "@MS," + Addr_Box.Text + "," + Sequence_Box.Text + "," + Point_Box.Text, Sabio_Time_Box.Text });
            listView1.Items.Add(item);
        }


        private void btn_TS_Click(object sender, EventArgs e)
        {
            ListViewItem item = new ListViewItem(new string[] { "@TS," + Addr_Box.Text + "," + Sequence_Box.Text + "," + Point_Box.Text, Sabio_Time_Box.Text });
            listView1.Items.Add(item);
        }

        private void btn_CmdStart_Click(object sender, EventArgs e)
        {
            Sabio.CmdTimeCount = 0;
            Sabio.CmdIndex = 0;
            timer_command.Start();
            Sabio_lock_btn();
        }

        private void btn_CmdStop_Click(object sender, EventArgs e)
        {
            StopCmdList();
        }

        private void btn_ClnList_Click(object sender, EventArgs e)
        {
            listView1.Items.Clear();
        }

        private void btn_Sabio_cap_Click(object sender, EventArgs e)
        {
            if (btn_Sabio_cap.Text == "Capture")
            {
                if (FileName_Box.Text == "")
                {
                    MessageBox.Show("請輸入檔名!");
                    return;
                }
                else
                {
                    btn_Sabio_cap.Text = "Stop";
                    Sabio_UpdateRate_Box.Enabled = false;
                    Sabio.CsvTimeCount = 0;
                    Sabio.FileName = FileName_Box.Text;
                    string strResult = "Computer Time,Time,Diluted Gases,Diluent MFC (Control),Diluent MFC (Monitor),Source MFC (Control),Source MFC (Monitor),Total Flow(Control),Total Flow(Monitor),Source\n";
                    using (StreamWriter strwrite = new StreamWriter("D:\\" + Sabio.FileName + ".csv", false, Encoding.Default))
                    {
                        strwrite.Write(strResult);
                    }
                    timer_getStatus.Start();
                }
            }
            else
            {
                btn_Sabio_cap.Text = "Capture";
                Sabio_UpdateRate_Box.Enabled = true;
                timer_getStatus.Stop();
            }
        }

        private void btn_CheckNow_Click(object sender, EventArgs e)
        {
            get_Status();
        }

        private void btn_Purge_Click(object sender, EventArgs e)
        {
            Sabio.CommandMode = 3;
            serialPort1_DataSend("@P,001");
            if (Sabio.CmdStatus != 1)
            {
                return;
            }
            get_Status();
        }

        bool purgelock = true;
        private void btn_PurgeLock_Click(object sender, EventArgs e)
        {
            if (purgelock == true)
            {
                purgelock = false;
                btn_PurgeLock.BackgroundImage = Properties.Resources.unlock;
                btn_Purge.Enabled = true;
                btn_Purge.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            }
            else
            {
                purgelock = true;
                btn_PurgeLock.BackgroundImage = Properties.Resources._lock;
                btn_Purge.Enabled = false;
                btn_Purge.BackColor = SystemColors.ActiveBorder;
            }
        }

        /*********************************************************************************************************************
                                   receive & write
        **********************************************************************************************************************/
        private ManualResetEvent ResponseReceived = new ManualResetEvent(false);
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {

            Console.WriteLine("DataReceived triggered!");
            if (btn_Sabio_Connect.Text == "Disconnect")
            {

                Sabio.RxString += serialPort1.ReadExisting();
                Console.WriteLine(Sabio.RxString);

                Sabio.RxInfo = Sabio.RxString.Replace("\u0006", "[ACK]\n");
                Sabio.RxInfo = Sabio.RxInfo.Replace("\u000D", "[CR]\n");

                if (Sabio.CommandMode == 0 || Sabio.CommandMode == 1)
                {
                    if (Sabio.RxString.Count(c => c == '\r') == 2)
                    {
                        ResponseReceived.Set();
                    }
                }
                else if (Sabio.CommandMode == 2)
                {
                    if (Sabio.RxString.Count(c => c == '\r') == 3)
                    {
                        ResponseReceived.Set();
                    }
                }
                else if (Sabio.CommandMode == 3)
                {
                    if (Sabio.RxString == "\u0006")
                    {
                        ResponseReceived.Set();
                    }
                }
            }
            else { }
        }


        private void serialPort1_DataSend(string command)
        {
            Sabio.CmdStatus = 0;
            ResponseReceived.Reset();
            int send_count = 0;

            while (send_count < 5)
            {
                if (serialPort1.IsOpen)
                {
                    Console.WriteLine(command);
                    Sabio.RxString = serialPort1.ReadExisting();//清空buffer
                    Sabio.RxString = "";
                    serialPort1.WriteLine(command + '\r' + '\n');
                    Sabio_Info_Box.AppendText(command + '\n');
                }
                else//結束流程
                {
                    Sabio.CmdIndex = 0;
                    Sabio.CmdTimeCount = 0;

                    Sabio_unlock_btn();
                    btn_Sabio_cap.Text = "Capture";
                    Sabio_UpdateRate_Box.Enabled = true;
                    timer_command.Stop();
                    timer_getStatus.Stop();
                    MessageBox.Show("Sabio 4010未連線");
                    return;
                }

                if (Sabio.CommandMode == 3)//ms/ts/stop/purge
                {
                    if (ResponseReceived.WaitOne(5000))//要等比較久
                    {
                        Sabio.CmdStatus = 1;
                        Sabio_Info_Box.AppendText(Sabio.RxInfo);
                        return;  // 收到 ACK，結束函式
                    }
                }
                else if (Sabio.CommandMode == 0 || Sabio.CommandMode == 1 || Sabio.CommandMode == 2)//GetStatus
                {
                    if (ResponseReceived.WaitOne(1000))//讀資料比較快
                    {
                        Sabio.CmdStatus = 1;
                        Sabio_Info_Box.AppendText(Sabio.RxInfo);
                        return;  // 收到 \r，結束函式
                    }
                }
                send_count++;
            }

            Sabio.CmdStatus = -1; // 4010沒有回應
            timer_command.Stop();
            timer_getStatus.Stop();
            MessageBox.Show("Sabio 4010沒有回應");
        }


        /*********************************************************************************************************************
                                         clock
        **********************************************************************************************************************/
        private void timer_command_Tick(object sender, EventArgs e)
        {
            timer_command.Interval = 1000;
            if (Sabio.CmdTimeCount == 0 && Sabio.CmdIndex < listView1.Items.Count)
            {
                ListViewItem item = listView1.Items[Sabio.CmdIndex]; // 取得第一個項目
                string command = item.Text; // 第一列
                int time = int.Parse(item.SubItems[1].Text) * 60;
                Sabio.CmdTimeCount = time;

                timer_command.Stop();
                //timer_getStatus.Stop();
                Sabio.CommandMode = 3;
                serialPort1_DataSend(command);
                if (Sabio.CmdStatus != 1)
                {
                    return;
                }
                else
                {
                    timer_command.Start();
                    //timer_getStatus.Start();
                    Sabio.CmdIndex++;
                }
            }
            else if (Sabio.CmdTimeCount == 0 && Sabio.CmdIndex >= listView1.Items.Count)
            {
                StopCmdList();
            }
            else
            {
                Sabio.CmdTimeCount--;
                listView1.Items[Sabio.CmdIndex - 1].SubItems[1].Text = (Sabio.CmdTimeCount / 60).ToString() + ":" + (Sabio.CmdTimeCount % 60).ToString();
            }
        }

        private void timer_getStatus_Tick(object sender, EventArgs e)
        {
            timer_getStatus.Interval = int.Parse(Sabio_UpdateRate_Box.Text) * 1000;

            get_Status();

            string strResult = string.Empty;
            strResult += DateTime.Now.ToString("yyyy/mm/dd hh:mm:ss") + ',';
            strResult += Sabio.CsvTimeCount.ToString() + ',';
            strResult += label_Status2.Text + ',';
            strResult += label_Measure1.Text + ',';
            strResult += label_Measure2.Text + ',';
            strResult += label_Measure3.Text + ',';
            strResult += label_Measure4.Text + ',';
            strResult += label_Measure5.Text + ',';
            strResult += label_Measure6.Text + ',';
            strResult += label_Status1.Text + '\n';
            using (StreamWriter strwrite = new StreamWriter("D:\\" + Sabio.FileName + ".csv", true, Encoding.Default))
            {
                strwrite.Write(strResult);
            }

            Sabio.CsvTimeCount += float.Parse(Sabio_UpdateRate_Box.Text) / 60;
        }
        /*********************************************************************************************************************
                                         other
        **********************************************************************************************************************/
        private void display_info()
        {
            Sabio_Info_Box.AppendText(Sabio.RxInfo);
        }
        

        //固定欄寬
        private void listView1_ColumnWidthChanging(object sender, ColumnWidthChangingEventArgs e)
        {
            try
            {
                e.Cancel = true;
                e.NewWidth = listView1.Columns[e.ColumnIndex].Width;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void StopCmdList()
        {
            Sabio.CmdIndex = 0;
            Sabio.CmdStatus = 0;
            Sabio.CmdTimeCount = 0;

            Sabio.CommandMode = 3;
            serialPort1_DataSend("@S,001");
            if (Sabio.CmdStatus != 1)
            {
                return;
            }
            timer_command.Stop();
            Sabio_unlock_btn();
        }

        private void get_Status()
        {
            Sabio.CommandMode = 0;
            serialPort1_DataSend("@GS,001,D");
            if (Sabio.CmdStatus != 1)
            {
                timer_getStatus.Stop();
                return;
            }
            string dilution = Sabio.RxString;
            dilution = dilution.Replace("\r", "");
            string[] dilution_list = dilution.Split(',');
            if (dilution_list.Length != 11)
            {
                Array.Resize(ref dilution_list, 11);
                for (int i = dilution_list.Length - 1; i < 11; i++)
                {
                    dilution_list[i] = "Miss";
                }
            }
            if (dilution_list[4] == "0.0" || dilution_list[4] == "Miss")
            {
                Sabio.CommandMode = 1;
            }
            else
            {
                Sabio.CommandMode = 2;
            }
            /******************************************************/
            serialPort1_DataSend("@GS,001,G");
            if (Sabio.CmdStatus != 1)
            {
                timer_getStatus.Stop();
                return;
            }
            string gas = Sabio.RxString;
            gas = gas.Replace("\r", "");
            string[] gas_list = gas.Split(',');

            /*******************************************************/
            string strResult = string.Empty;
            gas_list[0] = (float.Parse(gas_list[0]) / 1000).ToString();
            if (gas_list.Length == 3)
            {
                label_Status1.Text = "Nothing";
                label_Status2.Text = "0.0";
                label_Measure6.Text = gas_list[0];
            }
            else if (gas_list.Length < 3)
            {
                label_Status1.Text = "Miss";
                label_Status2.Text = "Miss";
                label_Measure6.Text = "Miss";
            }
            else
            {
                label_Status1.Text = gas_list[2];
                label_Status2.Text = (float.Parse(gas_list[3]) / 1000).ToString();
                label_Measure6.Text = gas_list[0];
            }
            label_Measure1.Text = dilution_list[0] != null ? dilution_list[0] : "Miss";
            label_Measure2.Text = dilution_list[1] != null ? dilution_list[1] : "Miss";
            label_Measure3.Text = dilution_list[4] != null ? dilution_list[4] : "Miss";
            label_Measure4.Text = dilution_list[5] != null ? dilution_list[5] : "Miss";
            label_Measure5.Text = dilution_list[4] != null ? (float.Parse(dilution_list[0]) + float.Parse(dilution_list[4]) / 1000).ToString() : "Miss";

            LEDFlash();
        }

        private void Sabio_lock_btn()
        {
            btn_CmdStart.Enabled = false;
            btn_CmdStart.BackColor = System.Drawing.SystemColors.ActiveBorder;
            btn_MS.Enabled = false;
            btn_MS.BackColor = System.Drawing.SystemColors.ActiveBorder;
            btn_TS.Enabled = false;
            btn_TS.BackColor = System.Drawing.SystemColors.ActiveBorder;
            btn_ClnList.Enabled = false;
            btn_ClnList.BackColor = System.Drawing.SystemColors.ActiveBorder;
        }

        private void Sabio_unlock_btn()
        {
            btn_CmdStart.Enabled = true;
            btn_CmdStart.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_MS.Enabled = true;
            btn_MS.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_TS.Enabled = true;
            btn_TS.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_ClnList.Enabled = true;
            btn_ClnList.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
        }

        private void Sabio_Port_box_DropDown(object sender, EventArgs e)
        {
            Sabio_Port_box.Items.Clear();
            Sabio_Port_box.Items.AddRange(SerialPort.GetPortNames());
        }

        private void Info_Box_TextChanged(object sender, EventArgs e)
        {
            Sabio_Info_Box.SelectionStart = this.Sabio_Info_Box.TextLength;
            Sabio_Info_Box.ScrollToCaret();
        }

        private void brn_InfoCln_Click(object sender, EventArgs e)
        {
            Sabio_Info_Box.Clear();
        }

        private async void LEDFlash()
        {
            Update_LED.ForeColor = Color.IndianRed;
            await Task.Delay(500);
            Update_LED.ForeColor = SystemColors.ControlLight;
        }
    }
}
