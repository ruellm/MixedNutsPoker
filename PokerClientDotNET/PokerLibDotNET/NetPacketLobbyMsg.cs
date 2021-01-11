using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
    /*
     * Request game room list base
    */
    public class NetPacketReqRoomList
        : INetPacket
    {
        public GameType _gameType;
        public NetPacketReqRoomList()
        {
            m_PacketId = PacketIDType.eReqGameList;
        }
        public override string ToString()
        {
            //packet_id(space)gametype;|
            string strRawPacket =
                string.Format("{0}*{1};|",
                    (int)m_PacketId,
                    (int)_gameType);
            return strRawPacket;
        }
    }
    
    /*
    * Request game room POKER list base
    */
    public class NetPacketReqRoomPokerList 
        : INetPacket
    {
        public GameModeType _gameMode;
        public GameLimitType _gameLimit;
        public GameType _gameType;
        public NetPacketReqRoomPokerList(){
            m_PacketId = PacketIDType.eReqPokerGameList;
        }
        
        public override string ToString()
        {
            //packet_id(space)gametype;gamemode;gamelimit;|
            string strRawPacket =
                string.Format("{0}*{1};{2};{3};|",
                    (int)m_PacketId,
                    (int)_gameType,
                    (int)_gameMode,
                    (int)_gameLimit);
            return strRawPacket;
        }
    }

    /*
    * Create Game
    */
    public class NetPacketCreateRoom
        : INetPacket
    {
        public GameInfo _gameInfo;
        public NetPacketCreateRoom() {
            m_PacketId = PacketIDType.eCreateGame;
        }

        public override string ToString()
        {
            //packet_id(space)game_type;stake;maxplayers;action_time_limit;mode;limit;raise_time;raise_factor|
            string strRawPacket =
                string.Format("{0}*{1};{2};{3};{4};{5};|",
                    (int)m_PacketId,
                    (int)_gameInfo.game_type,
                    _gameInfo.stake,
                    _gameInfo.max_players,
                    _gameInfo.action_time_imit,
                    _gameInfo.strDescription);
                    
            return strRawPacket;
        }
    }

    /*
    * Create Game - POKER base
    */
    public class NetPacketCreateRoomPoker : NetPacketCreateRoom
    {
        public NetPacketCreateRoomPoker(){
            m_PacketId = PacketIDType.eCreatePokerGame;
        }
        
        public override string ToString()
        {
            GameInfoPoker gameInfo = (GameInfoPoker)_gameInfo;
            //packet_id(space)game_type;stake;maxplayers;action_time_limit;mode;limit;raise_time;raise_factor|
            string strRawPacket =
                string.Format("{0}*{1};{2};{3};{4};{5};{6};{7};{8};{9};{10};{11};{12};|",
                    (int)m_PacketId,
                    (int)gameInfo.game_type,
                    gameInfo.stake,
                    gameInfo.max_players,
                    gameInfo.action_time_imit,
                    gameInfo.strDescription,
                    (int)gameInfo.mode,
                    (int)gameInfo.limit_type,
                    gameInfo.fAntes,
                    gameInfo.fBringIn,
                    gameInfo.blindsInfo.raise_time,
                    gameInfo.blindsInfo.raise_factor,
                    gameInfo.fStartChips);

            return strRawPacket;
        }
    }
    
    /*
    * Create Game - Seven card stud
    */
    //public class NetPacketCreateRoomSevenStud : NetPacketCreateRoom
    //{
    //    public NetPacketCreateRoomSevenStud()
    //    {
    //        m_PacketId = PacketIDType.eCreatePokerGame;
    //    }

    //    public override string ToString()
    //    {
    //        GameInfoSevenStud gameInfo = (GameInfoSevenStud)_gameInfo;
    //        string strRawPacket =
    //            string.Format("{0}*{1};{2};{3};{4};{5};{6};{7};{8};{9}|",
    //                (int)m_PacketId,
    //                (int)gameInfo.game_type,
    //                gameInfo.stake,
    //                gameInfo.max_players,
    //                gameInfo.action_time_imit,
    //                gameInfo.strDescription,
    //                (int)gameInfo.mode,
    //                (int)gameInfo.limit_type,                    
    //                gameInfo.fAntes,
    //                gameInfo.fBringIn);

    //        return strRawPacket;
    //    }
    //}

    /*
     * Game list item
     */
    public class NetPacketGameListItem
        : INetPacket
    {
        public int _gameID;
        public string _strGameDesc;
        public float _fStake;
        public GameType _type;
        public int _nPlayers;
        public int _nMaxPlayers;

        public NetPacketGameListItem(){
            m_PacketId = PacketIDType.eGameListItem;
        }
        public override string ToString()
        {
            string strRawPacket =
                 string.Format("{0}*{1};{2};{3};{4};{5};{6};|",
                     (int)m_PacketId,
                     (int)_gameID,
                     (int)_type,
                     _fStake,
                     _strGameDesc,
                     _nPlayers,
                     _nMaxPlayers
                     );

            return strRawPacket;
        }
    }
    
    /*
     * Game list item - Poker
     */
    public class NetPacketPokerGameListItem 
        : NetPacketGameListItem
    {
        public GameLimitType _gameLimit;
        public float _fAntes;
        public float _fBringIn;
        public int _RaiseTime;
        public float _RaiseFactor;
        public NetPacketPokerGameListItem() : base() { }

        public override string ToString()
        {
            //unused for client
            //string strRawPacket =
            //     string.Format("{0}*{1};{2};{3};{4};{5};{6};{7};|",
            //         (int)m_PacketId,
            //         (int)_gameID,
            //         (int)_type,
            //         _fStake,
            //         _strGameDesc,
            //         _nPlayers,
            //         _nMaxPlayers,
            //         (int)_gameLimit);

            //return strRawPacket;
            return null;
        }
    }

    // Game list item - seven card stud Poker
    //public class NetPacketSevenCardStudGameListItem
    //    : NetPacketPokerGameListItem
    //{
    //    public GameLimitType _gameLimit;
    //    public float _fAntes;
    //    public float _fBringIn;
    //    public NetPacketSevenCardStudGameListItem() : base() { }

    //    public override string ToString()
    //    {
    //        return null;
    //    }
    //}

    /*
     * Packet for integer parameters 
     */

    public class NetPacketInt : INetPacket
    {
        public int _nParam;
        public NetPacketInt(PacketIDType id) {
            m_PacketId = id;
        }
        public override string ToString()
        {
            string strRawPacket =
                 string.Format("{0}*{1};|",
                     (int)m_PacketId,
                     _nParam);
            return strRawPacket;
        }
    }
    
    /* Packet for request logedin */
    public class NetPacketReqLogIn : INetPacket {
        public string _UserName;
        public string _Password;
        public NetPacketReqLogIn() {
            m_PacketId = PacketIDType.eReqLogIn;
        }
        
        public override string ToString()
        {
            string strRawPacket =
                 string.Format("{0}*{1};{2};|",
                     (int)m_PacketId,
                     _UserName,
                     _Password);
            return strRawPacket;
        }
    }
    
    /* Packet Client request feedback */
    public class NetPacketClientInfoFeedBack : INetPacket
    {
        public string _UserName;
        public string _Location;
        public int _PlayerID;
        public NetPacketClientInfoFeedBack()
        {
            m_PacketId = PacketIDType.eClientInfoFeedBack;
        }
        public override string ToString()
        {
            return null;
        }
    }


    /* game Player list */
    public class NetPacketPlayerList :
        INetPacket
    {
        public List<Player> _playerList;
        public int _roomID;

        public NetPacketPlayerList()
            : base()
        {
            m_PacketId = PacketIDType.ePlayerList;
            _playerList = new List<Player>();
        }

        //no content for client, only server sends player list        
        public override string ToString()
        {
            return null;
        }
    }
}
