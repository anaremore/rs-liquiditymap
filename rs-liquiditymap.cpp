#include "sierrachart.h"

SCDLLName("Liquidity Map")

SCSFExport scsf_LiquidityMapping(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_WHPHigh = sc.Subgraph[0];
	SCSubgraphRef Subgraph_WHPLow = sc.Subgraph[1];
	SCSubgraphRef Subgraph_MHP = sc.Subgraph[2];
	SCSubgraphRef Subgraph_DDHigh = sc.Subgraph[3];
	SCSubgraphRef Subgraph_DDLow = sc.Subgraph[4];
	SCSubgraphRef Subgraph_PrevClose = sc.Subgraph[5];
	SCSubgraphRef Subgraph_CurrentOpen = sc.Subgraph[6];
	SCSubgraphRef Subgraph_HalfGap = sc.Subgraph[7];

	SCInputRef Input_WHPHigh = sc.Input[0];
	SCInputRef Input_WHPLow = sc.Input[1];
	SCInputRef Input_MHP = sc.Input[2];
	SCInputRef Input_RiskInterval = sc.Input[3];
	SCInputRef Input_SessionStartTime = sc.Input[4];
	SCInputRef Input_SessionEndTime = sc.Input[5];
	SCInputRef Input_ShowPrevClose = sc.Input[6];
	SCInputRef Input_ShowCurrentOpen = sc.Input[7];
	SCInputRef Input_ShowHalfGap = sc.Input[8];
	SCInputRef Input_UpdateOnlyOnNewDay = sc.Input[9];
	SCInputRef Input_UseManualPrices = sc.Input[10];
	SCInputRef Input_ManualPrevClose = sc.Input[11];
	SCInputRef Input_ManualCurrentOpen = sc.Input[12];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Liquidity Map";
		sc.StudyDescription = "Rocket Scooter style liquidity map levels.";
		sc.AutoLoop = 0;
		sc.DrawZeros = 0;
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.ScaleRangeType = SCALE_SAMEASREGION;

		Input_WHPHigh.Name = "WHP High"; Input_WHPHigh.SetFloat(0);
		Input_WHPLow.Name = "WHP Low"; Input_WHPLow.SetFloat(0);
		Input_MHP.Name = "MHP"; Input_MHP.SetFloat(0);
		Input_RiskInterval.Name = "Risk Interval (DD)"; Input_RiskInterval.SetFloat(50);
		Input_SessionStartTime.Name = "Session Start Time (HHMM)"; Input_SessionStartTime.SetTime(HMS_TIME(9,30,0));
		Input_SessionEndTime.Name = "Session End Time (HHMM)"; Input_SessionEndTime.SetTime(HMS_TIME(16,0,0));
		Input_ShowPrevClose.Name = "Show Previous Close"; Input_ShowPrevClose.SetYesNo(true);
		Input_ShowCurrentOpen.Name = "Show Current Open"; Input_ShowCurrentOpen.SetYesNo(true);
		Input_ShowHalfGap.Name = "Show Half Gap"; Input_ShowHalfGap.SetYesNo(true);
		Input_UpdateOnlyOnNewDay.Name = "Update Only on New Trading Day"; Input_UpdateOnlyOnNewDay.SetYesNo(false);
		Input_UseManualPrices.Name = "Use Manual Prices"; Input_UseManualPrices.SetYesNo(false);
		Input_ManualPrevClose.Name = "Manual Previous Close"; Input_ManualPrevClose.SetFloat(0);
		Input_ManualCurrentOpen.Name = "Manual Current Open"; Input_ManualCurrentOpen.SetFloat(0);

		for (int i = 0; i < 8; ++i) {
			sc.Subgraph[i].LineWidth = 2;
			sc.Subgraph[i].DrawZeros = 0;
			sc.Subgraph[i].DrawStyle = DRAWSTYLE_LINE;
			sc.Subgraph[i].LineStyle = LINESTYLE_UNSET;
		}

		Subgraph_WHPHigh.Name = "WHP High"; Subgraph_WHPHigh.PrimaryColor = RGB(0,128,255);
		Subgraph_WHPLow.Name = "WHP Low"; Subgraph_WHPLow.PrimaryColor = RGB(0,128,255);
		Subgraph_MHP.Name = "MHP"; Subgraph_MHP.PrimaryColor = RGB(255,140,0);
		Subgraph_DDHigh.Name = "DD High"; Subgraph_DDHigh.PrimaryColor = RGB(180,180,180);
		Subgraph_DDLow.Name = "DD Low"; Subgraph_DDLow.PrimaryColor = RGB(180,180,180);
		Subgraph_PrevClose.Name = "Previous Close"; Subgraph_PrevClose.PrimaryColor = RGB(255,255,0);
		Subgraph_CurrentOpen.Name = "Current Open"; Subgraph_CurrentOpen.PrimaryColor = RGB(0,255,255);
		Subgraph_HalfGap.Name = "Half Gap"; Subgraph_HalfGap.PrimaryColor = RGB(128,128,128);
		Subgraph_HalfGap.LineStyle = LINESTYLE_DASH; Subgraph_HalfGap.LineWidth = 1;
		return;
	}

	float& r_CachedPrevClose = sc.GetPersistentFloat(1);
	float& r_CachedCurrentOpen = sc.GetPersistentFloat(2);
	SCDateTime& r_LastProcessedTradingDay = sc.GetPersistentSCDateTime(1);
	int& r_LastUpdateIndex = sc.GetPersistentInt(1);

	SCDateTime CurrentTradingDay = sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.ArraySize - 1]);
	bool NeedToRecalculate = sc.IsFullRecalculation || (CurrentTradingDay != r_LastProcessedTradingDay) || !Input_UpdateOnlyOnNewDay.GetYesNo();

	// Check if we should use manual prices or auto-detection
	if (Input_UseManualPrices.GetYesNo())
	{
		// Use manual prices
		r_CachedPrevClose = Input_ManualPrevClose.GetFloat();
		r_CachedCurrentOpen = Input_ManualCurrentOpen.GetFloat();
	}
	else if (NeedToRecalculate)
	{
		// Auto-detection mode
		bool FoundPrevClose = false;
		bool FoundCurrentOpen = false;
		double SessionStartTime = (double)Input_SessionStartTime.GetTime() / SECONDS_PER_DAY;
		double SessionEndTime = (double)Input_SessionEndTime.GetTime() / SECONDS_PER_DAY;

		// Reset cached values
		r_CachedPrevClose = 0.0f;
		r_CachedCurrentOpen = 0.0f;

		// Find previous close - look for the last bar from previous trading day
		for (int i = sc.ArraySize - 1; i >= 1; --i)
		{
			SCDateTime dt = sc.BaseDateTimeIn[i];
			SCDateTime barTradingDay = sc.GetTradingDayDate(dt);
			
			if (barTradingDay < CurrentTradingDay)
			{
				r_CachedPrevClose = sc.Close[i];
				FoundPrevClose = true;
				break;
			}
		}

		// Find current session open - first bar at or after session start time for current trading day
		for (int i = 1; i < sc.ArraySize; ++i)
		{
			SCDateTime dt = sc.BaseDateTimeIn[i];
			SCDateTime barTradingDay = sc.GetTradingDayDate(dt);
			double barTime = dt.GetTime();
			
			if (barTradingDay == CurrentTradingDay && barTime >= SessionStartTime)
			{
				r_CachedCurrentOpen = sc.Open[i];
				FoundCurrentOpen = true;
				break;
			}
		}

		// Fallback: if no previous close found, use yesterday's data or current close
		if (!FoundPrevClose)
		{
			// Look for any bar from yesterday
			SCDateTime Yesterday = CurrentTradingDay - 1;
			for (int i = sc.ArraySize - 1; i >= 1; --i)
			{
				SCDateTime barTradingDay = sc.GetTradingDayDate(sc.BaseDateTimeIn[i]);
				if (barTradingDay == Yesterday)
				{
					r_CachedPrevClose = sc.Close[i];
					FoundPrevClose = true;
					break;
				}
			}
		}

		// If still no previous close, use current close as fallback
		if (!FoundPrevClose && sc.ArraySize > 1)
		{
			r_CachedPrevClose = sc.Close[sc.ArraySize - 1];
		}

		// If no current open found, use the first bar of current trading day
		if (!FoundCurrentOpen)
		{
			for (int i = 1; i < sc.ArraySize; ++i)
			{
				SCDateTime barTradingDay = sc.GetTradingDayDate(sc.BaseDateTimeIn[i]);
				if (barTradingDay == CurrentTradingDay)
				{
					r_CachedCurrentOpen = sc.Open[i];
					FoundCurrentOpen = true;
					break;
				}
			}
		}

		r_LastProcessedTradingDay = CurrentTradingDay;
	}

	float WHPHigh = Input_WHPHigh.GetFloat();
	float WHPLow = Input_WHPLow.GetFloat();
	float MHP = Input_MHP.GetFloat();
	float RiskInterval = Input_RiskInterval.GetFloat();
	
	// Calculate half gap only if we have valid cached values
	float HalfGap = 0.0f;
	if (r_CachedCurrentOpen != 0.0f && r_CachedPrevClose != 0.0f)
	{
		HalfGap = (r_CachedCurrentOpen + r_CachedPrevClose) / 2.0f;
	}
	
	float DDHigh = r_CachedPrevClose + RiskInterval;
	float DDLow = r_CachedPrevClose - RiskInterval;

	int StartIndex = NeedToRecalculate ? 0 : sc.UpdateStartIndex;
	double SessionStartTime = (double)Input_SessionStartTime.GetTime() / SECONDS_PER_DAY;

	for (int i = StartIndex; i < sc.ArraySize; ++i)
	{
		SCDateTime dt = sc.BaseDateTimeIn[i];
		SCDateTime tradingDay = sc.GetTradingDayDate(dt);
		double timeOfBar = dt.GetTime();

		Subgraph_WHPHigh[i] = WHPHigh;
		Subgraph_WHPLow[i] = WHPLow;
		Subgraph_MHP[i] = MHP;
		Subgraph_DDHigh[i] = DDHigh;
		Subgraph_DDLow[i] = DDLow;

		Subgraph_PrevClose[i] = Input_ShowPrevClose.GetYesNo() ? r_CachedPrevClose : 0.0f;

		if (Input_ShowCurrentOpen.GetYesNo() && tradingDay == CurrentTradingDay)
		{
			if (Input_UseManualPrices.GetYesNo())
			{
				// Manual mode: always show the manual current open price
				Subgraph_CurrentOpen[i] = r_CachedCurrentOpen;
			}
			else if (timeOfBar >= SessionStartTime && r_CachedCurrentOpen != 0.0f)
			{
				// Auto mode: After session start time, show the locked-in session open price
				Subgraph_CurrentOpen[i] = r_CachedCurrentOpen;
			}
			else
			{
				// Auto mode: Before session start time, show current price (close of current bar)
				Subgraph_CurrentOpen[i] = sc.Close[i];
			}
		}
		else
		{
			Subgraph_CurrentOpen[i] = 0.0f;
		}

		Subgraph_HalfGap[i] = Input_ShowHalfGap.GetYesNo() ? HalfGap : 0.0f;
	}

	r_LastUpdateIndex = sc.ArraySize - 1;
}
