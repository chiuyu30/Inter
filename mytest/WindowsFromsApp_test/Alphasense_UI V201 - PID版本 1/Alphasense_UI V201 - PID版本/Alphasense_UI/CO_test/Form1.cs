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
using System.Runtime.InteropServices;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.TaskbarClock;

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

            AlphFix_Port_box.Items.Clear();
            AlphFix_Port_box.Items.AddRange(SerialPort.GetPortNames());
            Sabio_Port_box.Items.Clear();
            Sabio_Port_box.Items.AddRange(SerialPort.GetPortNames());
            Item_box.Text = Item_box.Items[0].ToString();
            Sabio_UpdateRate_Box.Text = Sabio_UpdateRate_Box.Items[0].ToString();
            Sabio.CmdStatus = 0;
            Sabio.O3Flag = false;
            AlphFix_UpdateRate_Box.Text = AlphFix_UpdateRate_Box.Items[0].ToString();
            AlphFix_lock();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                serialPort1.Write("@S,001\r\n");
            }
            catch (Exception ex)
            {
                Console.WriteLine("寫入錯誤：" + ex.Message);
            }
            finally
            {
                serialPort1.Close();
                serialPort1.Dispose();
            }
        }

        /*****************************************************************************************************************************************************************
         *****************************************************************************************************************************************************************
                                                                                        SENSOR
         *****************************************************************************************************************************************************************
         *****************************************************************************************************************************************************************/
        int GasName = 0, WE = 1, AE = 2, ADC = 3, PPB = 4, Zero = 5, Span = 6, BSN = 7, FW = 8, A = 9, B = 10, CalDate = 11, RHT = 12;
        float[] AccValue = new float[24];
        float[] AverageValue = new float[24];
        int read = 0, write = 1;
        string[][] AllData = new string[13][];
        private void init_AllData()
        {
            for (int i = 0; i < 12; i++)
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
            for(int i = 0; i < 24; i++)
            {
                AccValue[i] = 0;
                AverageValue[i] = 0;
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
                    init_AllData();
                    if (InitRead() == 1) { return; }
                    AlphFix.pass_time = 1;
                    AlphFix.DisplayTimeCount = 1;
                    timer_getValue.Start();
                    btn_AlphFix_stopORstart.Text = "Stop";
                    AlphFix_unlock();
                }
                else
                {
                    AlphFix.pass_time = 0;
                    timer_getValue.Stop();
                    btn_AlphFix_stopORstart.Text = "Start";
                    btn_AlphFix_cap.Text = "Start Cap";
                    label_AlphFix_time.Text = "00 : 00 : 00";
                    AlphFix.CapFlag = false;

                    AlphFix_lock();
                }
            }
            else
            {
                MessageBox.Show("未連線");
            }    
        }

        private void btn_AlphFix_Log_Click(object sender, EventArgs e)
        {
            string strResult = "BSN,WE,AE,PPB,ADC,Zero,Span,a(Slope),b(intercept),Gas Name,FW ver,Cal Date\n";
            for(int i = 0; i < 24; i++)
            {
                strResult += AllData[BSN][i] + ',';
                strResult += AllData[WE][i] + ',';
                strResult += AllData[AE][i] + ',';
                strResult += AllData[PPB][i] + ',';
                strResult += AllData[ADC][i] + ',';
                strResult += AllData[Zero][i] + ',';
                strResult += AllData[Span][i] + ',';
                strResult += AllData[A][i] + ',';
                strResult += AllData[B][i] + ',';
                strResult += AllData[GasName][i] + ',';
                strResult += AllData[FW][i] + ',';
                strResult += AllData[CalDate][i] + ',';
                strResult += "\n";
            }
            
            strResult = strResult.Replace(",0", ",----");
            strResult = strResult.Replace("\n0", "\n----");

            using (StreamWriter strwrite = new StreamWriter("D:\\Logging Data.csv", false, Encoding.Default))
            {
                strwrite.Write(strResult);
            }
            GC.Collect();//強制回收多餘的物件
        }

        private void btn_AlphFix_cap_Click(object sender, EventArgs e)
        {
            if (btn_AlphFix_cap.Text == "Start Cap")
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
                    timer_alcap.Start();
                    btn_AlphFix_cap.Text = "Stop Cap";                   
                    string strResult = "";
                    strResult += "Computer_Time,time(min),";
                    for (int i = 0; i < 24; i++)
                    {
                        strResult += "we#" + AllData[GasName][i] + "_" + AllData[BSN][i] + ",";
                    }
                    for (int i = 0; i < 24; i++)
                    {
                        strResult += "ae#" + AllData[GasName][i] + "_" + AllData[BSN][i] + ",";
                    }
                    for (int i = 0; i < 24; i++)
                    {
                        strResult += "adc#" + AllData[GasName][i] + "_" + AllData[BSN][i] + ",";
                    }
                    for (int i = 0; i < 24; i++)
                    {
                        strResult += "ppb#" + AllData[GasName][i] + "_" + AllData[BSN][i] + ",";
                    }
                    strResult += "u4_Temp,u5_Temp,u6_Temp,u7_Temp,average_Temp,";
                    strResult += "u4_RH,u5_RH,u6_RH,u7_RH,average_RH,";
                    strResult += "u4_AH,u5_AH,u6_AH,u7_AH,average_AH\n";
                    using (StreamWriter strwrite = new StreamWriter("D:\\" + AlphFix.FileName + ".csv", false, Encoding.Default))
                    {
                        strwrite.Write(strResult);
                    }
                    GC.Collect();//強制回收多餘的物件
                }
            }
            else
            {
                AlphFix.CapFlag = false;
                timer_alcap.Stop();
                label_Sabio_time.Text = "00 : 00 : 00";
                btn_AlphFix_cap.Text = "Start Cap";
            }
        }

        private void btn_AlphFix_read_Click(object sender, EventArgs e)
        {
            label_Response.Text = "Wait";
            string ID = ((Int16)ID_box.Value).ToString("D2");
            string fun = "";
            int id = Decimal.ToInt32(ID_box.Value);
            if (Item_box.Text == "CAL DATE")
            {
                fun = "CAL";
            }
            else
            {
                fun = Item_box.Text;
            }

            if(serialPort2_DataSend("@RO," + fun + "," + ID, read) != 1)
            {
                label_Response.Text = "Read Fail!";
                return;
            }
            else
            {
                temp = AlphFix.RxString.Replace("\r", "");
                temp = temp.Replace(",", "");
                switch (fun)
                {
                    case "BSN":
                        AllData[BSN][id] = temp;
                        label_Response.Text = "Read Success!\nBSN_" + id.ToString() + " = " + temp;
                        break;
                    case "CAL":
                        AllData[CalDate][id] = temp;
                        label_Response.Text = "Read Success!\nCalDate_" + id.ToString() + " = " + temp;
                        break;
                    case "ZERO":
                        AllData[Zero][id] = temp;
                        label_Response.Text = "Read Success!\nZero_" + id.ToString() + " = " + temp;
                        break;
                    case "SPAN":
                        AllData[Span][id] = (float.Parse(temp)/1000).ToString();
                        label_Response.Text = "Read Success!\nSpan_" + id.ToString() + " = " + temp;
                        break;
                    case "A":
                        AllData[A][id] = (float.Parse(temp) / 1000).ToString();
                        label_Response.Text = "Read Success!\nA_" + id.ToString() + " = " + temp;
                        break;
                    case "B":
                        AllData[B][id] = temp;
                        label_Response.Text = "Read Success!\nB_" + id.ToString() + " = " + temp;
                        break;
                }
            }
        }

        private void btn_AlphFix_write_Click(object sender, EventArgs e)
        {
            label_Response.Text = "Wait";
            string ID = ((Int16)ID_box.Value).ToString("D2");
            string fun = "";
            int id = Decimal.ToInt32(ID_box.Value);
            if (Item_box.Text == "CAL DATE")
            {
                fun = "CAL";
            }
            else
            {
                fun = Item_box.Text;
            }

            if (serialPort2_DataSend("@W," + fun + "," + ID + "," + Value_box.Text, write) != 1)
            {
                label_Response.Text = "Write Fail!";
                return;
            }
            else
            {
                switch (fun)
                {
                    case "BSN":
                        AllData[BSN][id] = temp;
                        label_Response.Text = "Write Success!";
                        break;
                    case "CAL":
                        AllData[CalDate][id] = temp;
                        label_Response.Text = "Write Success!";
                        break;
                    case "ZERO":
                        AllData[Zero][id] = temp;
                        label_Response.Text = "Write Success!";
                        break;
                    case "SPAN":
                        AllData[Span][id] = temp;
                        label_Response.Text = "Write Success!";
                        break;
                    case "A":
                        AllData[A][id] = temp;
                        label_Response.Text = "Write Success!";
                        break;
                    case "B":
                        AllData[B][id] = temp;
                        label_Response.Text = "Write Success!";
                        break;
                }
            }
        }

        private void btn_CalZero_Click(object sender, EventArgs e)
        {
            label_Response.Text = "Wait";
            if (AutoZero() == 1)
            {
                label_Response.Text = "AutoZero Fail!";
                return;
            }
            label_Response.Text = "AutoZero Success!";
            InitRead();
        }

        private void btn_AutoSpan_Click(object sender, EventArgs e)
        {            
            label_Response.Text = "Wait";
            if(AutoSpan() == 1)
            {
                label_Response.Text = "AutoSpan Fail!";
                return;
            } 
            label_Response.Text = "AutoSpan Success!";
            InitRead();
        }

        private void btn_InitALL_Click(object sender, EventArgs e)
        {
            InitRead();
            label_Response.Text = "Wait";
            for (int i = 0; i < 24; i++)
            {
                if (AllData[GasName][i] != "0")
                {
                    if (serialPort2_DataSend("@W,ZERO," + i.ToString("D2") + ",0", write) != 1)
                    {
                        label_Response.Text = "InitALL Fail!";
                        return;
                    }
                    if (serialPort2_DataSend("@W,SPAN," + i.ToString("D2") + ",0", write) != 1)
                    {
                        label_Response.Text = "InitALL Fail!";
                        return;
                    }
                    if (serialPort2_DataSend("@W,A," + i.ToString("D2") + ",1", write) != 1)
                    {
                        label_Response.Text = "InitALL Fail!";
                        return;
                    }
                    if (serialPort2_DataSend("@W,B," + i.ToString("D2") + ",0", write) != 1)
                    {
                        label_Response.Text = "InitALL Fail!";
                        return;
                    }
                }
            }
            label_Response.Text = "InitALL Success!";
            InitRead();
        }

        /*********************************************************************************************************************************************************
                                                                           receive&send
        **********************************************************************************************************************************************************/
        private ManualResetEvent ResponseReceived2 = new ManualResetEvent(false);
        private void serialPort2_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {

            //Console.WriteLine("DataReceived triggered!");
            if (btn_AlphFix_Connect.Text == "Disconnect")
            {

                AlphFix.RxString += serialPort2.ReadExisting();
                

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
                    timer_alcap.Stop();
                    MessageBox.Show("Alphasense未連線");
                    btn_AlphFix_stopORstart.Text = "Start";
                    btn_AlphFix_cap.Text = "Capture";
                    AlphFix_UpdateRate_Box.Enabled = true;
                    AlphFix_lock();                    
                    return 0;
                }

                if (mode == 0)//read
                {
                    if (ResponseReceived2.WaitOne(1000))
                    {
                        AlphFix.CmdStatus = 1;
                        Console.WriteLine(AlphFix.RxInfo);
                        AlphFix_Info_box.AppendText(AlphFix.RxInfo);
                        return 1;  
                    }
                }
                else if (mode == 1)//write
                {
                    if (ResponseReceived2.WaitOne(5000))
                    {
                        AlphFix.CmdStatus = 1;
                        Console.WriteLine(AlphFix.RxInfo);
                        AlphFix_Info_box.AppendText(AlphFix.RxInfo);
                        return 1;  // 收到 ack，結束函式
                    }
                }
                send_count++;
            }
            AlphFix.pass_time = 0;
            timer_getValue.Stop();
            timer_alcap.Stop();
            AlphFix.CapFlag = false;

            MessageBox.Show("Alphasense沒有回應");

            btn_AlphFix_stopORstart.Text = "Start";
            btn_AlphFix_cap.Text = "Start";
            label_AlphFix_time.Text = "00 : 00 : 00";
            AlphFix_lock();
            return -1;
        }
        /*********************************************************************************************************************************************************
                                                                               tick
        **********************************************************************************************************************************************************/
        private void timer_getValue_Tick(object sender, EventArgs e)
        {
            timer_getValue.Interval = 1000;
                        
            if (AlphFix.DisplayTimeCount == 5)
            {
                AlphFix.DisplayTimeCount = 1;
                timer_getValue.Stop();
                if (AlwaysRead() == 1) { return; }
                PageDispaly();
                
                for (int i = 0; i < 24; i++)
                {
                    AccValue[i] += float.Parse(AllData[ADC][i])/1000000;//除1000000，避免累計值太大溢位
                }
                AlphFix.AverageTimeCount += 1;
                if (AlphFix.AverageTimeCount == 12)
                {
                    for (int i = 0; i < 24; i++)
                    {
                        AverageValue[i] = AccValue[i]/12;
                        AverageValue[i] = AverageValue[i] * 1000000;
                        AccValue[i] = 0;
                    }
                    AlphFix.AverageTimeCount = 0;
                }
                timer_getValue.Start();
            }
            AlphFix.DisplayTimeCount++;
        }

        string strResult;
        private void timer_alcap_Tick(object sender, EventArgs e)
        {
            timer_alcap.Interval = 1000;

            TimeSpan timeSpan = TimeSpan.FromSeconds(AlphFix.pass_time);

            if (AlphFix.CapTimeCount == int.Parse(AlphFix_UpdateRate_Box.Text))
            {
                AlphFix_Info_box.AppendText("cap\n");
                AlphFix.CapTimeCount = 1;
                float average = 0;
                Console.WriteLine("Capture time : " + DateTime.Now.ToString("HHmmss"));
                if (AlphFix.CsvTimeCount > 1)
                {
                    int now = int.Parse(DateTime.Now.ToString().Substring(DateTime.Now.ToString().Length - 2));
                    int last = int.Parse(strResult.Split(',')[0].Substring(strResult.Split(',')[0].Length - 2));
                    int dul = now - last;
                    if (dul != 5 && dul != -55)
                    {
                        Console.WriteLine("Capture time error");
                    }
                }
                strResult = "";
                strResult += DateTime.Now.ToString() + ',';
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
                for (int i = 0; i < 24; i++)
                {
                    strResult += AllData[PPB][i] + ",";
                }
                for (int i = 0; i < 4; i++)
                {
                    strResult += label.Temp[i].Text.Replace("°C", "") + ',';
                    average += float.Parse(label.Temp[i].Text.Replace("°C", ""));
                }
                strResult += (average / 4).ToString() + ',';
                average = 0;
                for (int i = 0; i < 4; i++)
                {
                    strResult += label.RH[i].Text + ',';
                    average += float.Parse(label.RH[i].Text);
                }
                strResult += (average / 4).ToString() + ',';
                average = 0;
                for (int i = 0; i < 4; i++)
                {
                    strResult += label.AH[i].Text + ',';
                    average += float.Parse(label.AH[i].Text);
                }
                strResult += (average / 4).ToString() + '\n';
                average = 0;
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
        /*********************************************************************************************************************************************************
                                                                            other void
        **********************************************************************************************************************************************************/
        private void AlphFix_Port_box_DropDown(object sender, EventArgs e)
        {
            AlphFix_Port_box.Items.Clear();
            AlphFix_Port_box.Items.AddRange(SerialPort.GetPortNames());
        }

        string temp = "";
        private int InitRead()
        {
            if (serialPort2_DataSend("@R,INIT", read) != 1) 
            {        
                return 1;           
            }
            else
            {
                temp = AlphFix.RxString.Replace("\r", "");
                string[] temp_list = temp.Split(':');
                AllData[GasName] = temp_list[0].Split(',');
                AllData[BSN] = temp_list[1].Split(',');
                AllData[FW] = temp_list[2].Split(',');
                AllData[CalDate] = temp_list[3].Split(',');
                AllData[Zero] = temp_list[4].Split(',');
                AllData[Span] = temp_list[5].Split(',');
                AllData[A] = temp_list[6].Split(',');
                AllData[B] = temp_list[7].Split(',');
               
                for (int i = 0; i < 24; i++)
                {
                    if (AllData[GasName][i] != "0")
                    {
                        AllData[GasName][i] = IntToAscii(int.Parse(AllData[GasName][i]));
                    }
                    if (AllData[Span][i] != "0")
                    {
                        AllData[Span][i] = (float.Parse(AllData[Span][i]) / 1000).ToString();
                    }
                    if (AllData[A][i] != "0")
                    {
                        AllData[A][i] = (float.Parse(AllData[A][i]) / 1000).ToString();
                    }
                }

                

                return 0;
            }
        }

        private int AlwaysRead()
        {           
            if (serialPort2_DataSend("@R,ALL", read) != 1)
            {
                return 1;
            }
            else
            {
                temp = AlphFix.RxString.Replace("\r", "");
                string[] temp_list = temp.Split(':');
                AllData[WE] = temp_list[0].Split(',');
                AllData[AE] = temp_list[1].Split(',');
                AllData[PPB] = temp_list[2].Split(',');
                AllData[RHT] = temp_list[3].Split(',');

                for (int i = 0; i < 24; i++)
                {
                    AllData[ADC][i] = (int.Parse(AllData[WE][i]) - int.Parse(AllData[AE][i])).ToString();
                }

                return 0;
            }                  
        }

        private int AutoZero()
        {
            for (int i = 0; i < 24; i++)
            {
                if (AllData[GasName][i] != "0")
                {
                    if (serialPort2_DataSend("@W,ZERO," + i.ToString("D2") + "," + (Convert.ToInt32(AverageValue[i])).ToString(), write) != 1)
                    {
                        return 1;
                    }
                }
            }
            return 0;
        }

        private int AutoSpan()
        {
            float span_temp;
            for (int i = 0; i < 24; i++)
            {
                if (AllData[GasName][i] != "0")
                {
                    span_temp = (AverageValue[i] - float.Parse(AllData[Zero][i])) / (float)SpanValue_box.Value;
                    if (serialPort2_DataSend("@W,SPAN," + i.ToString("D2") + "," + span_temp.ToString(), write) != 1)
                    {
                        return 1;
                    }
                    if (serialPort2_DataSend("@W,CAL," + i.ToString("D2") + "," + DateTime.Now.ToString("yyMMdd"), write) != 1)
                    {
                        return 1;
                    }
                }
            }
            return 0;
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
                AH = 6.112 * Math.Pow(2.71828182845905, (17.62 * Temp / (243.12 + Temp))) * (RH / 100) * 100 / (461.52 * (273.15 + Temp));
                AH = Math.Round(AH * 1000, 2);
                label.AH[i].Text = AH.ToString();
            }

            for (int i = 0; i < 24; i++)
            {
                if (AllData[GasName][i] != "0")
                {
                    label.A[i].Text = AllData[A][i];
                    label.B[i].Text = AllData[B][i];
                    label.Cal_Date[i].Text = AllData[CalDate][i];
                    label.FW[i].Text = AllData[FW][i];
                    label.BSN[i].Text = AllData[BSN][i];
                    label.Gas_Name[i].Text = AllData[GasName][i].Replace("\0", ""); ;
                    label.Span[i].Text = AllData[Span][i];
                    label.Zero[i].Text = AllData[Zero][i];

                    label.A[i].BackColor = Color.HotPink;
                    label.B[i].BackColor = Color.HotPink;
                    label.Cal_Date[i].BackColor = Color.HotPink;
                    label.FW[i].BackColor = Color.HotPink;
                    label.BSN[i].BackColor = Color.HotPink;
                    label.Gas_Name[i].BackColor = Color.HotPink;
                    label.Span[i].BackColor = Color.HotPink;
                    label.Zero[i].BackColor = Color.HotPink;

                    group.WEAE[i].BackColor = SystemColors.Control;
                    group.ADC[i].BackColor = SystemColors.Control;
                    group.Zero[i].BackColor = SystemColors.Control;
                    group.Span[i].BackColor = SystemColors.Control;
                    group.A[i].BackColor = SystemColors.Control;
                    group.B[i].BackColor = SystemColors.Control;
                    group.GasName[i].BackColor = SystemColors.Control;
                    group.BSN[i].BackColor = SystemColors.Control;
                    group.FW[i].BackColor = SystemColors.Control;
                    group.CalDate[i].BackColor = SystemColors.Control;
                }
            }

            for (int i = 0; i < 24; i++)
            {
                if (AllData[GasName][i] != "0")
                {
                    if (label.WE[i].Text == AllData[WE][i] && label.AE[i].Text == AllData[AE][i])
                    {
                        label.WE[i].BackColor = Color.Crimson;
                        label.AE[i].BackColor = Color.Crimson;
                    }
                    else
                    {
                        label.WE[i].Text = AllData[WE][i];
                        label.AE[i].Text = AllData[AE][i];
                        label.WE[i].BackColor = Color.AntiqueWhite;
                        label.AE[i].BackColor = Color.HotPink;
                    }
                }
            }

            for (int i = 0; i < 24; i++)
            {
                if (AllData[GasName][i] != "0")
                {
                    if (label.PPB[i].Text == AllData[PPB][i] && label.ADC[i].Text == AllData[ADC][i])
                    {
                        label.PPB[i].BackColor = Color.Crimson;
                        label.ADC[i].BackColor = Color.Crimson;
                    }
                    else
                    {
                        label.PPB[i].Text = AllData[PPB][i];
                        label.ADC[i].Text = AllData[ADC][i];
                        label.PPB[i].BackColor = Color.AntiqueWhite;
                        label.ADC[i].BackColor = Color.HotPink;
                    }
                }
            }

        }

        private void AlphFix_lock()
        {
            btn_AlphFix_write.Enabled = true;
            btn_AlphFix_write.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_AlphFix_read.Enabled = true;
            btn_AlphFix_read.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_InitALL.Enabled = true;
            btn_InitALL.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_AutoSpan.Enabled = false;
            btn_AutoSpan.BackColor = SystemColors.ActiveBorder;
            btn_CalZero.Enabled = false;
            btn_CalZero.BackColor = SystemColors.ActiveBorder;
            
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
                label.AE[i].Text = "AE Value";
                label.PPB[i].Text = "PPB Value";
                label.ADC[i].Text = "ADC Value";
                label.Zero[i].Text = "Zero Value";
                label.Span[i].Text = "Span Value";
                label.A[i].Text = "A Value";
                label.B[i].Text = "B Value";
                label.Gas_Name[i].Text = "Gas Name";
                label.BSN[i].Text = "SerialNum";
                label.FW[i].Text = "FW Ver";              
                label.Cal_Date[i].Text = "Cal Date";

                label.WE[i].BackColor = SystemColors.ActiveBorder;
                label.AE[i].BackColor = SystemColors.ActiveBorder;
                label.PPB[i].BackColor = SystemColors.ActiveBorder;
                label.ADC[i].BackColor = SystemColors.ActiveBorder;
                label.Zero[i].BackColor = SystemColors.ActiveBorder;
                label.Span[i].BackColor = SystemColors.ActiveBorder;
                label.A[i].BackColor = SystemColors.ActiveBorder;
                label.B[i].BackColor = SystemColors.ActiveBorder;
                label.Gas_Name[i].BackColor = SystemColors.ActiveBorder;
                label.BSN[i].BackColor = SystemColors.ActiveBorder;
                label.FW[i].BackColor = SystemColors.ActiveBorder;
                label.Cal_Date[i].BackColor = SystemColors.ActiveBorder;

                group.WEAE[i].BackColor = SystemColors.ActiveBorder;
                group.ADC[i].BackColor = SystemColors.ActiveBorder;
                group.Zero[i].BackColor = SystemColors.ActiveBorder;
                group.Span[i].BackColor = SystemColors.ActiveBorder;
                group.A[i].BackColor = SystemColors.ActiveBorder;
                group.B[i].BackColor = SystemColors.ActiveBorder;
                group.GasName[i].BackColor = SystemColors.ActiveBorder;
                group.BSN[i].BackColor = SystemColors.ActiveBorder;
                group.FW[i].BackColor = SystemColors.ActiveBorder;
                group.CalDate[i].BackColor = SystemColors.ActiveBorder;
            }
        }

        private void AlphFix_unlock()
        {
            btn_AlphFix_write.Enabled = false;
            btn_AlphFix_write.BackColor = SystemColors.ActiveBorder;
            btn_AlphFix_read.Enabled = false;
            btn_AlphFix_read.BackColor = SystemColors.ActiveBorder;
            btn_InitALL.Enabled = false;
            btn_InitALL.BackColor = SystemColors.ActiveBorder;
            btn_AutoSpan.Enabled = true;
            btn_AutoSpan.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_CalZero.Enabled = true;
            btn_CalZero.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_AlphFix_Log.Enabled = true;
            btn_AlphFix_Log.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_AlphFix_cap.Enabled = true;
            btn_AlphFix_cap.BackColor = Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
        }

        string IntToAscii(int num)
        {
            byte b1 = (byte)((num >> 16) & 0xFF);
            byte b2 = (byte)((num >> 8) & 0xFF);
            byte b3 = (byte)(num & 0xFF);
            return $"{(char)b1}{(char)b2}{(char)b3}";
        }

        private void AlphFix_Info_Box_TextChanged(object sender, EventArgs e)
        {
            AlphFix_Info_box.SelectionStart = this.AlphFix_Info_box.TextLength;
            AlphFix_Info_box.ScrollToCaret();
        }
        private void btn_test_Click(object sender, EventArgs e)
        {
            if (serialPort2_DataSend(test_box.Text, 1) != 1) { return; }
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

            label.PPB = new List<Label> { 
                label_ADC_Value0, label_ADC_Value1, label_ADC_Value2, label_ADC_Value3, label_ADC_Value4, label_ADC_Value5, label_ADC_Value6, label_ADC_Value7, label_ADC_Value8, label_ADC_Value9,
                label_ADC_Value10, label_ADC_Value11, label_ADC_Value12, label_ADC_Value13, label_ADC_Value14, label_ADC_Value15, label_ADC_Value16, label_ADC_Value17, label_ADC_Value18, label_ADC_Value19,
                label_ADC_Value20, label_ADC_Value21, label_ADC_Value22, label_ADC_Value23
            };

            label.ADC = new List<Label> { 
                label_ADC0, label_ADC1, label_ADC2, label_ADC3, label_ADC4, label_ADC5,label_ADC6,label_ADC7,label_ADC8,label_ADC9,
                label_ADC10, label_ADC11, label_ADC12, label_ADC13, label_ADC14, label_ADC15,label_ADC16,label_ADC17,label_ADC18,label_ADC19,
                label_ADC20, label_ADC21, label_ADC22, label_ADC23
            };

            label.Zero = new List<Label> {
                label_Zero0, label_Zero1, label_Zero2, label_Zero3, label_Zero4, label_Zero5, label_Zero6, label_Zero7, label_Zero8, label_Zero9,
                label_Zero10, label_Zero11, label_Zero12, label_Zero13, label_Zero14, label_Zero15, label_Zero16, label_Zero17, label_Zero18, label_Zero19,
                label_Zero20, label_Zero21, label_Zero22, label_Zero23
            };

            label.Span = new List<Label> { 
                label_Span0, label_Span1, label_Span2, label_Span3, label_Span4, label_Span5, label_Span6, label_Span7, label_Span8, label_Span9,
                label_Span10, label_Span11, label_Span12, label_Span13, label_Span14, label_Span15, label_Span16, label_Span17, label_Span18, label_Span19,
                label_Span20, label_Span21, label_Span22, label_Span23
            };

            label.A = new List<Label> {
                label_A0, label_A1, label_A2, label_A3, label_A4, label_A5, label_A6, label_A7, label_A8, label_A9,
                label_A10, label_A11, label_A12, label_A13, label_A14, label_A15, label_A16, label_A17, label_A18, label_A19,
                label_A20, label_A21, label_A22, label_A23
            };

            label.B = new List<Label> {
                label_B0, label_B1, label_B2, label_B3, label_B4, label_B5, label_B6, label_B7, label_B8, label_B9,
                label_B10, label_B11, label_B12, label_B13, label_B14, label_B15, label_B16, label_B17, label_B18, label_B19,
                label_B20, label_B21, label_B22, label_B23
            };

            label.Gas_Name = new List<Label> {
                label_Sensor_Name0, label_Sensor_Name1,  label_Sensor_Name2, label_Sensor_Name3, label_Sensor_Name4, label_Sensor_Name5, label_Sensor_Name6, label_Sensor_Name7, label_Sensor_Name8, label_Sensor_Name9,
                label_Sensor_Name10, label_Sensor_Name11,  label_Sensor_Name12, label_Sensor_Name13, label_Sensor_Name14, label_Sensor_Name15, label_Sensor_Name16, label_Sensor_Name17, label_Sensor_Name18, label_Sensor_Name19,
                label_Sensor_Name20, label_Sensor_Name21,  label_Sensor_Name22, label_Sensor_Name23
            };

            label.BSN = new List<Label> { 
                label_BSN0, label_BSN1, label_BSN2,  label_BSN3, label_BSN4, label_BSN5, label_BSN6, label_BSN7, label_BSN8, label_BSN9,
                label_BSN10, label_BSN11, label_BSN12,  label_BSN13, label_BSN14, label_BSN15, label_BSN16, label_BSN17, label_BSN18, label_BSN19,
                label_BSN20, label_BSN21, label_BSN22,  label_BSN23
            };
            
            label.FW = new List<Label> { 
                label_FW_0, label_FW_1, label_FW_2, label_FW_3, label_FW_4, label_FW_5, label_FW_6, label_FW_7, label_FW_8, label_FW_9,
                label_FW_10, label_FW_11, label_FW_12, label_FW_13, label_FW_14, label_FW_15, label_FW_16, label_FW_17, label_FW_18, label_FW_19,
                label_FW_20, label_FW_21, label_FW_22, label_FW_23
            };

            label.Cal_Date = new List<Label> {
                label_CalDate0, label_CalDate1, label_CalDate2, label_CalDate3, label_CalDate4, label_CalDate5, label_CalDate6, label_CalDate7, label_CalDate8, label_CalDate9,
                label_CalDate10, label_CalDate11, label_CalDate12, label_CalDate13, label_CalDate14, label_CalDate15, label_CalDate16, label_CalDate17, label_CalDate18, label_CalDate19,
                label_CalDate20, label_CalDate21, label_CalDate22, label_CalDate23
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

            group.WEAE = new List<GroupBox> {
                group_PCBA0, group_PCBA1, group_PCBA2, group_PCBA3, group_PCBA4, group_PCBA5, group_PCBA6, group_PCBA7, group_PCBA8, group_PCBA9,
                group_PCBA10, group_PCBA11, group_PCBA12, group_PCBA13, group_PCBA14, group_PCBA15, group_PCBA16, group_PCBA17, group_PCBA18, group_PCBA19,
                group_PCBA20, group_PCBA21, group_PCBA22, group_PCBA23
            };

            group.ADC = new List<GroupBox> {
                group_ADC0, group_ADC1, group_ADC2, group_ADC3, group_ADC4, group_ADC5, group_ADC6, group_ADC7, group_ADC8, group_ADC9,
                group_ADC10, group_ADC11, group_ADC12, group_ADC13, group_ADC14, group_ADC15, group_ADC16, group_ADC17, group_ADC18, group_ADC19,
                group_ADC20, group_ADC21, group_ADC22, group_ADC23
            };

            group.Zero = new List<GroupBox> {
                group_zero0, group_zero1, group_zero2, group_zero3, group_zero4, group_zero5, group_zero6, group_zero7, group_zero8, group_zero9,
                group_zero10, group_zero11, group_zero12, group_zero13, group_zero14, group_zero15, group_zero16, group_zero17, group_zero18, group_zero19,
                group_zero20, group_zero21, group_zero22, group_zero23
            };

            group.Span = new List<GroupBox> {
                group_Span0, group_Span1, group_Span2, group_Span3, group_Span4, group_Span5, group_Span6, group_Span7, group_Span8, group_Span9,
                group_Span10, group_Span11, group_Span12, group_Span13, group_Span14, group_Span15, group_Span16, group_Span17, group_Span18, group_Span19,
                group_Span20, group_Span21, group_Span22, group_Span23
            };

            group.A = new List<GroupBox> {
                group_A0, group_A1, group_A2, group_A3, group_A4, group_A5, group_A6, group_A7, group_A8, group_A9,
                group_A10, group_A11, group_A12, group_A13, group_A14, group_A15, group_A16, group_A17, group_A18, group_A19,
                group_A20, group_A21, group_A22, group_A23
            };

            group.B = new List<GroupBox> {
                group_B0, group_B1, group_B2, group_B3, group_B4, group_B5, group_B6, group_B7, group_B8, group_B9,
                group_B10, group_B11, group_B12, group_B13, group_B14, group_B15, group_B16, group_B17, group_B18, group_B19,
                group_B20, group_B21, group_B22, group_B23
            };

            group.GasName = new List<GroupBox> {
                groupBox_Sensor_Name0, groupBox_Sensor_Name1, groupBox_Sensor_Name2, groupBox_Sensor_Name3, groupBox_Sensor_Name4, groupBox_Sensor_Name5, groupBox_Sensor_Name6, groupBox_Sensor_Name7, groupBox_Sensor_Name8, groupBox_Sensor_Name9,
                groupBox_Sensor_Name10, groupBox_Sensor_Name11, groupBox_Sensor_Name12, groupBox_Sensor_Name13, groupBox_Sensor_Name14, groupBox_Sensor_Name15, groupBox_Sensor_Name16, groupBox_Sensor_Name17, groupBox_Sensor_Name18, groupBox_Sensor_Name19,
                groupBox_Sensor_Name20, groupBox_Sensor_Name21, groupBox_Sensor_Name22, groupBox_Sensor_Name23
            };

            group.BSN = new List<GroupBox> {
                group_BSN0, group_BSN1, group_BSN2, group_BSN3, group_BSN4, group_BSN5, group_BSN6, group_BSN7, group_BSN8, group_BSN9,
                group_BSN10, group_BSN11, group_BSN12, group_BSN13, group_BSN14, group_BSN15, group_BSN16, group_BSN17, group_BSN18, group_BSN19,
                group_BSN20, group_BSN21, group_BSN22, group_BSN23
            };

            group.FW = new List<GroupBox> {
                group_FW0, group_FW1, group_FW2, group_FW3, group_FW4, group_FW5, group_FW6, group_FW7, group_FW8, group_FW9,
                group_FW10, group_FW11, group_FW12, group_FW13, group_FW14, group_FW15, group_FW16, group_FW17, group_FW18, group_FW19,
                group_FW20, group_FW21, group_FW22, group_FW23
            };

            group.CalDate = new List<GroupBox> {
                group_CalDate0, group_CalDate1, group_CalDate2, group_CalDate3, group_CalDate4, group_CalDate5, group_CalDate6, group_CalDate7, group_CalDate8, group_CalDate9,
                group_CalDate10, group_CalDate11, group_CalDate12, group_CalDate13, group_CalDate14, group_CalDate15, group_CalDate16, group_CalDate17, group_CalDate18, group_CalDate19,
                group_CalDate20, group_CalDate21, group_CalDate22, group_CalDate23
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
            ListViewItem item = new ListViewItem(new string[] { "@MS," + Addr_Box.Text + "," + Sequence_Box.Text + "," + Point_Box.Text, Sabio_Time_Box.Text + ":" + "00" });
            listView1.Items.Add(item);
        }

        private void btn_Zero_MS_Click(object sender, EventArgs e)
        {
            ListViewItem item = new ListViewItem(new string[] { "@MS," + Addr_Box.Text + "," + Sequence_Box.Text + "," + Point_Box.Text, Sabio_Time_Box.Text + ":" + "00" });
            listView1.Items.Add(item);
            item = new ListViewItem(new string[] { "#AutoZero", "--" });
            listView1.Items.Add(item);
        }

        private void btn_Span_MS_Click(object sender, EventArgs e)
        {
            ListViewItem item = new ListViewItem(new string[] { "@MS," + Addr_Box.Text + "," + Sequence_Box.Text + "," + Point_Box.Text, Sabio_Time_Box.Text + ":" + "00" });
            listView1.Items.Add(item);
            item = new ListViewItem(new string[] { "#AutoSpan", "--" });
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
                    Sabio.pass_time = 1;
                    Sabio.CapTimeCount = 1;
                    btn_Sabio_cap.Text = "Stop";
                    Sabio_UpdateRate_Box.Enabled = false;
                    Sabio.CsvTimeCount = 0;
                    Sabio.FileName = FileName_Box.Text;
                    string strResult = "Computer Time,Time,Source,Diluted Gases,Diluent MFC (Control),Diluent MFC (Monitor),Source MFC (Control),Source MFC (Monitor)," +
                        "Total Flow(Control),Total Flow(Monitor),1st Gases,1st Diluted Gases,2st Gases,2st Diluted Gases,3st Gases,3st Diluted Gases\n";
                    using (StreamWriter strwrite = new StreamWriter("D:\\" + Sabio.FileName + ".csv", false, Encoding.Default))
                    {
                        strwrite.Write(strResult);
                    }
                    timer_getStatus.Start();
                }
            }
            else
            {
                Sabio.pass_time = 0;
                label_Sabio_time.Text = "00 : 00 : 00";
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

            //Console.WriteLine("DataReceived triggered!");
            if (btn_Sabio_Connect.Text == "Disconnect")
            {

                Sabio.RxString += serialPort1.ReadExisting();

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
                    if (Sabio.O3Flag)
                    {
                        if (Sabio.RxString.Count(c => c == '\r') == 6)
                        {
                            ResponseReceived.Set();
                        }
                    }
                    else
                    {
                        if (Sabio.RxString.Count(c => c == '\r') == 3)
                        {
                            ResponseReceived.Set();
                        }
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
                        Console.WriteLine(Sabio.RxInfo);
                        Sabio_Info_Box.AppendText(Sabio.RxInfo);
                        return;  // 收到 ACK，結束函式
                    }
                }
                else if (Sabio.CommandMode == 0 || Sabio.CommandMode == 1 || Sabio.CommandMode == 2)//GetStatus
                {
                    if (ResponseReceived.WaitOne(1000))//讀資料比較快
                    {
                        Sabio.CmdStatus = 1;
                        Console.WriteLine(Sabio.RxInfo);
                        Sabio_Info_Box.AppendText(Sabio.RxInfo);
                        return;  // 收到 \r，結束函式
                    }
                }
                send_count++;
            }

            Sabio.CmdStatus = -1; // 4010沒有回應
            timer_command.Stop();
            timer_getStatus.Stop();
            serialPort1.WriteLine("@S,001" + '\r' + '\n');
            MessageBox.Show("Sabio 4010沒有回應");
            Sabio.pass_time = 0;
            label_Sabio_time.Text = "00 : 00 : 00";
            Sabio_unlock_btn();
            btn_Sabio_cap.Text = "Capture";
            Sabio_UpdateRate_Box.Enabled = true;
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
                string[] timelist = item.SubItems[1].Text.Split(':');
                int time;
                if (item.SubItems[1].Text == "--")
                {
                    time = 0;
                }
                else
                {
                    time = int.Parse(timelist[0]) * 60 + int.Parse(timelist[1]);
                }
                Sabio.CmdTimeCount = time;

                if (command.Contains("+O3"))
                {
                    Sabio.O3Flag = true;
                }
                else
                {
                    Sabio.O3Flag = false;
                }

                timer_command.Stop();
                if(command == "#AutoZero")
                {

                    label_Response.Text = "Wait";
                    if (AutoZero() == 1)
                    {
                        label_Response.Text = "AutoZero Fail!";
                        return;
                    }
                    label_Response.Text = "AutoZero Success!";
                    InitRead();
                }
                else if (command == "#AutoSpan")
                {
                    label_Response.Text = "Wait";
                    if (AutoSpan() == 1)
                    {
                        label_Response.Text = "AutoSpan Fail!";
                        return;
                    }
                    label_Response.Text = "AutoSpan Success!";
                    InitRead();
                }
                else
                {
                    Sabio.CommandMode = 3;
                    serialPort1_DataSend(command);
                    if (Sabio.CmdStatus != 1)
                    {

                        return;
                    }
                }
                timer_command.Start();
                //timer_getStatus.Start();
                Sabio.CmdIndex++;
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
            timer_getStatus.Interval = 1000;
            
            //timer_getStatus.Interval = int.Parse(Sabio_UpdateRate_Box.Text) * 1000;
            TimeSpan timeSpan = TimeSpan.FromSeconds(Sabio.pass_time);

            if (Sabio.CapTimeCount == int.Parse(Sabio_UpdateRate_Box.Text))
            {
                Sabio.CapTimeCount = 1;
                get_Status();

                string strResult = string.Empty;
                strResult += DateTime.Now.ToString() + ',';
                strResult += Sabio.CsvTimeCount.ToString() + ',';
                strResult += label_Status1.Text + ',';
                strResult += label_Status2.Text + ',';
                strResult += label_Measure1.Text + ',';
                strResult += label_Measure2.Text + ',';
                strResult += label_Measure3.Text + ',';
                strResult += label_Measure4.Text + ',';
                strResult += label_Measure5.Text + ',';
                strResult += label_Measure6.Text + ',';
                strResult += label_Status3.Text + ',';
                strResult += label_Status4.Text + ',';
                strResult += label_Status5.Text + ',';
                strResult += label_Status6.Text + ',';
                strResult += label_Status7.Text + ',';
                strResult += label_Status8.Text + '\n';
                using (StreamWriter strwrite = new StreamWriter("D:\\" + Sabio.FileName + ".csv", true, Encoding.Default))
                {
                    strwrite.Write(strResult);
                }

                Sabio.CsvTimeCount += float.Parse(Sabio_UpdateRate_Box.Text) / 60;
            }
            else
            {
                Sabio.CapTimeCount++;
            }

            Sabio.pass_time++;
            label_Sabio_time.Text = $"{timeSpan.Hours:D2} : {timeSpan.Minutes:D2} : {timeSpan.Seconds:D2}";
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
            gas_list[0] = (float.Parse(gas_list[0]) / 1000).ToString();
            if (gas_list.Length == 3)
            {
                label_Status1.Text = "Nothing";
                label_Status2.Text = "0.0";
                label_Status3.Text = "--";
                label_Status4.Text = "0.0";
                label_Status5.Text = "--";
                label_Status6.Text = "0.0";
                label_Status7.Text = "--";
                label_Status8.Text = "0.0";
                label_Measure6.Text = gas_list[0];
            }
            else if (gas_list.Length == 5)
            {
                label_Status1.Text = gas_list[2];
                label_Status2.Text = (float.Parse(gas_list[3]) / 1000).ToString();
                label_Status3.Text = "--";
                label_Status4.Text = "0.0";
                label_Status5.Text = "--";
                label_Status6.Text = "0.0";
                label_Status7.Text = "--";
                label_Status8.Text = "0.0";
                label_Measure6.Text = gas_list[0];
            }
            else if (gas_list.Length == 11)
            {
                label_Status1.Text = gas_list[2];
                label_Status2.Text = (float.Parse(gas_list[3]) / 1000).ToString();
                label_Status3.Text = gas_list[4];
                label_Status4.Text = (float.Parse(gas_list[5]) / 1000).ToString();
                label_Status5.Text = gas_list[6];
                label_Status6.Text = (float.Parse(gas_list[7]) / 1000).ToString();
                label_Status7.Text = gas_list[8];
                label_Status8.Text = (float.Parse(gas_list[9]) / 1000).ToString();
                label_Measure6.Text = gas_list[0];
            }
            else
            {
                label_Status1.Text = "Miss";
                label_Status2.Text = "Miss";
                label_Status3.Text = "Miss";
                label_Status4.Text = "Miss";
                label_Status5.Text = "Miss";
                label_Status6.Text = "Miss";
                label_Measure6.Text = "Miss";
            }

            if (dilution_list.Length == 11)
            {
                label_Measure1.Text = dilution_list[0];
                label_Measure2.Text = dilution_list[1];
                label_Measure3.Text = dilution_list[4];
                label_Measure4.Text = dilution_list[5];
                label_Measure5.Text = (float.Parse(dilution_list[0]) + float.Parse(dilution_list[4]) / 1000).ToString();
            }
            else
            {
                label_Measure1.Text = "Miss";
                label_Measure2.Text = "Miss";
                label_Measure3.Text = "Miss";
                label_Measure4.Text = "Miss";
                label_Measure5.Text = "Miss";
            }

            LEDFlash();
        }

        private void Sabio_lock_btn()
        {
            btn_CmdStart.Enabled = false;
            btn_CmdStart.BackColor = System.Drawing.SystemColors.ActiveBorder;
            btn_MS.Enabled = false;
            btn_MS.BackColor = System.Drawing.SystemColors.ActiveBorder;
            btn_Zero_MS.Enabled = false;
            btn_Zero_MS.BackColor = System.Drawing.SystemColors.ActiveBorder;
            btn_Span_MS.Enabled = false;
            btn_Span_MS.BackColor = System.Drawing.SystemColors.ActiveBorder;
            btn_ClnList.Enabled = false;
            btn_ClnList.BackColor = System.Drawing.SystemColors.ActiveBorder;
        }

        private void Sabio_unlock_btn()
        {
            btn_CmdStart.Enabled = true;
            btn_CmdStart.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_MS.Enabled = true;
            btn_MS.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_Zero_MS.Enabled = true;
            btn_Zero_MS.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            btn_Span_MS.Enabled = true;
            btn_Span_MS.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
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
