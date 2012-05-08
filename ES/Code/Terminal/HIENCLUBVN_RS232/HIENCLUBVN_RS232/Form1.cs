using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.IO;
using System.IO.Ports;
using System.Xml;

namespace HIENCLUBVN_RS232
{
    public partial class Form1 : Form
    {
        SerialPort P = new SerialPort();
        string InputData = String.Empty;
        delegate void SetTextCallback(string text);
        public Form1()
        {
            InitializeComponent();
            string[] ports = SerialPort.GetPortNames();
            cbCom.Items.AddRange(ports);
            P.ReadTimeout = 1000;
            P.DataReceived += new SerialDataReceivedEventHandler(DataReceive);
            string[] BaudRate = { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
                cbRate.Items.AddRange(BaudRate);
            string[] Databits = {"8" };
                cbBits.Items.AddRange(Databits);
            string[] Parity = { "None"};
            cbParity.Items.AddRange(Parity);

            string[] stopbit = { "1"};
            cbBit.Items.AddRange(stopbit);
        }
        private void cbCom_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (P.IsOpen) 
            {
                P.Close();
            }
            P.PortName = cbCom.SelectedItem.ToString();
        }

        private void cbRate_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (P.IsOpen)
            {
                P.Close();
            }
            P.BaudRate = Convert.ToInt32(cbRate.Text);
        }

        private void cbBits_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (P.IsOpen)
            {
                P.Close();
            }
            P.DataBits = Convert.ToInt32(cbBits.Text);
        }

        private void cbParity_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (P.IsOpen)
            {
                P.Close();
            }
            switch (cbParity.SelectedItem.ToString())
            {
                case "Odd":
                    P.Parity = Parity.Odd;
                    break;
                case "None":
                    P.Parity = Parity.None;
                    break;
                case "Even":
                    P.Parity = Parity.Even;
                    break;
            }
        }

        private void cbBit_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (P.IsOpen)
            {
                P.Close();
            }
            switch (cbBit.SelectedItem.ToString())
            {
                case "1":
                    P.StopBits = StopBits.One;
                    break;
                case "1.5":
                    P.StopBits = StopBits.OnePointFive;
                    break;
                case "2":
                    P.StopBits = StopBits.Two;
                    break;
            }
        }
        private void DataReceive(object obj, SerialDataReceivedEventArgs e)
        {
            InputData = P.ReadExisting();
            if (InputData != String.Empty)
            {
                SetText(InputData);
            }
        }
        private void SetText(string text)
        {
            if (this.txtkq.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText);
                this.Invoke(d, new object[] { text });
            }
            else this.txtkq.Text += text;
        }
        private void btSend_Click(object sender, EventArgs e)
        {
            if (P.IsOpen)
            {
                if (txtSend.Text == "") MessageBox.Show("Chưa có dữ liệu!","Thông Báo");
                else P.Write(txtSend.Text);
               
            }
            else MessageBox.Show("COM chưa mở.", "Thông báo", MessageBoxButtons.OK, MessageBoxIcon.Error);
            txtSend.Clear();
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            try
            {
                cbCom.SelectedIndex = 0;
                cbRate.SelectedIndex = 4;
                cbBits.SelectedIndex = 0;
                cbParity.SelectedIndex = 0;
                cbBit.SelectedIndex = 0;
                status.Text = "\0Hãy chọn 1 cổng COM để kết nối.";
            }
            catch (Exception ex)
            {
                MessageBox.Show("Không có cổng COM");
            }
        }

        private void btKetNoi_Click(object sender, EventArgs e)
        {
            try
            {
                P.Open();
                btNgat.Enabled = true;
                btKetNoi.Enabled = false;
                status.Text = "Đang kết nối với cổng " + cbCom.SelectedItem.ToString();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Không kết nối được.","Thử lại",MessageBoxButtons.OK,MessageBoxIcon.Error);
            }
        }

        private void btNgat_Click(object sender, EventArgs e)
        {
            P.Close();
            btKetNoi.Enabled = true;
            btNgat.Enabled = false;
            status.Text = "Đã Ngắt Kết Nối";
        }

        private void btThoat_Click(object sender, EventArgs e)
        {
            DialogResult kq = MessageBox.Show("Bạn thực sự muốn thoát", "HIENCLUBVN", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (kq == DialogResult.Yes)
            {
                MessageBox.Show("Cảm ơn bạn đã sử dụng chương trình","HIENCLUBVN");
                this.Close();
            }
        }

        private void btXoa_Click(object sender, EventArgs e)
        {
            txtkq.Text = "";
            txtSend.Text = "";
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form2 frm = new Form2();
            frm.ShowDialog();
        }

        
    }
}
