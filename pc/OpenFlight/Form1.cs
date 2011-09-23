using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace OpenFlight
{
    public partial class Form1 : Form
    {
        private Drone drone = new Drone();
        private UdpLogger log;
        private int connectcount=0;

        public Form1()
        {
            InitializeComponent();

            label2.Text=                 
                "Return  Takeoff\n" + 
                " Space  Motors OFF (be careful - drone falls out off the sky!!!)\n" +
                "     I  Fly forward\n"+
                "     ,  Fly backward\n"+
                "     J  Fly leftward\n"+
                "     L  Fly rightward\n"+
                "     K  Hover\n" +
                "     W  Fly 5cm higher\n"+
                "     S  Fly 5cm lower\n"+
                "     A  Rotate 5 degrees anticlockwise\n"+
                "     D  Rotate 5 degrees clockwise\n" +
                "\n" +
                "Note: All keypresses on this PC are captured, even if this window is not active!!!\n";
            lblState.Text = string.Format("Start 'fly' on the drone then press Space to connect...");
            label1.Text = string.Format("");
            lblPos.Text = string.Format("");

            log = new UdpLogger(7778, string.Format("{0:yyyyMMddHHmmss}_navlog.csv", DateTime.Now));
            timer1.Interval = 250;
            timer1.Enabled = true;

            InterceptKeys.Start(KeyDownCallback);
        }

        private void KeyDownCallback(int vkcode)
        {
            double pitch_roll_step = 1;
            Keys k = (Keys)vkcode;
            Console.WriteLine("keydown {0} {1}", k,vkcode);

            if (log.msgcount > 0)
            {
                switch ((Keys)vkcode)
                {
                    //K Neutral
                    case Keys.K:
                        drone.Pitch = 0;
                        drone.Roll = 0;
                        drone.SendCmd();
                        break;
                    //I Fly forward
                    case Keys.I:
                        drone.Pitch += DEG2RAD(pitch_roll_step);
                        drone.SendCmd();
                        break;
                    //, Fly backward
                    case Keys.Oemcomma:
                        drone.Pitch -= DEG2RAD(pitch_roll_step);
                        drone.SendCmd();
                        break;
                    //J Fly leftward
                    case Keys.J:
                        drone.Roll -= DEG2RAD(pitch_roll_step);
                        drone.SendCmd();
                        break;
                    //L Fly rightward
                    case Keys.L:
                        drone.Roll += DEG2RAD(pitch_roll_step);
                        drone.SendCmd();
                        break;
                    //S Throttle down
                    case Keys.S:
                        drone.H -= 5;
                        drone.SendCmd();
                        break;
                    //W Throttle up
                    case Keys.W:
                        drone.H += 5;
                        drone.SendCmd();
                        break;
                    //A Rotate anticlockwise
                    case Keys.A:
                        drone.Yaw += DEG2RAD(5);
                        drone.SendCmd();
                        break;
                    //D Rotate clockwise
                    case Keys.D:
                        drone.Yaw -= DEG2RAD(5);
                        drone.SendCmd();
                        break;
                    //Space Land
                    case Keys.Space:
                        drone.Pitch = 0;
                        drone.Roll = 0;
                        drone.H = 0;
                        drone.SendCmd();
                        break;
                    //Return takeoff
                    case Keys.Return:
                        drone.Pitch = 0;
                        drone.Roll = 0;
                        drone.H = 75;
                        drone.SendCmd();
                        break;
                }
                lblPos.Text = string.Format("Setpoint: Pitch={0:0.0}° Roll={1:0.0}° Yaw={2:0.0}° Height={3:0}cm", RAD2DEG(drone.Pitch), RAD2DEG(drone.Roll), RAD2DEG(drone.Yaw), drone.H);
            }
            else
            {
                if ((Keys)vkcode == Keys.Space)
                {
                    drone.Pitch = 0;
                    drone.Roll = 0;
                    drone.H = 0;
                    drone.SendCmd();
                    connectcount++;
                    lblState.Text = string.Format("Drone wakeup packet {0} sent. Press Space to try again.",connectcount);
                }
            }
            
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (log.msgcount > 0)
            {
                lblState.Text = "Connected";
                label1.Text = string.Format("Nav Data logged to file {20}\nSequence nr={0} ax={1:+0.000;-0.000} ay={2:+0.000;-0.000} az={3:+0.000;-0.000} gx={4:+0.000;-0.000} gy={5:+0.000;-0.000} gz={6:+0.000;-0.000} hv={7:0.}\n\n"+
                    "HEIGHT  setpoint={8,6:0.0}cm  actual={9,6:0.0}cm  throttle    =  {10:0.000}\n" +
                    "PITCH   setpoint={11,6:+0.0;-0.0}°   actual={12,6:+0.0;-0.0}°   throttle adj= {13:+0.000;-0.000}\n" +
                    "ROLL    setpoint={14,6:+0.0;-0.0}°   actual={15,6:+0.0;-0.0}°   throttle adj= {16:+0.000;-0.000}\n" +
                    "YAW     setpoint={17,6:+0.0;-0.0}°   actual={18,6:+0.0;-0.0}°   throttle adj= {19:+0.000;-0.000}" 
                , log.msgcount, log.ax, log.ay, log.az, log.gx, log.gy, log.gz, log.act_hv
                , log.set_h, log.act_h, log.throttle
                , log.set_pitch, log.act_pitch, log.throttle_pitch_adj
                , log.set_roll, log.act_roll, log.throttle_roll_adj
                , log.set_yaw, log.act_yaw, log.throttle_yaw_adj
                , log.FileName);
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            InterceptKeys.Stop();
        }

        //degrees to radians
        private double DEG2RAD(double x)
        {
            return x * 3.1415926 / 180;
        }
        //radians to degrees
        private double RAD2DEG(double x)
        {
            return x / 3.1415926 * 180;
        }

    }
}
