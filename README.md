# walkingLizzard

Simple PlatformIO project for testing and calibrating a 4-leg, 8-servo robot on an Arduino UNO R4 Minima.

## What it does

The sketch in `src/walkingLizzard.ino` opens a serial control interface for one servo at a time. It is meant for safe calibration before building a full walking gait.

Servo pins are assigned like this:

- `FL_HIP` = 2
- `FL_KNEE` = 3
- `FR_HIP` = 4
- `FR_KNEE` = 5
- `BL_HIP` = 6
- `BL_KNEE` = 7
- `BR_HIP` = 8
- `BR_KNEE` = 9

## Requirements

- VS Code with the PlatformIO extension, or PlatformIO CLI
- Arduino UNO R4 Minima
- Arduino Servo library

## Run / Upload

### In VS Code

1. Open the project folder.
2. Build the project with the PlatformIO build button.
3. Upload it to the board with the PlatformIO upload button.
4. Open the serial monitor at `115200` baud.

### With PlatformIO CLI

```bash
pio run
pio run -t upload
pio device monitor -b 115200
```

## Usage

After upload, open the serial monitor and send commands one line at a time.

Important: manually position each joint near `90` degrees before attaching the servo.

### Commands

- `help` - show the command list
- `list` - show servo status and current limits
- `home` - move all attached servos back to `90`
- `attach N` - attach servo `N` and move it to `90`
- `detach N` - release servo `N`
- `N 90` - move servo `N` to an angle
- `N +5` - move servo `N` forward by 5 degrees
- `N -5` - move servo `N` backward by 5 degrees
- `min N 60` - set safe minimum for servo `N`
- `max N 120` - set safe maximum for servo `N`

### Example session

```text
attach 0
0 90
0 +5
list
home
detach 0
```

## Safety notes

- Default safe range starts very narrow: `75` to `105`.
- Expand limits only after confirming the joint moves in the correct direction.
- If a servo is not attached, commands for it will be ignored.
