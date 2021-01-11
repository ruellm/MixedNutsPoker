using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using PokerLibDotNET;
using System.Windows;

namespace PokerClientDotNET
{
    public delegate void NextFuncDelegate(StatusType error, object parm);
    public delegate void RefreshDelegate(ClientInfo info);

    public class ClientSessionData
    {
        public static ClientSessionData _pInstance=null;
        public string UserName="";
        private string _userNameTemp = "";
        private NextFuncDelegate _pfnNext;
        private object _LoginParam;

        private Hashtable _ClientList;
        public PacketSender _PacketSender;
        public Window _ParentWindow;
        public RefreshDelegate pfnRefresh;
        public float _MyBankRoll = 5000.0f;
        
        private ClientSessionData()
        {
            _ClientList = new Hashtable();
        }

        public static ClientSessionData GetInstance()
        {
            if (_pInstance == null)
                _pInstance = new ClientSessionData();
            return _pInstance;
        }

        public bool IsLoggedIn()
        {
            return UserName != "";
        }
        
        public string GetClientName(int cliID)
        {
            if (_ClientList.ContainsKey(cliID))
                return ((ClientInfo)_ClientList[cliID]).UserName;

            if (_PacketSender == null) return "";
            _PacketSender.SendReqPlayerInfo(cliID);

            return "[ID#" + cliID.ToString() + "]";
        }

        public void AddClientInfo(ClientInfo info)
        {
            if (_ClientList.ContainsKey(info.ID)) return;
            _ClientList.Add(info.ID, info);
        }

        public void DoLogIn(string name, NextFuncDelegate next,object param)
        {
            _pfnNext = next;
            _LoginParam = param;
            _userNameTemp = name;
            _PacketSender.SendReqLogin(name, "");
        }

        public void DoLogout()
        { 
            
        }

        public void HandleReqLogPacket(NetPacketInt packet)
        {
            StatusType type = (StatusType)packet._nParam;
            if (type == StatusType.Success) {
                UserName = _userNameTemp;
                _userNameTemp = "";
            }
            _ParentWindow.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Render,
                                _pfnNext, type,_LoginParam);
        }

        public void HandleAddClientInfo(NetPacketClientInfoFeedBack packet)
        {
            ClientInfo info = new ClientInfo();
            info.ID = packet._PlayerID;
            info.UserName = packet._UserName;
            info.Location = packet._Location;
            AddClientInfo(info);

            if (pfnRefresh != null) {
                pfnRefresh(info);
            }
        }
    }
}
