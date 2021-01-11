using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
    public class PacketParser
    {
        private NetPacketFactory _factory;

        public PacketParser()
        {
            _factory = new NetPacketFactory();
        }

        public void Parse(string strRawBuffer, ref string strIncomplete, 
            ref List<INetPacket> result)
        {
            int spos = 0;
            int lpos = 0;
            string strPacket = "";
            int startIndex = 0;
            
            if (result == null)
            {
                result = new List<INetPacket>();
            }

            while (startIndex < strRawBuffer.Length)
            {
                bool bGenerate = true;
                spos = startIndex;
                lpos = strRawBuffer.IndexOf(PokerLibConstants.PACKET_DELIMETER, spos);
                if (lpos == -1)
                {
                    lpos = strRawBuffer.Length;
                    bGenerate = false;
                }
                strPacket = strRawBuffer.Substring(spos, (lpos - spos) + 1);
                startIndex = lpos + 1;
                strIncomplete += strPacket;
                if (bGenerate)
                {
                    lpos = strIncomplete.IndexOf(PokerLibConstants.PACKET_DELIMETER, 0);
                    string strPacketComplete = strIncomplete.Substring(0, (lpos + 1));
                    strIncomplete = strIncomplete.Substring(lpos + 1);
                    INetPacket packet = Parse(strPacketComplete);
                    result.Add(packet);
                }
            }
        }

        public void Parse(string strBuffer,
            ref List<INetPacket> result)
        {            
            if (result == null) {
                result = new List<INetPacket>();
            }
            string[] tokens = strBuffer.Split(PokerLibConstants.PACKET_DELIMETER);
            for (int i = 0; i < tokens.Length-1; i++) {
                string[] token_argument = tokens[i].Split(PokerLibConstants.PACKET_ARGUMENT_DELIMETER);
                INetPacket packet = _factory.BuildFromTokens(token_argument);
                result.Add(packet);
            }            
        }

        public INetPacket Parse(string strRawData)
        {
            string[] tokens = strRawData.Split(PokerLibConstants.PACKET_DELIMETER);
            string[] token_argument = tokens[0].Split(PokerLibConstants.PACKET_ARGUMENT_DELIMETER);
            INetPacket packet = _factory.BuildFromTokens(token_argument);
            return packet;
        }
    }
}
