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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Collections;
using PokerLibDotNET;

namespace PokerClientDotNET
{

    //temporary class
    public class PlayerListInfo
    {
        public string PlayerName
        {   get;
            set;
        }

        public string TotalChips
        {
            get;
            set;
        }
    }
    
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        private ObservableCollection<GameInformation> _gameList;
        private ObservableCollection<PlayerListInfo> _PlayerList;
        private IPEndPoint _Ip;
        private Socket _Server;
        //private string strServerAddress = "127.0.0.1";
        private string strServerAddress = "10.191.17.45";
        private int iPort = 1983;
        private bool bThreadAlive = true;
        private PacketSender _Sender;
        private System.Timers.Timer _Refresh;
        private delegate void CallBackDelegate(object obj);
        private Mutex _SocketMutex;
        private Hashtable _gameWindowList;
        private Splash _splashWindow;
        private delegate void FunctionDelegate();

        public Window1()
        {
            InitializeComponent();

            _gameList = new ObservableCollection<GameInformation>();
            _PlayerList = new ObservableCollection<PlayerListInfo>();
            _SocketMutex = new Mutex();
            _gameWindowList = new Hashtable();
            _splashWindow = new Splash();                        
        }
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
            this.Loaded += new RoutedEventHandler(Window1_Loaded);
            this.Closing += new System.ComponentModel.CancelEventHandler(Window1_Closing);
        }

        void Window1_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            try
            {
                if (_gameWindowList.Count > 0) {
                    MessageBoxResult result = MessageBox.Show("Open game room will automatically closed, "+
                        "\r Open Hands will automatically folded, Continue?","Confirmation",MessageBoxButton.YesNo); 
                    if (result == MessageBoxResult.No) { 
                        //cancel exit
                        e.Cancel = true;
                        return;
                    }
                }

                foreach (DictionaryEntry entry in _gameWindowList)
                {
                    Window win = (Window)(entry.Value);
                    win.Close();
                }

                _Refresh.Enabled = false;

                bThreadAlive = false;

                _SocketMutex.WaitOne();
                _Server.Shutdown(SocketShutdown.Both);
                _Server.Close();
                _SocketMutex.ReleaseMutex();
            }
            catch
            { }
        }

        void Window1_Loaded(object sender, RoutedEventArgs e)
        {
            ListView1.ItemsSource = _gameList;                   
            new Thread(new ThreadStart(tryconnect)).Start();
            _splashWindow.Owner = this;
            _splashWindow.ShowDialog();
            _playerList.ItemsSource = _PlayerList;
            ListView1.SelectionChanged += new SelectionChangedEventHandler(_playerList_SelectionChanged);
         }

        void _playerList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (ListView1.SelectedIndex < 0 || ListView1.SelectedIndex >= ListView1.Items.Count) return;
            _Sender.SendReqPlayerList(_gameList[ListView1.SelectedIndex]._GameID);
        }

        public void RefreshList(ClientInfo info)
        {
            this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                          new CallBackDelegate(UpdateList), info);   
        }
        void UpdateList(object obj)
        {
            ClientInfo info = (ClientInfo)obj;
            for (int x = 0; x < _PlayerList.Count; x++)
            {
                string tempID = "[ID#" + info.ID.ToString() + "]";
                if (tempID == _PlayerList[x].PlayerName)
                {
                    //_PlayerList[x].PlayerName =
                      //   _PlayerList[x].PlayerName.Replace("[ID#" + info.ID.ToString() + "]", info.UserName);                    
                    //forced refresh
                    PlayerListInfo player = _PlayerList[x];
                    player.PlayerName =
                        player.PlayerName.Replace("[ID#" + info.ID.ToString() + "]", info.UserName);

                    //_PlayerList[x] = player;
                    _PlayerList.Remove(_PlayerList[x]);
                    _PlayerList.Add(player);
                }
            }
            //_playerList.ItemsSource = _PlayerList;
            _playerList.UpdateLayout();
        }
        
        void Item_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            HandleEnterHoldem();
        }

        void tryconnect()
        {
            _Ip = new IPEndPoint(IPAddress.Parse(strServerAddress), iPort);
            _Server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            try
            {
                _Server.Connect(_Ip);
                this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                new FunctionDelegate(Connect));
            }
            catch
            {
                System.Timers.Timer reconect = new System.Timers.Timer();
                reconect.Interval = 5000;
                reconect.Enabled = true;
                reconect.Elapsed += new System.Timers.ElapsedEventHandler(reconect_Elapsed);
                return;
            }
        }        

        private void Connect(){
           
            _splashWindow.Close();
            _Sender = new PacketSender(_Server);
            _Refresh = new System.Timers.Timer();
            _Refresh.Interval = 5000;
            _Refresh.Enabled = true;
            _Refresh.Elapsed += new System.Timers.ElapsedEventHandler(_Refresh_Elapsed);

            ClientSessionData.GetInstance()._ParentWindow = this;
            ClientSessionData.GetInstance()._PacketSender = _Sender;
            ClientSessionData.GetInstance().pfnRefresh += new RefreshDelegate(RefreshList);  

            //launch thread
            new Thread(new ParameterizedThreadStart(ConnectionThread)).Start(_Server);
        }

        void reconect_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            tryconnect();
        }

        void _Refresh_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
             _SocketMutex.WaitOne();
            
            ////send request for no limit
            _Sender.SendReqPokerGameList(GameType.eTexasHoldem, 
                 GameModeType.eModeRingGame, GameLimitType.eNoLimit);
            
            ////send request for fixed limit
            _Sender.SendReqPokerGameList(GameType.eTexasHoldem,
                GameModeType.eModeRingGame, GameLimitType.eFixedLimit);

            //send request for 7 card stud poker
            _Sender.SendReqPokerGameList(GameType.eSevenCardStud,
                GameModeType.eModeRingGame, GameLimitType.eFixedLimit);

            //send request for omaha holdem
            _Sender.SendReqPokerGameList(GameType.eOmahaHoldem,
                GameModeType.eModeRingGame, GameLimitType.ePotLimit);

            //send request for texas holdem SNG
            _Sender.SendReqPokerGameList(GameType.eTexasHoldem,
                GameModeType.eModeSNG, GameLimitType.eNoLimit);

            //send request for Stud HiLo fixed limit
            _Sender.SendReqPokerGameList(GameType.eSevenCardStudHiLo,
                GameModeType.eModeRingGame, GameLimitType.eFixedLimit);

            //send request for omaha hilo
            _Sender.SendReqPokerGameList(GameType.eOmahaHiLo,
                GameModeType.eModeRingGame, GameLimitType.ePotLimit);

            //send request for omaha hilo
            _Sender.SendReqPokerGameList(GameType.eRazz,
                GameModeType.eModeRingGame, GameLimitType.eFixedLimit);

            _SocketMutex.ReleaseMutex();

            //_Refresh.Enabled = false;

            //if (ListView1.SelectedIndex < 0 || ListView1.SelectedIndex >= ListView1.Items.Count) return;
            //_Sender.SendReqPlayerList(_gameList[ListView1.SelectedIndex]._GameID);
        }

        public void ConnectionThread(object obj)
        {
            Socket socket = (Socket)obj;
            PacketParser parser = new PacketParser();
            List<INetPacket> packetList = new List<INetPacket>();
            string strIncompleteData="";

            while (bThreadAlive) {
                byte[] recvbuf = new byte[PokerLibConstants.MAX_BUFFER_SIZE];
                int len = 0;
                try
                {
                    len = socket.Receive(recvbuf);
                }catch { }

                System.Text.Encoding enc = System.Text.Encoding.ASCII;
                string strData = enc.GetString(recvbuf);
                strData = strData.Substring(0, len);
                parser.Parse(strData,ref strIncompleteData,ref packetList);

                foreach (INetPacket packet in packetList)
                {
                    if (packet == null) return;
                    switch (packet.GetID())
                    {
                        case PacketIDType.eGameListItem:
                            this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                 new CallBackDelegate(HandleGameItemRequest), packet);
                            break;
                        case PacketIDType.eRoomSpecific:
                            NetPacketRoom packetRoom = (NetPacketRoom)packet;
                            //Return if window already exist
                            if (_gameWindowList.ContainsKey(packetRoom._nRoomID))
                                ((HoldemWindow)_gameWindowList[packetRoom._nRoomID]).HandlePacket(packetRoom);
                            break;
                        case PacketIDType.eRegLogInFeedback:
                            ClientSessionData.GetInstance().HandleReqLogPacket((NetPacketInt)packet);
                            break;
                        case PacketIDType.eClientInfoFeedBack:
                            ClientSessionData.GetInstance().HandleAddClientInfo((NetPacketClientInfoFeedBack)packet);
                            break;
                        case PacketIDType.ePlayerList:
                            this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                new CallBackDelegate(HandlePlayerList), packet);
                            break;

                    }                    
                }
                packetList.Clear();
            }

        }

        public void HandlePlayerList(object obj)
        {
            NetPacketPlayerList packet = (NetPacketPlayerList)obj;
            if (ListView1.SelectedIndex < 0 || ListView1.SelectedIndex >= ListView1.Items.Count) return;
            if (_gameList[ListView1.SelectedIndex]._GameID == packet._roomID)
            {
                _PlayerList.Clear();
                for (int x = 0; x < packet._playerList.Count; x++) 
                {
                    PlayerListInfo info = new PlayerListInfo();
                    info.PlayerName = ClientSessionData.GetInstance().GetClientName(packet._playerList[x]._SessionID);
                    info.TotalChips = packet._playerList[x]._TotalChips.ToString();
                    _PlayerList.Add(info);
                }

            }                
        }

        public void HandleGameItemRequest(object obj)
        {
            //TODO: handle casting of game list item based on what is active on listview
            //for this sample client there is only 1 listview (for texas holdem poker only)
            NetPacketPokerGameListItem packet = (NetPacketPokerGameListItem)obj;
            //search if game list exist in the collection
            for (int i = 0; i < _gameList.Count; i++) {
                if (_gameList[i]._GameID == packet._gameID)
                    return;
            }

            GameInformation gameInfo = new GameInformation();
            gameInfo._Info = new GameInfoPoker();

            //if (packet._type == GameType.eSevenCardStud)
            //{
            //    gameInfo._Info = new GameInfoSevenStud();
            //    NetPacketSevenCardStudGameListItem packet7 = (NetPacketSevenCardStudGameListItem)packet;
            //    ((GameInfoSevenStud)gameInfo._Info).fAntes = packet7._fAntes;
            //    ((GameInfoSevenStud)gameInfo._Info).fBringIn = packet7._fBringIn;
            //}
            //else
            //    gameInfo._Info = new GameInfoPoker();

            gameInfo._GameID = packet._gameID;
            gameInfo._Info.game_type = packet._type;
            gameInfo._Info.strDescription = packet._strGameDesc;
            gameInfo._Info.stake = packet._fStake;
            gameInfo._Info.max_players = packet._nMaxPlayers;
            gameInfo._NumPlayers = packet._nPlayers;
            gameInfo._Info.limit_type = packet._gameLimit;
            gameInfo._Info.fAntes = packet._fAntes;
            gameInfo._Info.fBringIn = packet._fBringIn;
            gameInfo._Info.blindsInfo.raise_time = packet._RaiseTime;
            gameInfo._Info.blindsInfo.raise_factor = packet._RaiseFactor;

            _gameList.Add(gameInfo);
        }

        private void HandleEnterHoldem()
        {
            HoldemWindow window = new HoldemWindow();
            if (ListView1.SelectedIndex < 0 || ListView1.SelectedIndex >= ListView1.Items.Count) return;

            window._GameInfo = _gameList[ListView1.SelectedIndex];
            window._PacketSender = _Sender;

            //Return if window already exist
            if (_gameWindowList.ContainsKey(_gameList[ListView1.SelectedIndex]._GameID))
                return;

            _gameWindowList.Add(window._GameInfo._GameID, window);
            window.Show();
            window.Activate();
            window.Closing += new System.ComponentModel.CancelEventHandler(window_Closing);
            ClientSessionData.GetInstance().pfnRefresh += new RefreshDelegate(window.Refresh);
        }

        void window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _gameWindowList.Remove(((HoldemWindow)sender)._GameInfo._GameID);
            
        }
        
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            HandleEnterHoldem();
        }

        private void MenuItem_LogIn_Click(object sender, RoutedEventArgs e)
        {
            string strLogin = logedin.Header.ToString();
            if (strLogin == "Login")
            {
                if (ClientSessionData.GetInstance().IsLoggedIn()) {
                    logedin.IsEnabled = false;
                    return;
                }
                LogIn log = new LogIn();
                log.Owner = this;
                log.ShowDialog();
                if (log._userName.Text != "")
                {
                    ClientSessionData.GetInstance().DoLogIn(log._userName.Text,
                        new NextFuncDelegate(LoggedInDelegate), null);
                }
            }
            else { 
                //TODO:
                //DoLogout
            }
        }

        private void LoggedInDelegate(StatusType error, object parm)
        {
            if (error != StatusType.Success)
            {
                MessageBox.Show("Error Logging in to server \r Name maybe exist!");
                return;
            }
            logedin.IsEnabled = false;
        }

        private void MenuItem_Close_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
