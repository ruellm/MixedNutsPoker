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
    /// Interaction logic for BuyInWnd.xaml
    /// </summary>
    public partial class BuyInWnd : Window
    {
        public float BuyInAmnt = 0.0f;
        public float MaxBuyIn = 0.0f;

        public BuyInWnd()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(BuyInWnd_Loaded);
        }

        void BuyInWnd_Loaded(object sender, RoutedEventArgs e)
        {
            _buyInText.KeyUp += new KeyEventHandler(_buyInText_KeyUp);
            _bankRollText.Text = ClientSessionData.GetInstance()._MyBankRoll.ToString();
            _maxBuyInText.Text = MaxBuyIn.ToString();
            _buyInBtn.Focus();
        }

        void _buyInText_KeyUp(object sender, KeyEventArgs e)
        {
            float buyIn;

            try
            {
                buyIn = System.Convert.ToSingle(_buyInText.Text);
                if (buyIn > ClientSessionData.GetInstance()._MyBankRoll)
                {
                    _buyInText.Text = ClientSessionData.GetInstance()._MyBankRoll.ToString();
                }
            }
            catch {
                _buyInText.Text = "0";
            }    
        }
                

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            try
            {
                BuyInAmnt = System.Convert.ToSingle(_buyInText.Text);
                if (BuyInAmnt > MaxBuyIn)
                {
                    MessageBox.Show("Max buy-in allowed is " + MaxBuyIn + " for this table");
                    return;
                }

                ClientSessionData.GetInstance()._MyBankRoll -= BuyInAmnt;
                this.Close();
            }
            catch
            { }            
        }
    }
}
