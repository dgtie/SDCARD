#include "fat32.h"

void init(void), toggle_LED(void);
bool wait(unsigned);    // always return true
void USBDeviceInit(void);
bool send_cdc(char*, int);
bool read_switch(void);
void int2str(unsigned, char*);

char buffer[15];

int main(void) {
  init();
  USBDeviceInit();
  sd_init();
  while (read_switch()) wait(0);
  File f("ABC     TXT");
  int n = 9;
  while (n) {
    while (!send_cdc(0, 0));
    n = f.read(buffer, 15);
    send_cdc(buffer, n);
  }
  f.close();
  while (wait(0));      // call wait() to transfer control to poll()
}

void poll(unsigned t) {
  if (!(t & 15)) read_switch();		// read switch every 16 ms
  sd_poll(t);
}

void on_switch_change(bool b) {
  if (!b) toggle_LED();
}
