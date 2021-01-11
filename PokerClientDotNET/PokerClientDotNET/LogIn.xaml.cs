using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace PokerClientDotNET
{
    /// <summary>
    /// Interaction logic for LogIn.xaml
    /// </summary>
    public partial class LogIn : Window
    {
        public LogIn()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(LogIn_Loaded);
        }

        void LogIn_Loaded(object sender, RoutedEventArgs e)
        {
            this._logInBtn.Focus();
        }
                        
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            this._userName.Text = "";
            this.Close();
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            this._userName.Text = this._userName.Text.Trim();
            if (this._userName.Text == "") {
                MessageBox.Show("You must input something!");
                return;
            }
            this.Close();
        }
    }
}
