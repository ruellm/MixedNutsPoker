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

namespace PokerClientDotNET
{
    //limit UI helper
    public abstract class ILimitUIHelper
    {
        public GameInformation _GameInfo;
        public Button _RaiseButton;
        public TextBox _RaiseTextBox;

        public abstract void UpdateRaiseButton(Player _Me, 
            float _MaxBet, short _MySeat, out float _fCurrRaise);

        protected bool IsRaisePossible(short _MySeat)
        {
            //count active players in the table
            for (int ix = 0; ix < _GameInfo._Table._Seats.Count; ix++)
            {
                if (_GameInfo._Table._Seats[ix].player != null &&
                    _GameInfo._Table._Seats[ix].player._IsPlaying == true &&
                    (_GameInfo._Table._Seats[ix].player._Action != PokerAction.eFold &&
                    _GameInfo._Table._Seats[ix].player._Action != PokerAction.eAllIn) &&
                    ix != _MySeat) { return true; }
            }

            return false;
        }

        public Pot GetCurrentPot()
        {
           return _GameInfo._Table._PotList[_GameInfo._Table._PotList.Count - 1];
        }

    }

    //no limit
    public class NoLimitUIHelper :
        ILimitUIHelper
    {        
        
        public override void UpdateRaiseButton(Player _Me,
            float _MaxBet, short _MySeat, out float _fCurrRaise) 
        {
            _RaiseButton.Visibility = (IsRaisePossible(_MySeat)) ? Visibility.Visible : Visibility.Collapsed;
            _RaiseTextBox.Visibility = _RaiseButton.Visibility;
            if (_RaiseButton.Visibility != Visibility.Visible) {
                _fCurrRaise = 0.0f;    
                return; 
            }

            Pot currpot = GetCurrentPot();
           // Pot currpot =  _GameInfo._Table._PotList[_GameInfo._Table._PotList.Count - 1];
            float fRaiseVal = currpot._currRaise;

            if (fRaiseVal == 0.0f)
            {
                fRaiseVal = _GameInfo._Info.stake;
            }
            else
            {
                fRaiseVal += _GameInfo._Info.stake;
                if (fRaiseVal >= _Me._TotalChips)
                {
                    fRaiseVal = _Me._TotalChips;
                }
            }

            //test with _Maxbet
            if (fRaiseVal > _MaxBet)
            {
                fRaiseVal = _MaxBet;
            }

            _RaiseButton.Content = "Raise " + fRaiseVal.ToString();
            _RaiseTextBox.Text = fRaiseVal.ToString();
            _fCurrRaise = fRaiseVal;
        }
    }

    //fixed limit
    public class FixedLimitUIHelper:
        ILimitUIHelper
    {
        public int _CountRaisers;

        private bool IsRaisePossible(short _MySeat)
        {
            if (!base.IsRaisePossible(_MySeat)) return false;

            //the "+" traps raiser in advance
             if (_CountRaisers+1 >= PokerLibConstants.MAX_RAISER_CNT)
                return false;
            return true;
        }
             
        private bool IsNextStakeStage()
        {
            //get table state
            TableState state = _GameInfo._Table._State;
            if (PokerLibGlobal.IsHoldemGame(_GameInfo._Info.game_type))
            {
                if (state == TableState.eTurnState ||
                    state == TableState.eRiverState) return true;
            }
            else if (PokerLibGlobal.IsSevenCardStudGame(_GameInfo._Info.game_type)) {
                if (state == TableState.e5thStreetState ||
                       state == TableState.e6thStreetState ||
                       state == TableState.e7thStreetState) return true;
            }
            return false;
        }
        
        public override void UpdateRaiseButton(Player _Me, 
            float _MaxBet, short _MySeat,out float _fCurrRaise)
        {
            //_fCurrRaise = 0.0f;
            _RaiseButton.Visibility = (IsRaisePossible(_MySeat)) ? Visibility.Visible : Visibility.Collapsed;
            
            //raise text alwys invisible
            _RaiseTextBox.Visibility = Visibility.Collapsed;

            if (_RaiseButton.Visibility != Visibility.Visible)
            {
                _fCurrRaise = 0.0f;
                return;
            }

            Pot currpot = GetCurrentPot();
            bool nextStage = IsNextStakeStage();

            //Pot currpot = _GameInfo._Table._PotList[_GameInfo._Table._PotList.Count - 1];
            float fRaiseVal = currpot._currRaise;
            float fMinimumRaise = (nextStage) ? _GameInfo._Info.stake : _GameInfo._Info.stake / 2.0f;


            if (fRaiseVal == 0.0f)
            {
                fRaiseVal = fMinimumRaise;
            }
            else
            {
                //fRaiseVal *= 2.0f;
                fRaiseVal += fMinimumRaise;
                if (fRaiseVal >= _Me._TotalChips)
                {
                    fRaiseVal = _Me._TotalChips;
                }
            }

            //test with _Maxbet
            if (fRaiseVal > _MaxBet)
            {
                fRaiseVal = _MaxBet;
            }

            _RaiseButton.Content = "Raise " + fRaiseVal.ToString();
            _RaiseTextBox.Text = fRaiseVal.ToString();
            _fCurrRaise = fRaiseVal;
        }
    }

    public class PotLimitUIHelper :
        ILimitUIHelper
    {
        public Button _PotButton;
       
        public PotLimitUIHelper()
        {
           
        }

        public float GetPotLimitRaise(Player me)
        {
            //some board pots
            float boardPot = 0.0f;
            float trailingPot = 0.0f;
            float totalPot = 0.0f;
            float potLimitRaise = 0.0f;
            Pot currpot = GetCurrentPot();
            if (me == null) return 0.0f;

            foreach (Pot pot in _GameInfo._Table._PotList)
            {
                boardPot += pot._amnt;
            }
            for (int ix = 0; ix < _GameInfo._Table._Seats.Count; ix++)
            {
                if (_GameInfo._Table._Seats[ix].player != null &&
                    _GameInfo._Table._Seats[ix].player._IsPlaying == true)
                {
                    trailingPot += _GameInfo._Table._Seats[ix].player._fBet;
                }
            }            
            float fNeedsToCall = currpot._currRaise - me._fBet;
            if (currpot._currRaise == 0)
                fNeedsToCall = 0;

            //Pot limit total pot os boardPot + TrailingPot + ToCa;;
            totalPot = boardPot + trailingPot + fNeedsToCall;
            potLimitRaise = totalPot + fNeedsToCall;
            return potLimitRaise;
        }

        public override void UpdateRaiseButton(Player _Me,
            float _MaxBet, short _MySeat, out float _fCurrRaise)
        {
            _RaiseButton.Visibility = (IsRaisePossible(_MySeat)) ? Visibility.Visible : Visibility.Collapsed;
            _RaiseTextBox.Visibility = _RaiseButton.Visibility;
            _PotButton.Visibility = _RaiseButton.Visibility;

            if (_RaiseButton.Visibility != Visibility.Visible)
            {
                _fCurrRaise = 0.0f;
                return;
            }

            Pot currpot = GetCurrentPot();
            // Pot currpot =  _GameInfo._Table._PotList[_GameInfo._Table._PotList.Count - 1];
            float fRaiseVal = currpot._currRaise;

            if (fRaiseVal == 0.0f)
            {
                fRaiseVal = _GameInfo._Info.stake;
            }
            else
            {
                fRaiseVal += _GameInfo._Info.stake;
                if (fRaiseVal >= _Me._TotalChips)
                {
                    fRaiseVal = _Me._TotalChips;
                }
            }

            //test with _Maxbet
            if (fRaiseVal > _MaxBet)
            {
                fRaiseVal = _MaxBet;
            }

            _RaiseButton.Content = "Raise " + fRaiseVal.ToString();
            _RaiseTextBox.Text = fRaiseVal.ToString();
            _fCurrRaise = fRaiseVal;
        }
    }

    //limit helper factory
    public class LimitUIFactory
    {
        public static ILimitUIHelper CreateLimitHelper(GameLimitType type) 
        {            
            ILimitUIHelper limit = null;

            switch (type)
            {
                case GameLimitType.ePotLimit:
                    limit = new PotLimitUIHelper();
                    break;
                case GameLimitType.eNoLimit:{
                    limit = new NoLimitUIHelper();
                    } break;
                case GameLimitType.eFixedLimit:
                    {
                        limit = new FixedLimitUIHelper();
                    } break;                
            }

            return limit;
        }
    }
}
