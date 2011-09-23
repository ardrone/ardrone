using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace OpenFlight
{
    class Drone
    {
        private double m_Pitch = 0;
        private double m_Roll = 0;
        private double m_Yaw = 0;
        private double m_H = 0;
        private IPEndPoint remote;
        private Socket newsock;

        public Drone()
        {
             remote = new IPEndPoint(IPAddress.Parse("192.168.1.1"), 7777);
             newsock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        }

        public double Pitch
        {
            get
            {
                return m_Pitch;
            }
            set
            {
                if (value > 1) value = 1;
                if (value < -1) value = -1;
                m_Pitch = value;
            }
        }

        public double Roll
        {
            get
            {
                return m_Roll;
            }
            set
            {
                if (value > 1) value = 1;
                if (value < -1) value = -1;
                m_Roll = value;
            }
        }

        public double Yaw
        {
            get
            {
                return m_Yaw;
            }
            set
            {
                if (value > 1) value = 1;
                if (value < -1) value = -1;
                m_Yaw = value;
            }
        }

        public double H
        {
            get
            {
                return m_H;
            }
            set
            {
                if (value > 600) value = 600;
                if (value < 0) value = 0;
                m_H = value;
            }
        }

        public void SendCmd()
        {
            byte[] data = new byte[1024];

            //EndPoint Remote = (EndPoint)(sender);

            data = Encoding.ASCII.GetBytes(string.Format("s,{0},{1},{2},{3}",m_Pitch,m_Roll,m_Yaw,m_H));
            newsock.SendTo(data, data.Length, SocketFlags.None, remote);
        }
    }
}
