# Rocker Scooter Liquidity Map Study for Sierra Chart

A Sierra Chart custom study that displays key liquidity levels in the Rocker Scooter trading style, including Weekly High/Low Points (WHP), Monthly High Point (MHP), Daily Deviation levels (DD), and session-based reference levels.

## Features

### Core Liquidity Levels
- **WHP High/Low**: Weekly High/Low Point levels (blue lines)
- **MHP**: Monthly High Point level (orange line)
- **DD High/Low**: Daily Deviation levels calculated as Previous Close ± Risk Interval (gray lines)

### Session Reference Levels
- **Previous Close**: Previous trading day's closing price (yellow line)
- **Current Open**: Current session's opening price (cyan line)
- **Half Gap**: Midpoint between Previous Close and Current Open (dashed gray line)

### Configuration Options
- Manual price entry mode for all levels
- Configurable session start/end times
- Risk interval adjustment for DD levels
- Individual level visibility toggles
- Update frequency control (new day only vs. continuous)

## Installation & Compilation

### Using Sierra Chart's Remote Build - Standard

1. **Place the source file**:
   - Copy `rs-liquiditymap.cpp` to your Sierra Chart `ACS_Source` folder
   - Default location: `C:\SierraChart\ACS_Source\`

2. **Compile using Remote Build**:
   - In Sierra Chart, go to **Analysis** → **Studies** → **Advanced Custom Studies**
   - Click **Build** → **Remote Build - Standard**
   - Select `rs-liquiditymap.cpp` from the file list
   - Click **Build Selected File**
   - Wait for compilation to complete

3. **Apply the study**:
   - Go to **Analysis** → **Studies**
   - Click **Add Custom Study**
   - Select "Liquidity Map" from the list
   - Configure your desired settings

## Configuration

### Manual vs Auto Mode
- **Auto Mode** (default): Automatically detects previous close and current open based on session times
- **Manual Mode**: Use manually entered prices for previous close and current open

### Key Settings
- **WHP High/Low**: Enter your weekly high/low point levels
- **MHP**: Enter your monthly high point level
- **Risk Interval (DD)**: Distance in points/ticks for Daily Deviation levels (default: 50)
- **Session Start/End Time**: Define your trading session (default: 9:30 AM - 4:00 PM)

## Known Issues & TODO

⚠️ **IMPORTANT NOTE**: There are currently issues with the Open and Close level detection logic. 

### Current Problems
- Previous Close detection may not work correctly in all market conditions
- Current Open detection can be unreliable during session transitions
- Session time logic needs refinement for different market hours

### Recommended Workaround
**For now, it's recommended to either:**
1. **Disable the problematic levels**: Uncheck "Show Previous Close" and "Show Current Open" in the study settings
2. **Use Manual Mode**: Enable "Use Manual Prices" and enter your Previous Close and Current Open values manually

### Contributing
If you can fix the Open/Close detection logic, **pull requests are welcome!** The main issues are in the auto-detection section of the code around lines 80-140.

## Study Parameters

| Parameter | Description | Default |
|-----------|-------------|---------|
| WHP High | Weekly High Point level | 0 |
| WHP Low | Weekly Low Point level | 0 |
| MHP | Monthly High Point level | 0 |
| Risk Interval (DD) | Daily Deviation distance | 50 |
| Session Start Time | Trading session start | 9:30 AM |
| Session End Time | Trading session end | 4:00 PM |
| Show Previous Close | Display previous close line | Yes |
| Show Current Open | Display current open line | Yes |
| Show Half Gap | Display half gap line | Yes |
| Update Only on New Trading Day | Limit updates to new days | No |
| Use Manual Prices | Enable manual price entry | No |
| Manual Previous Close | Manual previous close value | 0 |
| Manual Current Open | Manual current open value | 0 |

## Color Scheme

- **WHP High/Low**: Blue (`RGB(0,128,255)`)
- **MHP**: Orange (`RGB(255,140,0)`)
- **DD High/Low**: Gray (`RGB(180,180,180)`)
- **Previous Close**: Yellow (`RGB(255,255,0)`)
- **Current Open**: Cyan (`RGB(0,255,255)`)
- **Half Gap**: Gray dashed (`RGB(128,128,128)`)

## Requirements

- Sierra Chart version with Advanced Custom Studies support
- C++ compilation capability (Remote Build - Standard)

## License

This study is provided as-is for educational and trading purposes. Use at your own risk.
