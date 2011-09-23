using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.IO;
using System.Diagnostics;
using System.Threading;

public class UdpLogger
{
    private Stopwatch sw;
    private StreamWriter writer;
    private Socket newsock;
    private EndPoint Remote;
    public int msgcount=0;

    public float ax;
    public float ay;
    public float az;
    public float gx;
    public float gy;
    public float gz;

    public float act_h;
    public float act_pitch;
    public float act_roll;
    public float act_yaw;
    public float act_hv;

    public float set_pitch;
    public float set_roll;
    public float set_yaw;
    public float set_h;

    public float throttle;
    public float throttle_pitch_adj;
    public float throttle_roll_adj;
    public float throttle_yaw_adj;

    public string FileName;

    public UdpLogger(int Port, string FileName)
    {
        this.FileName = FileName;

        IPEndPoint ipep = new IPEndPoint(IPAddress.Any, Port);

        newsock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

        newsock.Bind(ipep);
        //Console.WriteLine("Waiting for a client...");

        IPEndPoint sender = new IPEndPoint(IPAddress.Any, 0);
        Remote = (EndPoint)(sender);

        Thread t = new Thread(LogWorker);
        t.IsBackground = true;
        t.Start();

        //writer.Close();
    }

    private void LogWorker()
    {
        double ts_trigger = -1;
        int recv;
        byte[] data = new byte[1024];
        while (true)
        {
            recv = newsock.ReceiveFrom(data, ref Remote);
            if (sw == null)
            {
                sw = new Stopwatch();
                sw.Start();
            }
            double ts = (double)sw.ElapsedMilliseconds/1000;
            msgcount++;
            string s = Encoding.ASCII.GetString(data, 0, recv);

            try
            {
                string[] ss = s.Split(',');
                ax = float.Parse(ss[2]);
                ay = float.Parse(ss[3]);
                az = float.Parse(ss[4]);
                gx = float.Parse(ss[5]);
                gy = float.Parse(ss[6]);
                gz = float.Parse(ss[7]);
                act_hv = float.Parse(ss[8]);

                set_h = float.Parse(ss[9]);
                act_h = float.Parse(ss[10]);
                throttle = float.Parse(ss[11]);

                set_pitch = float.Parse(ss[12]);
                act_pitch = float.Parse(ss[13]);
                throttle_pitch_adj = float.Parse(ss[14]);

                set_roll = float.Parse(ss[15]);
                act_roll = float.Parse(ss[16]);
                throttle_roll_adj = float.Parse(ss[17]);

                set_yaw = float.Parse(ss[18]);
                act_yaw = float.Parse(ss[19]);
                throttle_yaw_adj = float.Parse(ss[20]);
            }
            catch { }

            if (ts_trigger<0 && throttle_yaw_adj > 0)
            {
                ts_trigger = ts;
            }
            if (ts_trigger >= 0)
            {
                if (writer == null)
                {
                    writer = new StreamWriter(FileName);
                    writer.WriteLine("tsrec,seq,ts,ax,ay,az,gx,gy,gz,hv,height_setpoint,height_actual,throttle,pitch_setpoint,pitch_actual,pitch_throttle_adj,roll_setpoint,roll_actual,roll_throttle_adj,yaw_setpoint,yaw_actual,yaw_throttle_adj");
                }
                writer.Write(string.Format("{0:0.000}", ts - ts_trigger));
                writer.Write(",");
                writer.Write(s);
                writer.Write(Environment.NewLine);
                writer.Flush();
            }
        }
    }
}
