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
    /// Interaction logic for HoldemWindow.xaml
    /// </summary>
    public partial class HoldemWindow : Window
    {
        public GameInformation _GameInfo;
        public PacketSender _PacketSender;
        private List<PlayerSeat> SeatsCtrl;
        private int _NumSeats = 10;
        
        private bool _bSitDown=false;  
        private Player _Me;
        private short _MySeat;
        private float _fCurrRaise=0.0f;
        private delegate void CallBackDelegate(object obj);
        private delegate void FunctionDelegate();

        private float _MaxBet = 0.0f;
        private short _CurrentBettor = PokerLibConstants.POKER_INVALID_POSITION;
        private TableState _CurrentState;

        //limit helper
        private ILimitUIHelper _LimitHelper;

        //blind raise helper 
        float _fNextStake = 0.0f;

        private List<Card> _CommunityCards;

        public HoldemWindow()
        {
            InitializeComponent();
            SeatsCtrl = new List<PlayerSeat>();
            _CommunityCards = new List<Card>();

            _MySeat = PokerLibConstants.POKER_INVALID_POSITION;

            this.Loaded += new RoutedEventHandler(HoldemWindow_Loaded);
            this.Closing += new System.ComponentModel.CancelEventHandler(HoldemWindow_Closing);
        }

        void HoldemWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (_GameInfo._Table._CurrentBettor == _MySeat)
            {
                _PacketSender.SendAction(PokerAction.eFold,
                0.0f,
                _GameInfo._GameID);
                Thread.Sleep(100);
            }

            _PacketSender.SendLeaveRoom(_GameInfo._GameID);

            //unsubscribe to events
            ClientSessionData.GetInstance().pfnRefresh -= Refresh;

            //refund buyin
            if(_MySeat != PokerLibConstants.POKER_INVALID_POSITION)
                ClientSessionData.GetInstance()._MyBankRoll += (_Me._TotalChips - _Me._fBet);
        }

        void HoldemWindow_Loaded(object sender, RoutedEventArgs e)
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
            SeatsCtrl.Add(_seat7);
            SeatsCtrl.Add(_seat8);
            SeatsCtrl.Add(_seat9);

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

            _standUpButton.Visibility = Visibility.Collapsed;
            ToggleButtonVisible(false);

            _checkBtn.Click += new RoutedEventHandler(_checkBtn_Click);
            _foldBtn.Click += new RoutedEventHandler(_foldBtn_Click);
            _callBtn.Click += new RoutedEventHandler(_callBtn_Click);
            _raiseBtn.Click += new RoutedEventHandler(_raiseBtn_Click);

            _raiseTextBox.KeyUp += new KeyEventHandler(_raiseTextBox_KeyUp);
            
            this.Title = _GameInfo._Info.strDescription + " " + _GameInfo.Stake;

            //key is pressed in chat textbox
            _textBoxChat.KeyUp += new KeyEventHandler(_textBoxChat_KeyUp);

            //_raiseTextBox.TextChanged += new TextChangedEventHandler(raiseTextBox_TextChanged);

            //limit ui helper
            _LimitHelper = LimitUIFactory.CreateLimitHelper(_GameInfo._Info.limit_type);
            _LimitHelper._GameInfo = _GameInfo;
            _LimitHelper._RaiseButton = _raiseBtn;
            _LimitHelper._RaiseTextBox = _raiseTextBox;

            if (_GameInfo._Info.limit_type == GameLimitType.ePotLimit)
            {
                ((PotLimitUIHelper)_LimitHelper)._PotButton = _PotButton;
                _PotButton.Click += new RoutedEventHandler(_PotButton_Click);
            }

            _NumSeats = _GameInfo._Info.max_players;
            for (int s = 0; s < 10 - _NumSeats; s++)
                SeatsCtrl[_NumSeats + s].IsEnabled = false;
           
            _fNextStake = _GameInfo._Info.stake;

            _ShowHand.Click+=new RoutedEventHandler(_ShowHand_Click);
            _Muck.Click += delegate { _Muck.Visibility = Visibility.Hidden; };
        }

        void _ShowHand_Click(object sender, RoutedEventArgs e)
        {
            _PacketSender.SendReqRoomParam(GamePacket.eShowHands, 
                0, _GameInfo._GameID);

            _ShowHand.Visibility = Visibility.Hidden;
            _Muck.Visibility = Visibility.Hidden;

        }               

        void _PotButton_Click(object sender, RoutedEventArgs e)
        {            
            float PotCall = ((PotLimitUIHelper)_LimitHelper).GetPotLimitRaise(_Me);
            if (PotCall > _MaxBet)
                PotCall = _MaxBet;
            _raiseTextBox.Text = PotCall.ToString();

            if (PotCall >= _Me._TotalChips)
                _raiseBtn.Content = "AllIn " + _raiseTextBox.Text;
            else
                _raiseBtn.Content = "Raise " + _raiseTextBox.Text;

            _fCurrRaise = PotCall;
        }

        void _raiseTextBox_KeyUp(object sender, KeyEventArgs e)
        {
            float fStake = 0.0f;
            try
            {
                fStake = System.Convert.ToSingle(_raiseTextBox.Text);

                //limit maximum bet to biggest chips
                if (fStake > _MaxBet)
                {
                    _raiseTextBox.Text = _MaxBet.ToString();
                    fStake = _MaxBet;
                }
                else if (fStake > _Me._TotalChips)
                {
                    _raiseTextBox.Text = _Me._TotalChips.ToString();
                    fStake = _Me._TotalChips;
                }
               
                //evaluate minimum stake
                //if (GetCurrentPot()._currRaise == 0 && fStake < _GameInfo._Info.stake)
                //{
                //    _raiseTextBox.Text = _GameInfo._Info.stake.ToString();
                //    fStake = _GameInfo._Info.stake;
                //}
                //else if (fStake < GetCurrentPot()._currRaise)
                //{
                //    _raiseTextBox.Text = GetCurrentPot()._currRaise.ToString();
                //    fStake = GetCurrentPot()._currRaise;
                //}

                if (fStake >= _Me._TotalChips)
                    _raiseBtn.Content = "AllIn " + _raiseTextBox.Text;
                else
                    _raiseBtn.Content = "Raise " + _raiseTextBox.Text;

                _fCurrRaise = fStake;
            }
            catch
            {
                //float display = GetCurrentPot()._currRaise + _GameInfo._Info.stake; ;
                //_raiseBtn.Content = "Raise " + display.ToString();
            }
        }        
             
        private void ToggleButtonVisible(bool flag) 
        {
            if (flag)
            {
                _checkBtn.Visibility = Visibility.Visible;
                _raiseBtn.Visibility = Visibility.Visible;
                _foldBtn.Visibility = Visibility.Visible;
                _callBtn.Visibility = Visibility.Visible;
                _raiseTextBox.Visibility = Visibility.Visible;
            }
            else {
                _checkBtn.Visibility = Visibility.Collapsed;
                _raiseBtn.Visibility = Visibility.Collapsed;
                _foldBtn.Visibility = Visibility.Collapsed;
                _callBtn.Visibility = Visibility.Collapsed;
                _raiseTextBox.Visibility = Visibility.Collapsed;
            }
            _raiseUpBtn.Visibility = Visibility.Collapsed;
            _PotButton.Visibility = Visibility.Collapsed;
        }

        void _callBtn_Click(object sender, RoutedEventArgs e)
        {
            if (_Me._TotalChips <= GetCurrentPot()._currRaise)
            {
                _PacketSender.SendAction(PokerAction.eAllIn,
                    _Me._TotalChips,
                    _GameInfo._GameID);
            }
            else
            {
                _PacketSender.SendAction(PokerAction.eCall,
                    GetCurrentPot()._currRaise -_Me._fBet,
                    _GameInfo._GameID);
            }
            ToggleButtonVisible(false);
        }
        void _foldBtn_Click(object sender, RoutedEventArgs e)
        {
            _PacketSender.SendAction(PokerAction.eFold, 
                0.0f, 
                _GameInfo._GameID);

            ToggleButtonVisible(false);
        }

        void _checkBtn_Click(object sender, RoutedEventArgs e)
        {
            _PacketSender.SendAction(PokerAction.eCheck,
                0.0f,
                _GameInfo._GameID);

            ToggleButtonVisible(false);
        }        

        private float GetMinimumBet()
        {
            float minBet = 0.0f;

            if(_GameInfo._Info.limit_type == GameLimitType.eNoLimit || 
                _GameInfo._Info.limit_type == GameLimitType.ePotLimit)
            {
                minBet = _GameInfo._Info.stake;
            }
            else
            {
                TableState state = _GameInfo._Table._State;
                if (PokerLibGlobal.IsSevenCardStudGame(_GameInfo._Info.game_type))
                {                   
                    if (state == TableState.e5thStreetState ||
                       state == TableState.e6thStreetState ||
                       state == TableState.e7thStreetState)
                    {
                        minBet = _GameInfo._Info.stake;
                    }else{
                        if (IsBringInRound())
                        {
                            minBet = _GameInfo._Info.fBringIn;
                        }
                        else
                        {
                            minBet = _GameInfo._Info.stake / 2.0f;
                        }
                    }
                }else if(PokerLibGlobal.IsHoldemGame(_GameInfo._Info.game_type)){
                   
                    if (state == TableState.eTurnState || state == TableState.eRiverState) 
                    {
                        minBet = _GameInfo._Info.stake;
                    }
                    else
                    {
                        minBet = _GameInfo._Info.stake / 2.0f;
                    }
                }            
            }
            float newMin=(minBet + GetCurrentPot()._currRaise);
            if (newMin > _Me._TotalChips)
                return _Me._TotalChips;
            if (newMin > _MaxBet)
                return _MaxBet;
            return newMin;
        }

        void _raiseBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (_fCurrRaise > _MaxBet)
                {
                    MessageBox.Show("You cannot raise more than other people total chips");
                    return;                    
                }
               
                if (_fCurrRaise > _Me._TotalChips)
                {
                    MessageBox.Show("Raise exceed total chips");
                    return;
                }
                float min = GetMinimumBet();
                if (_fCurrRaise < GetMinimumBet())
                {
                    MessageBox.Show("Raise/bet is less than minimum bet ($" + min.ToString() + ")");
                    return;
                }

                //this part TBD
                float remainder = _fCurrRaise % (_GameInfo._Info.stake / 2.0f);
                if (remainder != 0.0f && 
                    _GameInfo._Info.limit_type != GameLimitType.eFixedLimit &&                    
                    _fCurrRaise != _Me._TotalChips &&
                    _fCurrRaise > min)                    
                {
                    MessageBox.Show("Raise/bet must be divisible by " +
                        (_GameInfo._Info.stake / 2.0f).ToString() + "/" +
                        _GameInfo._Info.stake.ToString());
                    return;
                }

                if (_fCurrRaise == _Me._TotalChips)
                {
                    _PacketSender.SendAction(PokerAction.eAllIn,
                        _Me._TotalChips, _GameInfo._GameID);
                }
                else
                {
                    _PacketSender.SendAction(PokerAction.eRaise,
                        _fCurrRaise, _GameInfo._GameID);
                }
            }
            catch
            {
                MessageBox.Show("Invalid raise");
            }
            ToggleButtonVisible(false);
        }

        public void HandlePacket(NetPacketRoom packet) 
        {
            switch (packet._gamePacketID)
            { 
                //case GamePacket.eHoldemTableSnapShot:
                //    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                //               new CallBackDelegate(UpdateTable), packet);
                //    break;

                case GamePacket.eTableSnapShot:
                    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                               new CallBackDelegate(UpdateTablePoker), packet);
                    break;

                case GamePacket.eWaitBet:
                    if (PokerLibGlobal.IsSevenCardStudGame(_GameInfo._Info.game_type))
                    {
                         this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                   new CallBackDelegate(SevenCardBettingRound), null);
                    }
                    else {
                        this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                   new CallBackDelegate(BettingRound), null);
                    }                          
                    break;
                case GamePacket.e7thStreet:
                case GamePacket.eHoleCard:
                    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                new CallBackDelegate(HoleCards), packet);
                    break;
                case GamePacket.eFlop:                    
                case GamePacket.eTurn:
                case GamePacket.eRiver:
                case GamePacket.eCommunityCard:
                    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                new CallBackDelegate(CommunityCards), packet);
                    break;
                case GamePacket.eWinner:
                    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                new CallBackDelegate(HandleWinner), packet);
                    break;
                case GamePacket.eShowHands:
                    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                new CallBackDelegate(ShowHandPlayer), packet);
                    break;
                case GamePacket.eChatMsg:
                    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                new CallBackDelegate(ChatMessage), packet);
                    break;

                //7 card stud group
                case GamePacket.eOpenCards:
                case GamePacket.e3rdStreet:
                case GamePacket.e4rthStreet:
                case GamePacket.e5thStreet:
                case GamePacket.e6thStreet:
                    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                new CallBackDelegate(OpenCards), packet);                    
                    break;
                case GamePacket.eBlindRaise:
                    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                new CallBackDelegate(BlindsRaise), packet);  
                    break;
                case GamePacket.e4thStreetParam:
                    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                new CallBackDelegate(RaiseUpChance), packet);  
                    break;
                    
            }
        }

        private void BlindsRaise(object obj)
        {
            _fNextStake += (_GameInfo._Info.stake * _GameInfo._Info.blindsInfo.raise_factor);
            string strMessage = string.Format("Blinds raise next hand to {0}/{1}...",
               _fNextStake / 2.0,
                _fNextStake);
            AddStatusMessage( strMessage + "\r",
              Brushes.Pink, FontWeights.Bold);
        }
        
        private void ChatMessage(object obj)
        {
            NetPacketChatMsg packet = (NetPacketChatMsg)obj;
            AddStatusMessage(ClientSessionData.GetInstance().GetClientName(packet._PlayerID) + ": ", 
                Brushes.Blue, FontWeights.Normal);
            AddStatusMessage(ChatFormater.ServerToClient(packet._Message) + "\r", 
                Brushes.Black, FontWeights.Normal);
        }

        private void ShowHandPlayer(object obj)
        {
            NetPacketShowHands showhand = (NetPacketShowHands)obj;
            //find player
            for (int x = 0; x < _GameInfo._Table._Seats.Count; x++) {
                if (!_GameInfo._Table._Seats[x]._bOccupied) continue;
                if (x == _MySeat) continue;
                if (showhand._PlayerID == _GameInfo._Table._Seats[x].player._SessionID) {
                    EmptyHoleCard((short)x);
                    for (int z = 0; z < showhand._Hands.Count; z++)
                    {                        
                        AddHoleCard((short)x, showhand._Hands[z]);
                        SeatsCtrl[x].ShowCards = true;
                    }
                }
            }
        }

        private void HandleWinner(object obj)
        {
            NetPacketWinner winner = (NetPacketWinner) obj;
            string strMessage=""; //Not final
            string strPotName="";
            string strHandMessage="";
            Pot pot = _GameInfo._Table._PotList[winner._PotNum];
            string strMessageHiLo = " for Low half of the pot with ";
            string strCardsList = "";

            //form hilo message
            for (int c = 0; c < winner._Hands.Count; c++) {
                strCardsList += GetCardString(winner._Hands[c]);
                if (c + 1 < winner._Hands.Count)
                    strCardsList += ",";
            }
            
            if (winner._PotNum == 0)
                strPotName = "($" + winner._Amount + ") from main pot";
            else
                strPotName = "($" + winner._Amount + ") from side pot";
            
            if (winner._PList.Count > 1)
            {
                strMessage = "Board is a split pot";
                if (winner._Rank == HandRanking.LowHand)
                {
                    strMessage += strMessageHiLo + strCardsList;
                }
                
                else {
                    strMessage += " " +
                        PokerHandEvaluator.GetHandString(winner._Rank, winner._Hands);
                }
                
                strMessage += "\r";
                for (int pc = 0; pc < winner._PList.Count; pc++) {
                    string pName = ClientSessionData.GetInstance().GetClientName(winner._PList[pc]);
                    string strIndivMsg = pName + " wins " + strPotName +"\r";
                    strMessage += strIndivMsg;
                }
                
                //todo animate split pot

            }
            else if (winner._PList.Count ==1)
            {
                strMessage = "{0} wins {1} {2}"; //Not final;
                if (winner._Rank == HandRanking.LowHand)
                {
                    strMessage += strMessageHiLo + strCardsList;
                }

                if (winner._Rank != HandRanking.NoRank && 
                    !PokerLibGlobal.IsLowBall(_GameInfo._Info.game_type))
                {
                    strHandMessage = PokerHandEvaluator.GetHandString(winner._Rank, winner._Hands);
                }
                else if (PokerLibGlobal.IsLowBall(_GameInfo._Info.game_type))
                {
                    strHandMessage = "using low cards " + strCardsList;
                }

                strMessage = string.Format(strMessage,
                    ClientSessionData.GetInstance().GetClientName(winner._PList[0]),
                    strPotName,
                    strHandMessage);
            }
            else if (winner._PList.Count == 0)
            {
                if (winner._Rank == HandRanking.NoRank) {
                    strMessage = "There are no hand qualified to play Low.";
                }            
            }
            
            AddStatusMessage(strMessage+"\r", Brushes.Green,FontWeights.Bold);
            HighlightWinnerCards(winner);

            bool bBelong = false;
            for (int cp = 0; cp < winner._PList.Count; cp++)
                if (winner._PList[cp] == _Me._SessionID)
                    bBelong = true;
            
            //if (!bBelong && 
            //    _GameInfo._Table._State == TableState.eShowDownState
            //    && winner._Rank == HandRanking.NoRank) //if player did not win during showdown only
            //    _textLose.Visibility = Visibility.Visible;
            ShowMuckCards();
        }

        private void ShowMuckCards()
        { 
            _ShowHand.Visibility = Visibility.Visible;
            _Muck.Visibility= Visibility.Visible;            
        }

        private void AddStatusMessage(string message, Brush color, FontWeight weight)
        {
            TextRange range = new TextRange(_StatusMessage.Document.ContentEnd,
            _StatusMessage.Document.ContentEnd);
            range.Text += message;
            range.ApplyPropertyValue(TextElement.ForegroundProperty, color);
            range.ApplyPropertyValue(TextElement.FontWeightProperty, weight);
            
            _StatusMessage.ScrollToEnd();
        }        

        private void HoleCards(object obj) 
        {
            NetPacketCards cards = (NetPacketCards)obj;
            for(int x=0;x<cards._Cards.Count;x++){
                AddHoleCard(_MySeat, cards._Cards[x]);
            }
        }

        private void CommunityCards(object obj)
        {
            NetPacketCards cards = (NetPacketCards)obj;
            for (int x = 0; x < cards._Cards.Count; x++)
            {
                AddCommunityCard(cards._Cards[x], FontWeights.Normal,11);
                _CommunityCards.Add(cards._Cards[x]);
            }

            if (cards._gamePacketID == GamePacket.eCommunityCard) return;

            string strCardType = cards._gamePacketID.ToString();
            strCardType = strCardType.Substring(1, strCardType.Length - 1);
            strCardType += " was dealt";

            AddStatusMessage("Dealer: ", Brushes.Blue,FontWeights.Normal);
            AddStatusMessage(strCardType + "\r", Brushes.Black, FontWeights.Normal);
            
        }
        private Pot GetCurrentPot()
        {
            return _GameInfo._Table._PotList[_GameInfo._Table._PotList.Count - 1];
        }

        private void BettingRound(object obj)
        {
            Pot currpot = GetCurrentPot();

            if (currpot._currRaise == 0.0||
                currpot._currRaise == _Me._fBet) //for big blind
            {
                _checkBtn.Content = "Check";
                _callBtn.Visibility = Visibility.Collapsed;
                _checkBtn.Visibility = Visibility.Visible;

                _LimitHelper.UpdateRaiseButton(_Me,_MaxBet,_MySeat, out _fCurrRaise);
                
            }
            else {
                if (currpot._currRaise >= _Me._TotalChips)
                {
                    _raiseBtn.Content = "All In " + _Me._TotalChips.ToString();
                    _raiseBtn.Visibility = Visibility.Visible;
                    _callBtn.Visibility = Visibility.Collapsed;
                    _raiseTextBox.Visibility = Visibility.Collapsed;
                    _fCurrRaise = _Me._TotalChips;                   
                }
                else
                {
                    float fNeedsToCall = currpot._currRaise - _Me._fBet;
                    _callBtn.Content = "Call " + fNeedsToCall.ToString();
                    _callBtn.Visibility = Visibility.Visible;
                    _LimitHelper.UpdateRaiseButton(_Me, _MaxBet, _MySeat,out _fCurrRaise);
                }                

                _checkBtn.Visibility = Visibility.Collapsed;                               
                
            }
            _foldBtn.Visibility = Visibility.Visible;
         
            //NOTE:hacked quick fix code!
            //if only check and fold visible
            //automatic check
            //why client?
            //enable/disable this as user settings
            /*
            if (_foldBtn.Visibility == Visibility.Visible &&
                _checkBtn.Visibility == Visibility.Visible &&
                _raiseBtn.Visibility != Visibility.Visible)
            {

                //Send check action
                _PacketSender.SendAction(PokerAction.eCheck,
                        0.0f, _GameInfo._GameID);
                ToggleButtonVisible(false);
                Thread.Sleep(100);
                return;
            }*/
        }

        //private void UpdateRaiseButton()
        //{
        //    _raiseBtn.Visibility = (IsRaisePossible()) ? Visibility.Visible : Visibility.Collapsed;
        //    _raiseTextBox.Visibility = _raiseBtn.Visibility;
        //    if (_raiseBtn.Visibility != Visibility.Visible) return;

        //    Pot currpot = GetCurrentPot();            
        //    float fRaiseVal = currpot._currRaise;

        //    if (fRaiseVal == 0.0f)
        //    {
        //        fRaiseVal = _GameInfo._Info.stake;
        //    }
        //    else {
        //        fRaiseVal *= 2.0f;
        //        if (fRaiseVal >= _Me._TotalChips)
        //        {
        //            fRaiseVal = _Me._TotalChips;
        //        }
        //    }
            
        //    //test with _Maxbet
        //    if (fRaiseVal > _MaxBet) {
        //        fRaiseVal = _MaxBet;
        //    }

        //    _raiseBtn.Content = "Raise " + fRaiseVal.ToString();
        //    _raiseTextBox.Text = fRaiseVal.ToString();
        //    _fCurrRaise = fRaiseVal;

        //}

        private float GetMaxBet()
        {
            float maxChips = 0.0f;
            Pot pot = GetCurrentPot();

            for (int ix = 0; ix < _GameInfo._Table._Seats.Count; ix++)
            {
                if (_GameInfo._Table._Seats[ix].player != null &&
                     _GameInfo._Table._Seats[ix].player._IsPlaying == true &&
                     (_GameInfo._Table._Seats[ix].player._Action != PokerAction.eFold &&
                     _GameInfo._Table._Seats[ix].player._Action != PokerAction.eAllIn) &&
                     ix != _MySeat)
                {
                    if (maxChips < _GameInfo._Table._Seats[ix].player._TotalChips)
                        maxChips = _GameInfo._Table._Seats[ix].player._TotalChips;
                }
            }

            if (maxChips < GetCurrentPot()._currRaise) {
                maxChips = GetCurrentPot()._currRaise;                
            }

            return maxChips;
        }

        //private bool IsRaisePossible()
        //{ 
        //    //count active players in the table
        //    for (int ix=0; ix < _GameInfo._Table._Seats.Count; ix++) {
        //        if (_GameInfo._Table._Seats[ix].player != null &&
        //            _GameInfo._Table._Seats[ix].player._IsPlaying == true &&
        //            (_GameInfo._Table._Seats[ix].player._Action != PokerAction.eFold &&
        //            _GameInfo._Table._Seats[ix].player._Action != PokerAction.eAllIn) &&
        //            ix != _MySeat) { return true; }
        //    }

        //    return false;                    
        //}
        
      /*  private void UpdateTable(object obj)
        {
            NetPacketHoldemTableSnapShot packet = (NetPacketHoldemTableSnapShot)obj;
            
            //save current table state
            _GameInfo._Table = packet._Table;
                    
            for (int x = 0; x < _NumSeats; x++) {
                Player p = packet._Table._Seats[x].player;
                SeatsCtrl[x]._betValue.Text = "$"+p._fBet.ToString();
                //SeatsCtrl[x]._totalChips.Text = "$" + p._TotalChips.ToString();
                SeatsCtrl[x]._totalChips.Text = "$" + (p._TotalChips - p._fBet).ToString() ;  //NOTE: CLient side UI substracts the totalchips
                SeatsCtrl[x]._btnSeat.IsEnabled = (!packet._Table._Seats[x]._bOccupied);
                
                if (packet._Table._State == TableState.eNewRound) {
                    SeatsCtrl[x].ShowCards = false;
                }


                //Set Action
                if (p._Action != PokerAction.eIdle)
                {
                    string strAction = p._Action.ToString();
                    SeatsCtrl[x]._actionValue.Text = strAction.Substring(1, strAction.Length - 1);

                    if (p._Action == PokerAction.eFold)
                        SeatsCtrl[x]._actionValue.Foreground = Brushes.Red;
                    else
                        SeatsCtrl[x]._actionValue.Foreground = Brushes.Blue;

                }
                else {
                    SeatsCtrl[x]._actionValue.Text = "";
                }

                //poker chairs
                if (packet._Table._Seats[x]._bOccupied)
                {
                    SeatsCtrl[x]._btnSeat.IsEnabled = false;
                    SeatsCtrl[x]._btnSeat.Content = 
                        ClientSessionData.GetInstance().GetClientName(p._SessionID);
                }
                else
                {
                    SeatsCtrl[x]._btnSeat.Content = "Sit Down!";
                }
                
                //if already sit down all buttons are deactivated
                if (_bSitDown) {
                    SeatsCtrl[x]._btnSeat.IsEnabled = false;
                }
                
                //Hole cards
                if (p._Action != PokerAction.eFold &&
                    p._SessionID != 0 &&
                    packet._Table._State != TableState.eWaitingPlayers)
                {
                    if (x != _MySeat && p._IsPlaying && !SeatsCtrl[x].ShowCards)                        
                    {
                        //dont show this if not show cards
                        SetTextToHole("X X", (short)x, Brushes.Blue);
                    }                    
                }
                else {
                    SetTextToHole("", (short)x, Brushes.Blue);
                }
                                
                //
                if (packet._Table._State != TableState.eWaitingPlayers &&
                    packet._Table._CurrentBettor == x &&
                    p._SessionID != 0)
                {
                    SeatsCtrl[x]._mainBorder.BorderBrush = Brushes.Green;                    
                }
                else 
                {
                    SeatsCtrl[x]._mainBorder.BorderBrush = Brushes.Black;
                }
                
                //
                //Set Pot Information                
                _potInfo.Text = "";
                for (int z = 0; z < packet._Table._PotList.Count; z++) {
                    if (z == 0)
                    {
                        _potInfo.Text += "Main Pot=$" + 
                            packet._Table._PotList[z]._amnt.ToString() + "\n";
                    }
                    else {
                        _potInfo.Text += "Side Pot" + 
                            z.ToString() + "=$" + 
                            packet._Table._PotList[z]._amnt.ToString() + "\n";
                    }
                }
      
                //Seat button informations
                SeatsCtrl[x]._buttonState.Text="";
                if (packet._Table._DealerIndex == x)
                {
                    SeatsCtrl[x]._buttonState.Text += "D ";
                }
                if (packet._Table._SBIndex == x)
                {
                    SeatsCtrl[x]._buttonState.Text += "SB ";
                }
                if (packet._Table._BBIndex == x)
                {
                    SeatsCtrl[x]._buttonState.Text += "BB ";
                }                                            
            }

            //if start of play,reset hole cards
            if (packet._Table._State == TableState.eWaitingPlayers)
            {
                //cleat community cards
                ClearCommunityCards();
                EmptyHoleCard((short)_MySeat);
            }
           
            //populate ME
            if( _MySeat != PokerLibConstants.POKER_INVALID_POSITION)
                _Me = packet._Table._Seats[_MySeat].player;            

            //get current bettor -- trigger for action
            if (packet._Table._CurrentBettor != _CurrentBettor)
            {
                if (_CurrentBettor != PokerLibConstants.POKER_INVALID_POSITION)
                {
                    string strAction = "";
                    Player playa = packet._Table._Seats[_CurrentBettor].player;
                    if (playa._Action == PokerAction.eCheck || playa._Action == PokerAction.eFold)
                    {
                        strAction = playa._Action.ToString().Substring(1, playa._Action.ToString().Length - 1);
                    }
                    else if(playa._Action != PokerAction.eIdle) {
                        strAction = playa._Action.ToString().Substring(1,playa._Action.ToString().Length-1);
                        strAction += " "+packet._Table._Seats[_CurrentBettor].player._fBet.ToString();
                    }
                    else if (playa._Action != PokerAction.eIdle && playa._fBet != 0.0f)
                    { 
                        //blinds
                        strAction = "Bets blinds ";
                        strAction += " " + packet._Table._Seats[_CurrentBettor].player._fBet.ToString();
                    }

                    if (strAction != "")
                    {
                        int sessionID = packet._Table._Seats[_CurrentBettor].player._SessionID;
                        string strMessage = string.Format("{0} {1}... ",
                            ClientSessionData.GetInstance().GetClientName(sessionID),                            
                            strAction);

                        AddStatusMessage("Dealer: ", Brushes.Blue, FontWeights.Normal);
                        AddStatusMessage(strMessage + "\r", Brushes.Black, FontWeights.Normal);
                    }

                    //fixed limit raiser count
                    if (playa._Action == PokerAction.eRaise ||
                        (playa._Action == PokerAction.eAllIn && playa._fBet > GetCurrentPot()._currRaise))
                    {
                        if (_GameInfo._Info.limit_type == GameLimitType.eFixedLimit)
                        {
                            ((FixedLimitUIHelper)_LimitHelper)._CountRaisers++;
                        }
                    }
                }
                _CurrentBettor = packet._Table._CurrentBettor;

            }

            //Update Max bet value
            _MaxBet = GetMaxBet();
            if (_MaxBet == 0.0f)
            {
                //players dont have chips they are allin
                //set to last raise value 
                _MaxBet = GetCurrentPot()._currRaise;
            }

            //save current table state
            _GameInfo._Table = packet._Table;


            //new round
            if (_GameInfo._Table._State == TableState.eNewRound)
            {
                AddStatusMessage("Dealer: ", Brushes.Blue, FontWeights.Normal);
                AddStatusMessage("Dealing next hand...\r", Brushes.Black, FontWeights.Normal);
                _CommunityCards.Clear();
                ToggleButtonVisible(false);
            }

            //state change handle
            if (_CurrentState != packet._Table._State) { 
                //set data
                _CurrentState = packet._Table._State;

                //state changed
                if(_GameInfo._Info.limit_type == GameLimitType.eFixedLimit){
                    ((FixedLimitUIHelper)_LimitHelper)._CountRaisers = 0;
                }
                
            }
        }*/


        private void EmptyHoleCard(short seatnum)
        {
            if (seatnum < 0 || seatnum >= _GameInfo._Info.max_players) return;
            TextRange range = new TextRange(SeatsCtrl[seatnum]._holeCards.Document.ContentStart,
                    SeatsCtrl[seatnum]._holeCards.Document.ContentEnd);
            range.Text = "";
        }
        private void AddHoleCard(short seatnum, Card card)
        {
            if (seatnum < 0 || seatnum >= _GameInfo._Info.max_players) return;
            TextRange range = new TextRange(SeatsCtrl[seatnum]._holeCards.Document.ContentEnd,
            SeatsCtrl[seatnum]._holeCards.Document.ContentEnd);
            range.Text = GetCardString(card) + " ";
            range.ApplyPropertyValue(TextElement.ForegroundProperty, GetCardColor(card));
            range.ApplyPropertyValue(TextElement.FontSizeProperty, 10.0);
        }
        private void ClearCommunityCards()
        {
            TextRange range = new TextRange(_communityCards.Document.ContentStart,
                _communityCards.Document.ContentEnd);
            range.Text = "";            
        }

        private void AddCommunityCard(Card card,FontWeight weight,double fontsize)
        {
            TextRange range = new TextRange(_communityCards.Document.ContentEnd,
                _communityCards.Document.ContentEnd);
            range.Text += GetCardString(card)+" ";
            range.ApplyPropertyValue(TextElement.ForegroundProperty, GetCardColor(card));
            range.ApplyPropertyValue(TextElement.FontWeightProperty, weight);
            range.ApplyPropertyValue(TextElement.FontSizeProperty, fontsize);
        }

        private Brush GetCardColor(Card card)
        {
            if (card.suite == SuiteType.Heart ||
                card.suite == SuiteType.Diamond) {
                    return Brushes.Red;
            }

            return Brushes.Black;
        }

        private string GetCardString(Card card)
        {
            string strSymbol = GetCardSymbol(card);
            string strResult="";
            if (card.face == FaceCardType.Jack) {
                strResult = "J";
            }
            else if (card.face == FaceCardType.Queen) {
                strResult = "Q";
            }
            else if (card.face == FaceCardType.King)
            {
                strResult = "K";
            }
            else if (card.face == FaceCardType.Ace)
            {
                strResult = "A";
            }
            else
            {
                strResult = ((int)card.face).ToString();
            }

            return strResult + strSymbol;

        }

        private string GetCardSymbol(Card card)
        {
            string symbol = "";
            if (card.suite == SuiteType.Diamond)
                symbol = "♦";
            else if (card.suite == SuiteType.Heart)
                symbol = "♥";
            else if (card.suite == SuiteType.Spade)
                symbol = "♠";
            else if (card.suite == SuiteType.Clubs)
                symbol = "♣";

            return symbol;
        }


        //// for coloring text in richtextbox
        private void SetTextToHole(string Text,short seatnum, Brush Color)
        {
            if (seatnum < 0 || seatnum >= _GameInfo._Info.max_players) return;
            TextRange range = new TextRange(SeatsCtrl[seatnum]._holeCards.Document.ContentStart,
                SeatsCtrl[seatnum]._holeCards.Document.ContentEnd);
            range.Text = Text;
            range.ApplyPropertyValue(TextElement.ForegroundProperty, Color);
            range.ApplyPropertyValue(TextElement.FontSizeProperty, 10.0);
        }
        
        private void SetDown(short chair)
        {
            //check if he is loggedin 
            if (!ClientSessionData.GetInstance().IsLoggedIn()) {
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
            if (log._userName.Text != "") {
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
            if(_GameInfo._Table._CurrentBettor == _MySeat) {
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

        public void Refresh(ClientInfo info)
        {
            this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                new CallBackDelegate(RefreshStatusMsg),(object) info);
                          
        }

        public void RefreshStatusMsg(object obj)
        {
            ClientInfo info = (ClientInfo)obj;
            TextRange range = new TextRange(_StatusMessage.Document.ContentStart,
            _StatusMessage.Document.ContentEnd);
            string strMessage = range.Text;
            if (range.Text != "")
            {
                range.Text = strMessage.Replace("[ID#" + info.ID.ToString() + "]", info.UserName); ;
            }
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

        public void HighlightWinnerCards(NetPacketWinner winner)
        {
            List<Card> win = CardsHelper.GetWinningCards(winner, _CommunityCards);
            ClearCommunityCards();
            for (int x = 0; x < _CommunityCards.Count; x++)
            {
                if(CardsHelper.IsCardInList(win,_CommunityCards[x]))
                    AddCommunityCard(_CommunityCards[x], FontWeights.Bold,17);               
                else
                    AddCommunityCard(_CommunityCards[x], FontWeights.Normal,11);               
            }
        }

        private int GetPlayerSeat(int playerID)
        {
            for (int x = 0; x < _GameInfo._Table._Seats.Count; x++)
            {
                if (!_GameInfo._Table._Seats[x]._bOccupied) continue;
                if (playerID == _GameInfo._Table._Seats[x].player._SessionID)
                {
                    return x;
                }
            }
            return PokerLibConstants.POKER_INVALID_POSITION;
        }

        

        #region Seven card stud specialize area
        bool _RaiseUpChance = false;
        float _raiseUp = 0.0f;

        private void OpenCards(object obj)
        {
            NetPacketOpenCards packet = (NetPacketOpenCards)obj;
            for (int x = 0; x < packet._ownedList.Count; x++)
            {
                CardOwn own = packet._ownedList[x];
                int seat = GetPlayerSeat(own.playerID);
                if (seat == PokerLibConstants.POKER_INVALID_POSITION) continue;

                if (packet._gamePacketID == GamePacket.eOpenCards ||
                    packet._gamePacketID == GamePacket.e3rdStreet && 
                    seat != _MySeat)
                {      
                    //this is to broadcast community during session add
                    EmptyHoleCard((short)seat);
                    SetTextToHole("X X ", (short)seat, Brushes.Blue);
                }

                //exclude 3rd street since user already recieved hole cards
                if (packet._gamePacketID == GamePacket.e3rdStreet && seat == _MySeat) continue;

                for(int c=0;c <own.hands.Count;c++)
                    AddHoleCard((short)seat, own.hands[c]);
            }
        }

        private void RaiseUpChance(object obj)
        {
            NetPacketRoomInt packet = (NetPacketRoomInt)obj;
            _RaiseUpChance = System.Convert.ToBoolean(packet._Param);
        
        }

        //TODO betting round must be unified and handled by UI handler
        private void SevenCardBettingRound(object obj)
        {
            Pot currpot = GetCurrentPot();
            //GameInfoSevenStud studInfo = (GameInfoSevenStud)_GameInfo._Info;

            ToggleButtonVisible(false);
            _foldBtn.Visibility = Visibility.Visible;

            if (currpot._currRaise == 0.0)
            {
                //bring in
                if (_GameInfo._Table._State == TableState.eHoleState)
                {
                    _raiseBtn.Content = "Bring-in " + _GameInfo._Info.fBringIn.ToString();
                    _raiseUpBtn.Content = "Bet " + (_GameInfo._Info.stake / 2.0f).ToString();
                    _raiseBtn.Visibility = Visibility.Visible;
                    _raiseUpBtn.Visibility = Visibility.Visible;
                    _raiseUp = _GameInfo._Info.stake / 2.0f;
                    _fCurrRaise = _GameInfo._Info.fBringIn;
                    _foldBtn.Visibility = Visibility.Collapsed;
                }
                else {
                    _checkBtn.Content = "Check";
                    _callBtn.Visibility = Visibility.Collapsed;
                    _checkBtn.Visibility = Visibility.Visible;

                    _LimitHelper.UpdateRaiseButton(_Me, _MaxBet, _MySeat, out _fCurrRaise);
                    _foldBtn.Visibility = Visibility.Visible;
                }
            }
            else {
                
                if (currpot._currRaise >= _Me._TotalChips)
                {
                    _raiseBtn.Content = "All In " + _Me._TotalChips.ToString();
                    _raiseBtn.Visibility = Visibility.Visible;
                    _callBtn.Visibility = Visibility.Collapsed;
                    _raiseTextBox.Visibility = Visibility.Collapsed;
                    _fCurrRaise = _Me._TotalChips;
                }
                else {
                    float fNeedsToCall = currpot._currRaise - _Me._fBet;
                    _callBtn.Content = "Call " + fNeedsToCall.ToString();
                    _callBtn.Visibility = Visibility.Visible;
                    _LimitHelper.UpdateRaiseButton(_Me, _MaxBet, _MySeat, out _fCurrRaise);


                    //if (_RaiseUpChance == RaiseFlag.eClaimed && 
                    //    _GameInfo._Info.game_type == GameType.eSevenCardStud)
                    //{
                    //    if (_raiseBtn.Visibility == Visibility.Visible)
                    //    {
                    //        _fCurrRaise = currpot._currRaise + _GameInfo._Info.stake;
                    //        raiseBtn.Content = "Raise " + _fCurrRaise.ToString();
                    //    }
                    //}
                }
                _checkBtn.Visibility = Visibility.Collapsed;
            }

            if (_raiseBtn.Visibility == Visibility.Visible && 
                _fCurrRaise != _Me._TotalChips &&
                PokerLibGlobal.IsSevenCardStudGame(_GameInfo._Info.game_type)
               /*_GameInfo._Info.game_type == GameType.eSevenCardStud*/)
            {
                if (_RaiseUpChance== true)
                {
                    _raiseUpBtn.Visibility = Visibility.Visible;
                    _raiseUpBtn.Content = "Raise " + (currpot._currRaise+_GameInfo._Info.stake).ToString();
                    _raiseUp = (currpot._currRaise + _GameInfo._Info.stake);
                }
            }

            
        }

        void UpdateTablePoker(object obj)
        {
            NetPacketPokerTableSnapShot packet = (NetPacketPokerTableSnapShot)obj;

            //save current table state
            _GameInfo._Table = packet._Table;

            for (int x = 0; x < _NumSeats; x++)
            {
                Player p = packet._Table._Seats[x].player;
                SeatsCtrl[x]._betValue.Text = "$" + p._fBet.ToString();
                //SeatsCtrl[x]._totalChips.Text = "$" + p._TotalChips.ToString();
                SeatsCtrl[x]._totalChips.Text = "$" + (p._TotalChips - p._fBet).ToString();  //NOTE: CLient side UI substracts the totalchips
                SeatsCtrl[x]._btnSeat.IsEnabled = (!packet._Table._Seats[x]._bOccupied);

                if (packet._Table._State == TableState.eNewRound)
                {
                    SeatsCtrl[x].ShowCards = false;
                }


                //Set Action
                if (p._Action != PokerAction.eIdle)
                {
                    string strAction = p._Action.ToString();
                    SeatsCtrl[x]._actionValue.Text = strAction.Substring(1, strAction.Length - 1);

                    if (p._Action == PokerAction.eFold)
                        SeatsCtrl[x]._actionValue.Foreground = Brushes.Red;
                    else
                        SeatsCtrl[x]._actionValue.Foreground = Brushes.Blue;

                }
                else
                {
                    SeatsCtrl[x]._actionValue.Text = "";
                }

                //poker chairs
                if (packet._Table._Seats[x]._bOccupied)
                {
                    SeatsCtrl[x]._btnSeat.IsEnabled = false;
                    SeatsCtrl[x]._btnSeat.Content =
                        ClientSessionData.GetInstance().GetClientName(p._SessionID);
                }
                else
                {
                    SeatsCtrl[x]._btnSeat.Content = "Sit Down!";
                }

                //if already sit down all buttons are deactivated
                if (_bSitDown)
                {
                    SeatsCtrl[x]._btnSeat.IsEnabled = false;
                }

                //Hole cards
                //if (p._Action != PokerAction.eFold &&
                //    p._SessionID != 0 &&
                //    packet._Table._State != TableState.eWaitingPlayers)
                //{
                //    //if (x != _MySeat && p._IsPlaying && !SeatsCtrl[x].ShowCards) //deal hole dapat? 
                //    //{
                //    //    //dont show this if not show cards
                //    //    SetTextToHole("X X", (short)x, Brushes.Blue);
                //    //}
                //}
                if(p._Action == PokerAction.eFold)
                {
                    SetTextToHole("", (short)x, Brushes.Blue);
                }

                //
                if (packet._Table._State != TableState.eWaitingPlayers &&
                    packet._Table._CurrentBettor == x &&
                    p._SessionID != 0)
                {
                    SeatsCtrl[x]._mainBorder.BorderBrush = Brushes.Green;
                }
                else
                {
                    SeatsCtrl[x]._mainBorder.BorderBrush = Brushes.Black;
                }

                //
                //Set Pot Information                
                _potInfo.Text = "";
                for (int z = 0; z < packet._Table._PotList.Count; z++)
                {
                    if (z == 0)
                    {
                        _potInfo.Text += "Main Pot=$" +
                            packet._Table._PotList[z]._amnt.ToString() + "\n";
                    }
                    else
                    {
                        _potInfo.Text += "Side Pot" +
                            z.ToString() + "=$" +
                            packet._Table._PotList[z]._amnt.ToString() + "\n";
                    }
                }

                SeatsCtrl[x]._buttonState.Text = "";
                if (packet._Table._DealerIndex == x &&
                    PokerLibGlobal.IsHoldemGame(_GameInfo._Info.game_type))
                {
                    SeatsCtrl[x]._buttonState.Text += "D ";
                }
            }

            //if start of play,reset hole cards
            if (packet._Table._State == TableState.eWaitingPlayers)
            {
                //cleat community cards
                ClearCommunityCards();
                EmptyHoleCard((short)_MySeat);

                //clear flags
                for (int x = 0; x < _NumSeats; x++)
                {
                    SeatsCtrl[x].HoleCardsDealt = false;
                    SeatsCtrl[x].LastHoleCardsDealt = false;
                }
            }

            //populate ME
            if (_MySeat != PokerLibConstants.POKER_INVALID_POSITION)
                _Me = packet._Table._Seats[_MySeat].player;

            //get current bettor -- trigger for action
            if (packet._Table._CurrentBettor != _CurrentBettor)
            {
                if (_CurrentBettor != PokerLibConstants.POKER_INVALID_POSITION)
                {
                    string strAction = "";
                    Player playa = packet._Table._Seats[_CurrentBettor].player;
                    if (playa._Action == PokerAction.eCheck || playa._Action == PokerAction.eFold)
                    {
                        strAction = playa._Action.ToString().Substring(1, playa._Action.ToString().Length - 1);
                    }
                    else if (playa._Action != PokerAction.eIdle)
                    {
                        strAction = playa._Action.ToString().Substring(1, playa._Action.ToString().Length - 1);
                        strAction += " " + packet._Table._Seats[_CurrentBettor].player._fBet.ToString();
                    }
                    else if (playa._Action != PokerAction.eIdle && playa._fBet != 0.0f)
                    {
                        //blinds
                        strAction = "Bets blinds ";
                        strAction += " " + packet._Table._Seats[_CurrentBettor].player._fBet.ToString();
                    }

                    if (strAction != "")
                    {
                        int sessionID = packet._Table._Seats[_CurrentBettor].player._SessionID;
                        string strMessage = string.Format("{0} {1}... ",
                            ClientSessionData.GetInstance().GetClientName(sessionID),
                            strAction);

                        AddStatusMessage("Dealer: ", Brushes.Blue, FontWeights.Normal);
                        AddStatusMessage(strMessage + "\r", Brushes.Black, FontWeights.Normal);
                    }

                    //fixed limit raiser count
                    if (playa._Action == PokerAction.eRaise ||
                        (playa._Action == PokerAction.eAllIn && playa._fBet > GetCurrentPot()._currRaise))
                    {
                        if (_GameInfo._Info.limit_type == GameLimitType.eFixedLimit)
                        {
                            ((FixedLimitUIHelper)_LimitHelper)._CountRaisers++;
                        }
                    }
                }
                _CurrentBettor = packet._Table._CurrentBettor;

            }

            //Update Max bet value
            _MaxBet = GetMaxBet();
            if (_MaxBet == 0.0f)
            {
                //players dont have chips they are allin
                //set to last raise value 
                _MaxBet = GetCurrentPot()._currRaise;
            }

            //If pot limit
            if (_GameInfo._Info.limit_type == GameLimitType.ePotLimit)
            {
                
                float potLimit = ((PotLimitUIHelper)_LimitHelper).GetPotLimitRaise(_Me);
                if (_MaxBet > potLimit)
                    _MaxBet = potLimit;
            }

            //save current table state
            _GameInfo._Table = packet._Table;


            //new round
            if (_GameInfo._Table._State == TableState.eNewRound)
            {
                _ShowHand.Visibility = Visibility.Hidden;
                _Muck.Visibility = Visibility.Hidden;
                //_textLose.Visibility = Visibility.Hidden;

                if (_GameInfo._Info.stake != _fNextStake)
                {
                    _GameInfo._Info.stake = _fNextStake;
                    this.Title = _GameInfo._Info.strDescription + " " + _GameInfo.Stake;                 
                }

                AddStatusMessage("Dealer: ", Brushes.Blue, FontWeights.Normal);
                AddStatusMessage("Dealing next hand...\r", Brushes.Black, FontWeights.Normal);
                _CommunityCards.Clear();
                ToggleButtonVisible(false);

                //deal cards to playing players on next round
                for (int x = 0; x < _NumSeats; x++)
                {
                    Player p = packet._Table._Seats[x].player;
                    if (p._Action != PokerAction.eFold &&
                        p._SessionID != 0 &&
                        packet._Table._State != TableState.eWaitingPlayers)
                    {
                        if (x != _MySeat && p._IsPlaying && !SeatsCtrl[x].ShowCards) //deal hole dapat? 
                        {
                            //dont show this if not show cards
                            if (SeatsCtrl[x].HoleCardsDealt != true)
                            {
                                //2 close cards for texas holdem and 7 card stud, 4 for omaha
                                if( _GameInfo._Info.game_type == GameType.eTexasHoldem ||
                                    PokerLibGlobal.IsSevenCardStudGame(_GameInfo._Info.game_type))
                                    SetTextToHole("X X ", (short)x, Brushes.Blue);
                                else if(PokerLibGlobal.IsOmahaGame(_GameInfo._Info.game_type))
                                    SetTextToHole("X X X X ", (short)x, Brushes.Blue);

                                SeatsCtrl[x].HoleCardsDealt = true;
                            }                           
                        }
                    }
                }
            }
            else if (_GameInfo._Table._State == TableState.e7thStreetState)
            {
                for (int x = 0; x < _NumSeats; x++)
                {
                    Player p = packet._Table._Seats[x].player;
                    if (p._Action != PokerAction.eFold &&
                        p._SessionID != 0 &&
                        packet._Table._State != TableState.eWaitingPlayers)
                    {
                        if (x != _MySeat && p._IsPlaying && !SeatsCtrl[x].ShowCards) //deal hole dapat? 
                        {
                            //dont show this if not show cards
                            if (SeatsCtrl[x].LastHoleCardsDealt != true)
                            {
                                AddTextToHole("X", (short)x, Brushes.Blue);
                                SeatsCtrl[x].LastHoleCardsDealt = true;
                            }
                        }
                    }
                }
            }

            //state change handle
            if (_CurrentState != packet._Table._State)
            {
                //set data
                _CurrentState = packet._Table._State;

                //state changed
                if (_GameInfo._Info.limit_type == GameLimitType.eFixedLimit)
                {
                    ((FixedLimitUIHelper)_LimitHelper)._CountRaisers = 0;
                }

            }
        }

        private void _raiseUpBtn_Click(object sender, RoutedEventArgs e)
        {
            if (_raiseUp == _Me._TotalChips)
            {
                _PacketSender.SendAction(PokerAction.eAllIn,
                    _Me._TotalChips, _GameInfo._GameID);

            }
            else
            {
                _PacketSender.SendAction(PokerAction.eRaise,
                    _raiseUp, _GameInfo._GameID);
            }

            ToggleButtonVisible(false);
        }

        private void AddTextToHole(string Text, short seatnum, Brush Color)
        {
            if (seatnum < 0 || seatnum >= _GameInfo._Info.max_players) return;
            TextRange range = new TextRange(SeatsCtrl[seatnum]._holeCards.Document.ContentEnd,
                SeatsCtrl[seatnum]._holeCards.Document.ContentEnd);
            range.Text = Text;
            range.ApplyPropertyValue(TextElement.ForegroundProperty, Color);
        }

        private bool IsBringInRound()
        {
            if( PokerLibGlobal.IsSevenCardStudGame(_GameInfo._Info.game_type) )
                return ((GetCurrentPot()._currRaise == 0.0) &&
                                (_GameInfo._Table._State == TableState.eHoleState));
            return false;
        }

        #endregion

   
    }
}
