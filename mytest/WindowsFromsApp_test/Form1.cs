using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Reflection;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
using System.Xml.Linq;
using System.Threading;

namespace MyTest
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            comboBox1.Items.Clear();
            comboBox1.Items.AddRange(SerialPort.GetPortNames());
            btn_Connect.Enabled = true;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            serialPort1.Close();
            serialPort1.Dispose();
        }

        /*********************************************************************
                                         button
        *********************************************************************/
        private void btn_Connect_Click(object sender, EventArgs e)
        {
            if (btn_Connect.Text == "Connect") {
                try
                {
                    serialPort1.PortName = comboBox1.Text;
                    serialPort1.BaudRate = int.Parse(comboBox2.Text);

                    if (serialPort1.IsOpen)
                    {
                        label_State.Text = serialPort1.PortName + " port isent openned.";
                        btn_Connect.Text = "Disconnect";
                    }
                    else
                    {
                        serialPort1.Open();
                        if (serialPort1.IsOpen)
                        {
                            label_State.Text = serialPort1.PortName + " connect success.";
                            btn_Connect.Text = "Disconnect";
                        }
                        else
                        {
                            label_State.Text = serialPort1.PortName + " Connect unsuccess.";
                        }
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }

            else {
                serialPort1.Close();
                if (serialPort1.IsOpen)
                {
                    label_State.Text = serialPort1.PortName + " disconnect unsuccess.";
                }
                else
                {
                    label_State.Text = "Coose port to connect.";
                    btn_Connect.Text = "Connect";
                }
            }
        }
        private void btn_MS_Click(object sender, EventArgs e)
        {
            ListViewItem item = new ListViewItem(new string[] { "@MS," + Addr_Box.Text + "," + Sequence_Box.Text + "," + Point_Box.Text, Time_Box.Text });
            listView1.Items.Add(item);
        }


        private void btn_TS_Click(object sender, EventArgs e)
        {
            ListViewItem item = new ListViewItem(new string[] { "@TS," + Addr_Box.Text + "," + Sequence_Box.Text + "," + Point_Box.Text, Time_Box.Text });
            listView1.Items.Add(item);
        }

        private void btn_CmdStart_Click(object sender, EventArgs e)
        {
            Variable.CmdStatus = 0;
            Variable.CmdTimeCount = 0;
            Variable.CmdIndex = 0;
            Variable.RxString = "";
            timer_command.Start();
            

            btn_CmdStart.Enabled = false;
            btn_MS.Enabled = false;
            btn_TS.Enabled = false;
            btn_ClnList.Enabled = false;
        }

        private void btn_CmdStop_Click(object sender, EventArgs e)
        {
            StopCmdList();
        }

        private void btn_ClnList_Click(object sender, EventArgs e)
        {
            listView1.Items.Clear();
        }

        /*********************************************************************
                                   receive & write
        *********************************************************************/
        private ManualResetEvent ackReceived = new ManualResetEvent(false);
        private ManualResetEvent EndLineReceived = new ManualResetEvent(false);
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            if (btn_Connect.Text == "Disconnect")
            {
                Variable.RxString += serialPort1.ReadExisting();
                Variable.RxInfo = Variable.RxString.Replace("\u0006", "[ACK]\n");
                Variable.RxInfo = Variable.RxInfo.Replace("\u000D", "[CR]\n");

                if (Variable.RxString == "\u0006")
                {
                    ackReceived.Set();
                }
                if (Variable.RxString.EndsWith("\r"))
                {
                    EndLineReceived.Set();
                }

                Invoke(new EventHandler(display_info));
            }
            else { }
        }

        
        private void serialPort1_DataSend(string command, string fun)
        {
            string Rxtest = "";
            Variable.CmdStatus = 0;
            int send_count = 0;
            int scan_count = 0;

            while (send_count < 5)
            {
                if (serialPort1.IsOpen)
                {
                    ackReceived.Reset();
                    EndLineReceived.Reset();
                    Info_Box.AppendText(command + '\n');
                    Variable.RxString = "";
                    serialPort1.WriteLine(command + '\r' + '\n');
                    Thread.Sleep(100);
                    Rxtest = serialPort1.ReadExisting();
                }
                else
                {
                    Variable.CmdIndex = 0;
                    Variable.CmdStatus = 0;
                    Variable.CmdTimeCount = 0;

                    btn_CmdStart.Enabled = true;
                    btn_MS.Enabled = true;
                    btn_TS.Enabled = true;
                    btn_ClnList.Enabled = true;
                    timer_command.Stop();
                    MessageBox.Show("Sabio 4010未連線");
                    return;
                }

                if(fun == "set point or stop")
                {
                    Rxtest = serialPort1.ReadExisting();
                    /*if (ackReceived.WaitOne(5000))  // 等待 1 秒
                    {
                        Variable.CmdStatus = 1;
                        break;  // 收到 ACK，跳出迴圈
                    }*/

                }
                else if(fun == "get status")
                {
                    /*if (EndLineReceived.WaitOne(30000))  // 等待 1 秒
                    {
                        Variable.CmdStatus = 1;
                        break;  // 收到 ACK，跳出迴圈
                    }*/
                    while (scan_count < 5)
                    {
                        Thread.Sleep(100); // 每 100ms 檢查一次
                        Rxtest = serialPort1.ReadExisting();
                        if (!Rxtest.EndsWith("\r"))  // 等待 1 秒
                        {
                            Variable.CmdStatus = 1;
                            break;  // 收到 ACK，跳出迴圈
                        }
                        scan_count++;
                    }
                    scan_count = 0;
                    
                }
                send_count++;
            }

            if (send_count == 5)
            {
                Variable.CmdStatus = -1; // 4010沒有回應
                timer_command.Stop();
                timer_getStatus_D.Stop();
                timer_getStatus_G.Stop();
                MessageBox.Show("Sabio 4010沒有回應");
            }

        }
              
        
        /*********************************************************************
                                         clock
        *********************************************************************/
        private void timer_command_Tick(object sender, EventArgs e)
        {
            timer_command.Interval = 1000;
            if(Variable.CmdTimeCount == 0 && Variable.CmdIndex < listView1.Items.Count)
            {
                ListViewItem item = listView1.Items[Variable.CmdIndex]; // 取得第一個項目
                string command = item.Text; // 第一列
                int time = int.Parse(item.SubItems[1].Text);
                Variable.CmdTimeCount = time;

                while (Variable.CmdStatus == -2)
                {
                    Thread.Sleep(100); // 每 100ms 檢查一次
                }
                timer_command.Enabled = false;
                Variable.CmdStatus = -2;
                serialPort1_DataSend(command, "set point or stop");
                timer_command.Enabled = true;

                timer_getStatus_D.Start();
                timer_getStatus_G.Start();
                if (Variable.CmdStatus == -1)
                {
                    return;
                }
                Variable.CmdIndex++;
                
            }
            else if(Variable.CmdTimeCount == 0 && Variable.CmdIndex >= listView1.Items.Count)
            {
                StopCmdList();
            }
            else if(Variable.CmdStatus == 1)
            {
                Variable.CmdTimeCount--;
                listView1.Items[Variable.CmdIndex-1].SubItems[1].Text = Variable.CmdTimeCount.ToString();
            }
            else
            {
                return;
            }
        }

        private void timer_getStatus_D_Tick(object sender, EventArgs e)
        {
            timer_getStatus_D.Interval = 5000;
            while (Variable.CmdStatus == -2)
            {
                Thread.Sleep(100); // 每 100ms 檢查一次
            }
            Variable.CmdStatus = -2;
            serialPort1_DataSend("@GS,001,D", "get status");
            String dilutiondata = Variable.RxString;
            label_Status1.Text = dilutiondata;
        }

        private void timer_getStatus_G_Tick(object sender, EventArgs e)
        {
            Thread.Sleep(100);
            timer_getStatus_G.Interval = 5000;
            while (Variable.CmdStatus == -2)
            {
                Thread.Sleep(100); // 每 100ms 檢查一次
            }
            Variable.CmdStatus = -2;
            serialPort1_DataSend("@GS,001,G", "get status");
            String gas = Variable.RxString;
            label_Status2.Text = gas;
        }

        /*********************************************************************
                                         other
        *********************************************************************/
        private void display_info(object s, EventArgs e) {
            Info_Box.AppendText(Variable.RxInfo);
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
            Variable.CmdIndex = 0;
            Variable.CmdStatus = 0;
            Variable.CmdTimeCount = 0;

            btn_CmdStart.Enabled = true;
            btn_MS.Enabled = true;
            btn_TS.Enabled = true;
            btn_ClnList.Enabled = true;
            serialPort1_DataSend("@S,001", "set point or stop");
            timer_command.Stop();
            timer_getStatus_D.Stop();
            timer_getStatus_G.Stop();
        }

        private void btn_test_Click(object sender, EventArgs e)
        {
            Variable.StatusBuf = "";
            serialPort1_DataSend(command_box.Text, "get status");
            //serialPort1.WriteLine(command_box.Text+'\r'+'\n');
            
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void comboBox1_DropDown(object sender, EventArgs e)
        {
            comboBox1.Items.Clear();
            comboBox1.Items.AddRange(SerialPort.GetPortNames());
        }

        private void Info_Box_TextChanged(object sender, EventArgs e)
        {
            this.Info_Box.SelectionStart = this.Info_Box.TextLength;
            this.Info_Box.ScrollToCaret();
        }

        
    }
}
