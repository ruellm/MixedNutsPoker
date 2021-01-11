using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
    public abstract class INetPacket
    {
        protected PacketIDType m_PacketId;
        public override abstract string ToString();
        public PacketIDType GetID() {
            return m_PacketId;
        }
    }
}
