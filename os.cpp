#include <xc.h>

/*  SD CARD
 *  *******
 *  SCK2 - RB15 (IFS1, IEC1, IPC9)
 *  CSX  - RC4
 *  MOSI - RB5
 *  MISO - RC3
 *
 *  other CS: RB0, RC7
 */

#define MS 20000        // 1 ms

int read_cdc(char*&);
bool send_cdc(char*, int);

void init(void) {
  CNPUBSET = 1 << 8;		// enable B8 pull-up resistor
  TRISBCLR = 1 << 9;            // clear tris -> set B9 as output
  _CP0_SET_COMPARE(MS);         // set core timer interrupt condition
  SYSKEY = 0;                   // ensure OSCCON is locked
  SYSKEY = 0xAA996655;          // unlock sequence
  SYSKEY = 0x556699AA;
  CFGCONbits.IOLOCK = 0;        // allow write
  SDI2R = 0b0111;		// RC3 - SDI2
  RPB5R = 0b0100;		// RB5 - SDO2
  CFGCONbits.IOLOCK = 1;        // forbidden write
  SYSKEY = 0;                   // relock
  ANSELCCLR = 1 << 3;		// RC3
  TRISBCLR = TRISBSET = 1;
  TRISCCLR = TRISCSET = 0x90;
  IPC0bits.CTIP = 1;            // core timer interrupt at lowest priority
  IEC0bits.CTIE = 1;            // enable core timer interrupt
  INTCONSET = _INTCON_MVEC_MASK;
  __builtin_enable_interrupts();        // global interrupt enable
}

void toggle_LED(void) { LATBINV = 1 << 9; }

static volatile unsigned tick;

void poll(unsigned timestamp);  // it will be called when timestamp changes

static void loop(unsigned t) {
  static char *buf, buffer[64];
  static int len;
  if (buf) {
    if (send_cdc(buffer, len)) buf = 0;
  } else {
    if (send_cdc(0, 0))
      if ((len = read_cdc(buf)))
        for (int i = 0; i < len; i++) buffer[i] = buf[i];
  }
  static unsigned tick;
  if (tick != t) poll(tick = t);
}

bool wait(unsigned t) {
  unsigned u = tick;
  for (loop(u); t--; u = tick) while (u == tick) loop(u);
  return true;
}

extern "C"
__attribute__((interrupt(ipl1soft), vector(_CORE_TIMER_VECTOR), nomips16))
void ctisr(void) {
  _CP0_SET_COMPARE(_CP0_GET_COMPARE() + MS);    // next interrupt at 1 ms
  tick++;                                       // keep track of time
  IFS0bits.CTIF = 0;                            // clear flag
}

void on_switch_change(bool);
bool read_switch(void) {
  static int state = 0x100;	// assume switch is not pressed
  if (state != (PORTB & 0x100)) on_switch_change(state ^= 0x100);
  return state;
}
