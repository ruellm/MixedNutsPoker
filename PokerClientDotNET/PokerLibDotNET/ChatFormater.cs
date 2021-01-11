using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
    public class ChatFormater
    {
        public static string ClientToServer(string strMsg)
        { 
            string[] strDelim = new string[5];
            strDelim[0]=string.Format("{0}",PokerLibConstants.PACKET_DELIMETER);
            strDelim[1]=string.Format("{0}",PokerLibConstants.PACKET_PARAMETER_DELIMETER);
            strDelim[2]=string.Format("{0}",PokerLibConstants.PACKET_ARGUMENT_DELIMETER);
            strDelim[3]=string.Format("{0}",PokerLibConstants.PACKET_INTERNAL_DELIMETER);
            strDelim[4] = string.Format("{0}", PokerLibConstants.PACKET_INTERNAL_DELIMTER_2);

            string[] strReplacement = new string[5];
            strReplacement[0] = PokerLibConstants.DELIMETER_REPLACEMENT;
            strReplacement[1] = PokerLibConstants.PARAM_DELEM_REPLACEMENT;
            strReplacement[2] = PokerLibConstants.ARG_DELEM_REPLACEMENT;
            strReplacement[3] = PokerLibConstants.INTERN_DELEM_REPLACEMENT;
            strReplacement[4] = PokerLibConstants.INTERN_DELEM_2_REPLACEMENT;

            string strResult = strMsg;
            for (int x = 0; x < 5; x++) {
                strResult = strResult.Replace(strDelim[x], strReplacement[x]);
            }
            return strResult;
        }

        public static string ServerToClient(string strMsg)
        {
            string[] strDelim = new string[5];
            strDelim[0] = string.Format("{0}", PokerLibConstants.PACKET_DELIMETER);
            strDelim[1] = string.Format("{0}", PokerLibConstants.PACKET_PARAMETER_DELIMETER);
            strDelim[2] = string.Format("{0}", PokerLibConstants.PACKET_ARGUMENT_DELIMETER);
            strDelim[3] = string.Format("{0}", PokerLibConstants.PACKET_INTERNAL_DELIMETER);
            strDelim[4] = string.Format("{0}", PokerLibConstants.PACKET_INTERNAL_DELIMTER_2);

            string[] strReplacement = new string[5];
            strReplacement[0] = PokerLibConstants.DELIMETER_REPLACEMENT;
            strReplacement[1] = PokerLibConstants.PARAM_DELEM_REPLACEMENT;
            strReplacement[2] = PokerLibConstants.ARG_DELEM_REPLACEMENT;
            strReplacement[3] = PokerLibConstants.INTERN_DELEM_REPLACEMENT;
            strReplacement[4] = PokerLibConstants.INTERN_DELEM_2_REPLACEMENT;

            string strResult = strMsg;
            for (int x = 0; x < 5; x++)
            {
                strResult = strResult.Replace(strReplacement[x], strDelim[x]);
            }

            return strResult;
        }
    }
}
