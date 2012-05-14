using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace GasManager
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();

            editTreeView.Nodes[1].Nodes.Add("Nguyễn Văn A");
            editTreeView.Nodes[1].Nodes.Add("Trịnh Thanh B");

            editTreeView.Nodes[2].Nodes.Add("Nguyễn Văn A");
            editTreeView.Nodes[2].Nodes.Add("Trịnh Thanh B");

            editTreeView.AfterSelect += new TreeViewEventHandler(editTreeView_AfterSelect);
        }

        void editTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {

            // Code edit o day
            if (e.Node.Level > 0 && e.Node.Parent.Index == 1)
            {
                //MessageBox.Show("Edit ha cung index = " + e.Node.Index + " level = " + e.Node.Level);
                editPanel.Visible = true;
                listPanel.Visible = false;
                
            }
            // Code add o day
            else if (e.Node.Level > 0 && e.Node.Parent.Index == 2)
            {
                listPanel.Visible = false;
                editPanel.Visible = true;
            }
            else {
                listPanel.Visible = true;
                editPanel.Visible = false;
            }

            //if (e.Node.Level != 0)
            //MessageBox.Show("Index = " + e.Node.Index + " level = " + e.Node.Level+ " Parent index = " + e.Node.Parent.Index);
        }




    }
}
