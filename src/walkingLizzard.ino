#include <Servo.h>

// ----------------------------------------------------------------
// Pin assignments — 4 legs x 2 servos
// Wire one servo at a time during calibration.
//
//  ID  Name      Pin   Leg
//   0  FL_HIP     2    Front Left  — swings leg fwd/back
//   1  FL_KNEE    3    Front Left  — lifts/lowers foot
//   2  FR_HIP     4    Front Right
//   3  FR_KNEE    5    Front Right
//   4  BL_HIP     6    Back Left
//   5  BL_KNEE    7    Back Left
//   6  BR_HIP     8    Back Right
//   7  BR_KNEE    9    Back Right
// ----------------------------------------------------------------

const int NUM  = 8;
const int PINS[NUM] = {2, 3, 4, 5, 6, 7, 8, 9};
const char* NAMES[NUM] = {
  "FL_HIP","FL_KNEE","FR_HIP","FR_KNEE",
  "BL_HIP","BL_KNEE","BR_HIP","BR_KNEE"
};

Servo   servos[NUM];
int     pos[NUM];
int     safeMin[NUM];
int     safeMax[NUM];
bool    attached[NUM];

// 20 ms per degree = ~3 seconds to travel 150 degrees. Painfully slow. Safe.
const int STEP_MS = 20;

// ----------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------

void slowMove(int id, int target) {
  if (!attached[id]) { Serial.println("not attached"); return; }
  target = constrain(target, safeMin[id], safeMax[id]);
  int step = (target > pos[id]) ? 1 : -1;
  while (pos[id] != target) {
    pos[id] += step;
    servos[id].write(pos[id]);
    delay(STEP_MS);
  }
}

void printStatus() {
  Serial.println("ID  Name       pos  min  max");
  for (int i = 0; i < NUM; i++) {
    Serial.print(i); Serial.print("   ");
    Serial.print(NAMES[i]); Serial.print("  ");
    if (attached[i]) {
      Serial.print(pos[i]); Serial.print("    ");
      Serial.print(safeMin[i]); Serial.print("    ");
      Serial.println(safeMax[i]);
    } else {
      Serial.println("(not attached)");
    }
  }
}

void printHelp() {
  Serial.println("=== COMMANDS ===");
  Serial.println("  attach N       attach servo N (commands it to 90 — pre-position joint first!)");
  Serial.println("  detach N       release servo N");
  Serial.println("  N 90           move servo N to 90 deg (slow)");
  Serial.println("  N +5           servo N forward 5 deg");
  Serial.println("  N -5           servo N back 5 deg");
  Serial.println("  min N 60       set servo N safe minimum");
  Serial.println("  max N 120      set servo N safe maximum");
  Serial.println("  home           all attached servos -> 90");
  Serial.println("  list           show all positions and limits");
  Serial.println("  help           this message");
  Serial.println("================");
}

// ----------------------------------------------------------------
// Setup
// ----------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM; i++) {
    safeMin[i]  = 75;   // very conservative — only 15 deg each way
    safeMax[i]  = 105;
    pos[i]      = 90;
    attached[i] = false;
  }
  Serial.println("\n*** Servo Calibration — walkingLizzard ***");
  Serial.println("IMPORTANT: manually position each joint to ~90 deg neutral before attaching.");
  Serial.println("Default safe range is 75-105. Expand limits only after confirming direction is clear.");
  Serial.println();
  printHelp();
}

// ----------------------------------------------------------------
// Loop — parse serial commands
// ----------------------------------------------------------------

void loop() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  if (cmd.length() == 0) return;

  // --- help / list / home ---
  if (cmd == "help")  { printHelp();   return; }
  if (cmd == "list")  { printStatus(); return; }
  if (cmd == "home")  {
    for (int i = 0; i < NUM; i++) if (attached[i]) slowMove(i, 90);
    Serial.println("home done");
    return;
  }

  // --- attach N ---
  if (cmd.startsWith("attach ")) {
    int id = cmd.substring(7).toInt();
    if (id < 0 || id >= NUM) { Serial.println("bad id"); return; }
    if (attached[id]) { Serial.println("already attached"); return; }
    servos[id].attach(PINS[id], 500, 2400);
    servos[id].write(90);
    pos[id]      = 90;
    attached[id] = true;
    Serial.print("attached "); Serial.print(NAMES[id]); Serial.println(" -> 90");
    return;
  }

  // --- detach N ---
  if (cmd.startsWith("detach ")) {
    int id = cmd.substring(7).toInt();
    if (id < 0 || id >= NUM) { Serial.println("bad id"); return; }
    servos[id].detach();
    attached[id] = false;
    Serial.print("detached "); Serial.println(NAMES[id]);
    return;
  }

  // --- min N V  /  max N V ---
  if (cmd.startsWith("min ") || cmd.startsWith("max ")) {
    bool isMin = cmd.startsWith("min");
    String rest = cmd.substring(4);
    int sp = rest.indexOf(' ');
    if (sp < 0) { Serial.println("usage: min N val"); return; }
    int id  = rest.substring(0, sp).toInt();
    int val = rest.substring(sp + 1).toInt();
    if (id < 0 || id >= NUM) { Serial.println("bad id"); return; }
    if (isMin) safeMin[id] = val;
    else       safeMax[id] = val;
    Serial.print(isMin ? "min" : "max");
    Serial.print("["); Serial.print(id); Serial.print("] = "); Serial.println(val);
    return;
  }

  // --- N angle  /  N +step  /  N -step ---
  int sp = cmd.indexOf(' ');
  if (sp < 0) { Serial.println("unknown command — type 'help'"); return; }

  int    id   = cmd.substring(0, sp).toInt();
  String rest = cmd.substring(sp + 1);
  rest.trim();

  if (id < 0 || id >= NUM) { Serial.println("bad servo id"); return; }

  int target;
  if      (rest.startsWith("+")) target = pos[id] + rest.substring(1).toInt();
  else if (rest.startsWith("-")) target = pos[id] - rest.substring(1).toInt();
  else                           target = rest.toInt();

  int clamped = constrain(target, safeMin[id], safeMax[id]);
  if (clamped != target) {
    Serial.print("  (clamped to "); Serial.print(clamped); Serial.println(")");
  }
  slowMove(id, clamped);
  Serial.print(NAMES[id]); Serial.print(" -> "); Serial.println(pos[id]);
}
