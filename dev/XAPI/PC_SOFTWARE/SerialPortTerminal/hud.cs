using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SerialPortTerminal
{
    public partial class hud : Form
    {
        Util m_util;
        frmTerminal m_term;
        private const string NOT_CONNECTED = "No Packet Protocol Active";
        private const string CONNECTION_ESTABLISHED = "Packet Protocol Active";

        //*********************************************************
        //*********************************************************
        // routine to display a string
        public void DEBUG_display(byte[] msg)
        {
            string str_msg = m_util.GetString(msg);

            DEBUG_display(str_msg);

        }

        //*********************************************************
        //*********************************************************
        // routine to display a string
        public void DEBUG_display(string msg)
        {
            Invoke(new MethodInvoker(() =>
            {
               // txtDebug.Text = msg;
            }));
        }

        //*********************************************************
        //*********************************************************
        // Constructor
        public hud(frmTerminal term)
        {
            InitializeComponent();
            m_term = term;
            m_util = new Util();
           
        }

        //*********************************************************
        //*********************************************************
        // PONG was recieved from
        public void connection_established()
        {
            Invoke(new MethodInvoker(() =>
            {
                lblTop.BackColor = Color.Green;
                lblBottom.BackColor = Color.Green;
                lblTop.Text = CONNECTION_ESTABLISHED;
                lblBottom.Text = CONNECTION_ESTABLISHED;

                Application.DoEvents();
            }));
        }

        //*********************************************************
        //*********************************************************
        // To show that no PONG was recieved by PC Serial on Arduino
        public void no_connection()
        {
            Invoke(new MethodInvoker(() =>
            {
                lblTop.BackColor = Color.Red;
                lblBottom.BackColor = Color.Red;
                lblTop.Text = NOT_CONNECTED;
                lblBottom.Text = NOT_CONNECTED;

                Application.DoEvents();
            }));
        }

        //*********************************************************
        //*********************************************************
        public void debug_msg(byte[] msg)
        {
            string msg_str = m_util.GetString(msg);
            debug_msg(msg_str);
        }

        //*********************************************************
        //*********************************************************
        public void debug_msg(string msg)
        {
            Invoke(new MethodInvoker(() =>
            {
                // must remove null bytes from string
                string msg_new = msg.Replace("\0", string.Empty);

                DateTime saveNow = DateTime.Now;
                StringBuilder b_msg = new StringBuilder();
                b_msg.Append("[");
                b_msg.Append(saveNow);
                b_msg.Append("]  ");
                b_msg.Append(msg_new);
                b_msg.Append("\n");

                txtDebug.Text += b_msg.ToString();

                Application.DoEvents();
            }));
        }





        //*********************************************************
        //*********************************************************
        private void hud_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            debug_msg("this is a test");
        }
    }
}
