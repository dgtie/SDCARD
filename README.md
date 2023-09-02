# PIC32MX Interfacing to SD CARD

### References
- SD CARD: elm-chan.org/docs/mmc/mmc_e.html
- FAT32: https://www.pjrc.com/tech/8051/ide/fat32.html

### Coverage
- SD Ver.2 & SD Ver.1
- FAT32
- search file in root directory only
- short file name (8.3 format)
- no writing to FAT (file allocation table), hence read and modify only

### Pin Assignment
- SCK2 - RB15
- CSX  - RC4
- MOSI - RB5 (SDO2)
- MISO - RC3 (SDI2)

### Peripherals
- SPI2: 2 MHz, master, CKE
- DMA0 (IFS1, IEC1, IPC10): write to SPI
- DMA1 (IFS1, IEC1, IPC10): read from SPI

### SD Card
- command phase: polling SPI
- block data transfer: DMA and Interrupts (takes 2 ms for 512 bytes)

### FAT32
- each File object takes more than half kilo bytes
- File f; // create a file object
- File f("ABC     TXT");  // create a file object and open file ABC.TXT
- bool File::open("ABC     TXT"); // there are 5 space between ABC and TXT
- void File::close(); // close file by writing up unsaved records
- int File::read(char *data, int length); // read a number of bytes, return number of byte actually read
- int File::write(char *data, int length); // write a number of bytes, return number of byte actually written

### Things Learned
- sudo dd if=/dev/sdb bs=512 count=1 skip=2048 2>/dev/null | hexdump -C -v
- CRC16(polynomal = 0x1021) & CRC7(polynomal = 0x09)
