using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

public class label
{
    //page1
    public static List<Label> WE;
    public static List<Label> AE;
    //page2
    public static List<Label> ADC_Value;
    public static List<Label> ADC;
    //page3
    public static List<Label> Zero_Value;
    public static List<Label> Zero;
    //page5
    public static List<Label> Span_Value;
    public static List<Label> Span;
    //page6
    public static List<Label> BSN_Value;
    public static List<Label> BSN;
    //page7
    public static List<Label> Sensor_Name;
    //page8
    public static List<Label> FW;
    //SHT
    public static List<Label> Temp;
    public static List<Label> RH;
    public static List<Label> AH;
    //LED
    public static List<Label> WEAE_LED;
}

public class Sabio
{

    public static string RxString = "";
    public static string RxInfo = "";
    public static int CmdStatus = 0; //0:idle 1:success -1:error
    public static int CmdIndex = 0;
    public static int CmdTimeCount = 0;
    public static int UpdateRate = 5000;
    public static string FileName = "";
    public static float CsvTimeCount = 0;
    public static int CommandMode = 0; //0:dilution 1:gas_ppb_eq0 2:gas_ppb_neq0 3:MS/TS/Stop/Purge
}

public class AlphFix
{

    public static string RxString = "";
    public static string RxInfo = "";
    public static int CmdStatus = 0; //0:idle 1:success 2:wait -1:error
    public static int UpdateRate = 5000;
    public static string FileName = "";
    public static int CapTimeCount = 1;
    public static float CsvTimeCount = 0;
    public static int DisplayTimeCount = 1;
    public static int pass_time = 0;
    public static int CommandMode = 0; //0:read 1:write
    public static bool CapFlag = false;
    public static int NumOfSensor;
    public static string[] Sensor_Name = { "----", "GS+4CO", "AL-NO2", "AL-O3", "GS+7NH3", "AL-H2S", "AL-CO", "AL-SO2" };
}