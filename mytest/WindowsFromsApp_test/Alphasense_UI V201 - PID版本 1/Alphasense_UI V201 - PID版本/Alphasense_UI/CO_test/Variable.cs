using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

public class label
{
    //page0
    public static List<Label> WE;
    public static List<Label> AE;
    //page1
    public static List<Label> PPB;
    public static List<Label> ADC;
    //page2
    public static List<Label> Zero;
    //page3
    public static List<Label> Span;
    //page4
    public static List<Label> A;
    //page5
    public static List<Label> B;
    //page6
    public static List<Label> Gas_Name;
    //page7
    public static List<Label> BSN; 
    //page8
    public static List<Label> FW;
    //page9
    public static List<Label> Cal_Date;
    //SHT
    public static List<Label> Temp;
    public static List<Label> RH;
    public static List<Label> AH;
    //LED
    public static List<Label> WEAE_LED;
}

public class group
{
    public static List<GroupBox> WEAE;
    public static List<GroupBox> ADC;
    public static List<GroupBox> Zero;
    public static List<GroupBox> Span;
    public static List<GroupBox> A;
    public static List<GroupBox> B;
    public static List<GroupBox> GasName;
    public static List<GroupBox> BSN;
    public static List<GroupBox> FW;
    public static List<GroupBox> CalDate;
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
    public static int CapTimeCount = 1;
    public static int pass_time = 0;
    public static int CommandMode = 0; //0:dilution 1:gas_ppb_eq0 2:gas_ppb_neq0 3:MS/TS/Stop/Purge
    public static bool O3Flag = false;
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
    public static int AverageTimeCount = 0;
    public static int pass_time = 0;
    public static int CommandMode = 0; //0:read 1:write
    public static bool CapFlag = false;
    public static int NumOfSensor;
    //public static string[] Sensor_Name = { "----", "GS+4CO", "AL-NO2", "AL-O3", "GS+7NH3", "AL-H2S", "AL-CO", "AL-SO2" };
}