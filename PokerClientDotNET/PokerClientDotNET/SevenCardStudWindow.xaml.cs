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
using PokerLibDotNET;
using System.Windows.Threading;
using System.Threading;
namespace PokerClientDotNET
{
    /// <summary>
    /// Interaction logic for SevenCardStudWindow.xaml
    /// </summary>
    public partial class SevenCardStudWindow : Window
    {
        public GameInformation _GameInfo;
        public PacketSender _PacketSender;
        private List<PlayerSeat> SeatsCtrl;
        private const int _NumSeats = 7; //52 deck/ 7 cards
        private bool _bSitDown = false;
        private Player _Me;
        private short _MySeat;
        private ILimitUIHelper _LimitHelper;

        public SevenCardStudWindow()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(SevenCardStudWindow_Loaded);
        }

        void SevenCardStudWindow_Loaded(object sender, RoutedEventArgs e)
        {
            if (_PacketSender == null) return;
            _PacketSender.SendReqSpectateGame(_GameInfo._GameID);

            SeatsCtrl.Add(_seat0);
            SeatsCtrl.Add(_seat1);
            SeatsCtrl.Add(_seat2);
            SeatsCtrl.Add(_seat3);
            SeatsCtrl.Add(_seat4);
            SeatsCtrl.Add(_seat5);
            SeatsCtrl.Add(_seat6);


            //Loop does not work in here
            SeatsCtrl[0]._btnSeat.Click += delegate { SetDown(0); };
            SeatsCtrl[1]._btnSeat.Click += delegate { SetDown(1); };
            SeatsCtrl[2]._btnSeat.Click += delegate { SetDown(2); };
            SeatsCtrl[3]._btnSeat.Click += delegate { SetDown(3); };
            SeatsCtrl[4]._btnSeat.Click += delegate { SetDown(4); };
            SeatsCtrl[5]._btnSeat.Click += delegate { SetDown(5); };
            SeatsCtrl[6]._btnSeat.Click += delegate { SetDown(6); };
            SeatsCtrl[7]._btnSeat.Click += delegate { SetDown(7); };
            SeatsCtrl[8]._btnSeat.Click += delegate { SetDown(8); };
            SeatsCtrl[9]._btnSeat.Click += delegate { SetDown(9); };       

            //adjust hole cards size
            //for(int x=0;x<_NumSeats;x++){
            //    SeatsCtrl[x]._holeCards.Width = 
            // }

            //key is pressed in chat textbox
            _textBoxChat.KeyUp += new KeyEventHandler(_textBoxChat_KeyUp);

            //limit ui helper
            _LimitHelper = LimitUIFactory.CreateLimitHelper(_GameInfo._Info.limit_type);
            _LimitHelper._GameInfo = _GameInfo;
            _LimitHelper._RaiseButton = _raiseBtn;            
        }

        private void SetDown(short chair)
        {
            //check if he is loggedin 
            if (!ClientSessionData.GetInstance().IsLoggedIn())
            {
                DoLoggedIn(chair);
                return;
            }

            DoSitDown(chair);
        }

        private void DoSitDown(short chair)
        {
            BuyInWnd buyIn = new BuyInWnd();
            buyIn.MaxBuyIn = _GameInfo._Info.stake * 100.0f;
            buyIn.Owner = this;
            buyIn.ShowDialog();

            _bSitDown = true;
            _MySeat = chair;
            _standUpButton.Visibility = Visibility.Visible;

            //Send request to sitdown
            _PacketSender.SendJoinGame(_GameInfo._GameID, chair, buyIn.BuyInAmnt);
        }

        private void DoLoggedIn(short chair)
        {
            LogIn log = new LogIn();
            log.Owner = this;
            log.ShowDialog();
            if (log._userName.Text != "")
            {
                ClientSessionData.GetInstance().DoLogIn(log._userName.Text,
                    new NextFuncDelegate(LoggedInDelegate), (object)chair);
            }
        }

        private void LoggedInDelegate(StatusType error, object parm)
        {
            if (error == StatusType.Success)
                DoSitDown((short)parm);
            else
                MessageBox.Show("Error Logging in to server \r Name maybe exist!");
        }

        private void _standUpButton_Click(object sender, RoutedEventArgs e)
        {
            if (_GameInfo._Table._CurrentBettor == _MySeat)
            {
                _PacketSender.SendAction(PokerAction.eFold,
                0.0f,
                _GameInfo._GameID);
                Thread.Sleep(100);
            }

            _PacketSender.SendStandUpRoom(_GameInfo._GameID);
            _bSitDown = false;
            _MySeat = PokerLibConstants.POKER_INVALID_POSITION;

            _standUpButton.Visibility = Visibility.Collapsed;

            ToggleButtonVisible(false);

            //refund buyin
            ClientSessionData.GetInstance()._MyBankRoll += (_Me._TotalChips - _Me._fBet);
        }

        //chat functionality
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            string strMessage = ChatFormater.ClientToServer(_textBoxChat.Text);
            if (strMessage == "" || !ClientSessionData.GetInstance().IsLoggedIn()) return;
            _PacketSender.SendChat(strMessage, _GameInfo._GameID);
            _textBoxChat.Text = "";
        }

        void _textBoxChat_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key != Key.Enter) return;
            Button_Click(null, null);
        }

        private void ToggleButtonVisible(bool flag)
        {
            if (flag)
            {
                _checkBtn.Visibility = Visibility.Visible;
                _raiseBtn.Visibility = Visibility.Visible;
                _foldBtn.Visibility = Visibility.Visible;
                _callBtn.Visibility = Visibility.Visible;
                _raiseDoubleBtn.Visibility = Visibility.Visible;
            }
            else
            {
                _checkBtn.Visibility = Visibility.Collapsed;
                _raiseBtn.Visibility = Visibility.Collapsed;
                _foldBtn.Visibility = Visibility.Collapsed;
                _callBtn.Visibility = Visibility.Collapsed;
                _raiseDoubleBtn.Visibility = Visibility.Collapsed;
            }

        }
    }
}
