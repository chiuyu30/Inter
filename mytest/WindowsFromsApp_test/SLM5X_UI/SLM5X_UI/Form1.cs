using SLM5X_UI.Properties;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SLM5X_UI
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            Port1_box.Items.AddRange(SerialPort.GetPortNames());
            Port2_box.Items.AddRange(SerialPort.GetPortNames());
            Port3_box.Items.AddRange(SerialPort.GetPortNames());
            Port4_box.Items.AddRange(SerialPort.GetPortNames());
            Port5_box.Items.AddRange(SerialPort.GetPortNames());
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///                                                Button                                               ///
        //////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool capflag = false;
        private void btn_Connect_Click(object sender, EventArgs e)
        {
            if (btn_Connect.Text == "Connect")
            {
                PortsInit();
                try
                {
                    for(int i = 0; i < 5; i++)
                    {
                        if (ports[i] != null)
                        {
                            ports[i].Open();
                        }
                    }

                    label_State1.Text = "Connection successful.";
                    btn_Connect.Text = "Disconnect";
                    timer1.Start();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }

            else
            {
                try
                {
                    for (int i = 0; i < 5; i++)
                    {
                        if (ports[i] != null)
                        {
                            ports[i].Close();
                        }
                    }

                    label_State1.Text = "Choose port to connect.";
                    btn_Connect.Text = "Connect";
                    timer1.Stop();
                    int index = 0;
                    for (int i = 0; i < 60; i++)
                    {
                        listView1.Items[index].SubItems[1].Text = "--";
                        listView2.Items[index].SubItems[1].Text = "--";
                        listView3.Items[index].SubItems[1].Text = "--";
                        listView4.Items[index].SubItems[1].Text = "--";
                        listView5.Items[index].SubItems[1].Text = "--";
                        index++;
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        private void btn_cap_Click(object sender, EventArgs e)
        {
            if (btn_cap.Text == "Capture")
            {
                capflag = true;
                pass_time = 0;
                timer2.Start();
                btn_cap.Text = "Stop";
                for (int i = 0; i < 5; i++)
                {
                    string strResult = "";
                    if (ports[i] != null)
                    {
                        strResult += "Computer_Time,";
                        strResult += "epoch(utc time)_h,epoch(utc time)_l,voltage,current,PM10 CF,PM2.5 CF,PM1 CF,PM1 Air,PM10 Air,PM2.5 Air,0.3u count,0.5u count,";
                        strResult += "1.0u count,2.5u count,5.0u count,10u count,Ver+ Error,RH,Temperature,TVOC,eCO2,R1,R2,R3,R4,Differential Pressure,DP Temperature,";
                        strResult += "RH,Temperature,Status,Base line_h1,Base line_h2,Base line_l1,Base line_l2,PM4 CF,4.0u count,Typical_particle_size,RH,Temperature,Wind Speed(m/s),Wind Speed(m/s),Wind  angle,";
                        strResult += "Rainfall,UV,Illuminance_h,Illuminance_l,Atmospheric pressure_h,Atmospheric pressure_l,Wind Speed(m/s),Wind  Speed(m/s),Wind  angle,fan type_h1,fan type_h2,fan type_l1,fan type_l2,";
                        strResult += " ,utc time_h,utc time_l,A_db,Z_db\n";
                        using (StreamWriter strwrite = new StreamWriter("D:\\" + "STD5X_" + (i + 1) + "_test" + ".csv", false, Encoding.Default))
                        {
                            strwrite.Write(strResult);
                        }
                    }
                }

            }
            else
            {
                capflag = false;
                timer2.Stop();
                btn_cap.Text = "Capture";
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///                                                Timer                                               ///
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        int pass_time = 0;
        private void timer1_Tick(object sender, EventArgs e)
        {
            timer1.Interval = 5000;
            
            for (int i = 0; i < 5; i++)
            {
                if (ports[i] != null && ports[i].IsOpen)
                {
                    byte[] request = BuildReadInputRegisterCommand(234, 0, 60);
                    ports[i].Write(request, 0, request.Length);
                    byte[] buffer = new byte[125];
                    int bytesRead = ports[i].Read(buffer, 0, buffer.Length);
                    byte[] data = buffer.Take(bytesRead).ToArray();

                    int index = 0;
                    for (int j = 3; j + 1 < (data.Length - 1); j += 2)
                    {
                        ushort value = (ushort)((data[j] << 8) | data[j + 1]);
                        listViews[i].Items[index].SubItems[1].Text = value.ToString();
                        index++;
                    }
                    index = 0;
                    if (capflag == true)
                    {
                        string strResult = "";
                        strResult += DateTime.Now.ToString() + ',';
                        for (int j = 3; j + 1 < (data.Length - 1); j += 2)
                        {
                            ushort value = (ushort)((data[j] << 8) | data[j + 1]);
                            strResult += value.ToString() + ",";
                            index++;
                        }
                        strResult += '\n';
                        using (StreamWriter strwrite = new StreamWriter("D:\\" + "STD5X_" + (i + 1) + "_test" + ".csv", true, Encoding.Default))
                        {
                            strwrite.Write(strResult);
                        }
                        
                        
                    }
                }
            }
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            timer2.Interval = 1000;
            TimeSpan timeSpan = TimeSpan.FromSeconds(pass_time);
            pass_time++;
            label_pass_time.Text = $"{timeSpan.Hours:D2} : {timeSpan.Minutes:D2} : {timeSpan.Seconds:D2}";
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///                                           Modbus Command                                           ///
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        public byte[] BuildReadInputRegisterCommand(byte slaveId, ushort startAddress, ushort numRegisters)
        {
            byte[] frame = new byte[8];
            frame[0] = slaveId;
            frame[1] = 0x04; // Function code: Read Holding Registers
            frame[2] = (byte)(startAddress >> 8); // Start Address Hi
            frame[3] = (byte)(startAddress & 0xFF); // Start Address Lo
            frame[4] = (byte)(numRegisters >> 8); // Quantity Hi
            frame[5] = (byte)(numRegisters & 0xFF); // Quantity Lo

            ushort crc = ComputeCRC(frame, 6);
            frame[6] = (byte)(crc & 0xFF);     // CRC Lo
            frame[7] = (byte)(crc >> 8);       // CRC Hi

            return frame;
        }

        public ushort ComputeCRC(byte[] data, int length)
        {
            ushort crc = 0xFFFF;
            for (int pos = 0; pos < length; pos++)
            {
                crc ^= data[pos];
                for (int i = 0; i < 8; i++)
                {
                    bool lsb = (crc & 0x0001) != 0;
                    crc >>= 1;
                    if (lsb) crc ^= 0xA001;
                }
            }
            return crc;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///                                             others                                                 ///
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        private void Port1_box_DropDown(object sender, EventArgs e)
        {
            Port1_box.Items.Clear();
            Port1_box.Items.AddRange(SerialPort.GetPortNames());
        }
        private void Port2_box_DropDown(object sender, EventArgs e)
        {
            Port2_box.Items.Clear();
            Port2_box.Items.AddRange(SerialPort.GetPortNames());
        }
        private void Port3_box_DropDown(object sender, EventArgs e)
        {
            Port3_box.Items.Clear();
            Port3_box.Items.AddRange(SerialPort.GetPortNames());
        }
        private void Port4_box_DropDown(object sender, EventArgs e)
        {
            Port4_box.Items.Clear();
            Port4_box.Items.AddRange(SerialPort.GetPortNames());
        }
        private void Port5_box_DropDown(object sender, EventArgs e)
        {
            Port5_box.Items.Clear();
            Port5_box.Items.AddRange(SerialPort.GetPortNames());
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///                                             Initial                                                ///
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        List<SerialPort> ports = new List<SerialPort>(new SerialPort[20]);
        List<ListView> listViews = new List<ListView>();


        private void PortsInit()
        {
            listViews = new List<ListView> { listView1, listView2, listView3, listView4, listView5};
            string[] PortName = new string[] { Port1_box.Text, Port2_box.Text, Port3_box.Text, Port4_box.Text, Port5_box.Text};

            for (int i = 0; i < 5; i++)
            {
                if(PortName[i] != "")
                {
                    ports[i] = new SerialPort(PortName[i], 9600);
                }
                else
                {
                    ports[i] = null;
                }
            }
        }

        
    }
}
