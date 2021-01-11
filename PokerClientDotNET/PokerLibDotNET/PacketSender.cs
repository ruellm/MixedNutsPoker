using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace PokerLibDotNET
{
    public class PacketSender
    {
        private Socket _Server;

        public PacketSender(Socket server)
        {
            _Server = server;
        }

        public void Send(INetPacket packet)
        {
            if (packet == null) return;
            string input = packet.ToString();    //Create room
            byte[] data = Encoding.ASCII.GetBytes(input);
            try
            {
                _Server.Send(data);
            }
            catch { 
                //...
            }
        }

        public void SendReqPokerGameList(GameType type, GameModeType mode, GameLimitType limit)
        {
            NetPacketReqRoomPokerList info = 
                new NetPacketReqRoomPokerList();
            info._gameLimit = limit;
            info._gameType = type;
            info._gameMode = mode;            
            Send(info);
        }
        public void SendReqSpectateGame(int gameID) {
            NetPacketInt packet = 
                new NetPacketInt(PacketIDType.eGameSpectate);
            packet._nParam = gameID;
            Send(packet);
        }

        public void SendJoinGame(int gameID, short seatno, float buyin)
        {
            NetPacketJoinGame join = 
                new NetPacketJoinGame();
            join._BuyIn = buyin;
            join._nRoomID = gameID;
            join._SeatNum = seatno;
            Send(join);
        }

        public void SendStandUpRoom(int gameID)
        {
            NetPacketInt packet = 
                new NetPacketInt(PacketIDType.eStandUp);
            packet._nParam = gameID;
            Send(packet);
        }

        public void SendLeaveRoom(int gameID)
        {
            NetPacketInt packet = 
                new NetPacketInt(PacketIDType.eLeaveRoom);
            packet._nParam = gameID;
            Send(packet);
        }

        public void SendAction(PokerAction action, float amnt,int gameID)
        {
            NetPacketAction packet = new NetPacketAction();
            packet._Action = action;
            packet._fAmnt = amnt;
            packet._nRoomID = gameID;
            Send(packet);
        }

        public void SendReqLogin(string username, string password)
        {
            NetPacketReqLogIn packet = new NetPacketReqLogIn();
            packet._UserName = username;
            packet._Password = password;
            Send(packet);
        }

        public void SendReqPlayerInfo(int pID)
        {
            NetPacketInt packet =
                new NetPacketInt(PacketIDType.eGetClientInfo);
            packet._nParam = pID;
            Send(packet);
        }

        public void SendChat(string strMsg,int roomID)
        {
            NetPacketChatMsg packet = new NetPacketChatMsg();
            packet._Message = strMsg;
            packet._nRoomID = roomID;
            packet._PlayerID = PokerLibConstants.CLIENT_ID_INVALID;
            Send(packet);
        }

        public void SendReqPlayerList(int gameID)
        {
            NetPacketInt packet =
                new NetPacketInt(PacketIDType.eReqGamePlayerList);
            packet._nParam = gameID;
            Send(packet);
        }

        public void SendReqRoomParam(GamePacket packetID, int param, int roomID)
        {
            NetPacketRoomInt packet = new NetPacketRoomInt(packetID, param);
            packet._nRoomID = roomID;
            Send(packet);
        }
    }
}
