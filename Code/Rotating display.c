#include <mega64.h>
#include <delay.h>


#ifndef RXB8
#define RXB8 1
#endif

#ifndef TXB8
#define TXB8 0
#endif

#ifndef UPE
#define UPE 2
#endif

#ifndef DOR
#define DOR 3
#endif

#ifndef FE
#define FE 4
#endif

#ifndef UDRE
#define UDRE 5
#endif

#ifndef RXC
#define RXC 7
#endif

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

// USART1 Receiver buffer
#define RX_BUFFER_SIZE1 32
char rx_buffer1[RX_BUFFER_SIZE1];

#if RX_BUFFER_SIZE1 <= 256
unsigned char rx_wr_index1,rx_rd_index1,rx_counter1;
#else
unsigned int rx_wr_index1,rx_rd_index1,rx_counter1;
#endif

// This flag is set on USART1 Receiver buffer overflow
bit rx_buffer_overflow1;

// USART1 Receiver interrupt service routine
interrupt [USART1_RXC] void usart1_rx_isr(void)
{
char status,data;
status=UCSR1A;
data=UDR1;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer1[rx_wr_index1++]=data;
#if RX_BUFFER_SIZE1 == 256
   // special case for receiver buffer size=256
   if (++rx_counter1 == 0) rx_buffer_overflow1=1;
#else
   if (rx_wr_index1 == RX_BUFFER_SIZE1) rx_wr_index1=0;
   if (++rx_counter1 == RX_BUFFER_SIZE1)
      {
      rx_counter1=0;
      rx_buffer_overflow1=1;
      }
#endif
   }
}

// Get a character from the USART1 Receiver buffer
#pragma used+
char getchar1(void)
{
char data;
while (rx_counter1==0);
data=rx_buffer1[rx_rd_index1++];
#if RX_BUFFER_SIZE1 != 256
if (rx_rd_index1 == RX_BUFFER_SIZE1) rx_rd_index1=0;
#endif
#asm("cli")
--rx_counter1;
#asm("sei")
return data;
}
#pragma used-
// Write a character to the USART1 Transmitter
#pragma used+
void putchar1(char c)
{
while ((UCSR1A & DATA_REGISTER_EMPTY)==0);
UDR1=c;
}
#pragma used-



unsigned int p=0;
int t=0;
int i;

// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void)
{
   p=TCNT0+(i<<8);
   t=p/120;
   TCNT0=0;
   i=0;

}

 // Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
i++;

}


unsigned char im[240]=
{
    0x00,0x00,    //    0002        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    0004        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    0006        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    0008        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    000A        . . . . . . . . . . . . . . . . 
    0x3C,0x00,    //    000C        . . # # # # . . . . . . . . . . 
    0x62,0x00,    //    000E        . # # . . . # . . . . . . . . . 
    0xC0,0x00,    //    0010        # # . . . . . . . . . . . . . . 
    0xC0,0x00,    //    0012        # # . . . . . . . . . . . . . . 
    0xC0,0x00,    //    0014        # # . . . . . . . . . . . . . . 
    0xC0,0x00,    //    0016        # # . . . . . . . . . . . . . . 
    0xD8,0x00,    //    0018        # # . # # . . . . . . . . . . . 
    0xD4,0x00,    //    001A        # # . # . # . . . . . . . . . . 
    0xD6,0x00,    //    001C        # # . # . # # . . . . . . . . . 
    0x56,0x00,    //    001E        . # . # . # # . . . . . . . . . 
    0x36,0x00,    //    0020        . . # # . # # . . . . . . . . . 
    0x06,0x00,    //    0022        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0024        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0026        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0028        . . . . . # # . . . . . . . . . 
    0x03,0x80,    //    002A        . . . . . . # # # . . . . . . . 
    0x07,0x00,    //    002C        . . . . . # # # . . . . . . . . 
    0x06,0x00,    //    002E        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0030        . . . . . # # . . . . . . . . . 
    0x03,0x80,    //    0032        . . . . . . # # # . . . . . . . 
    0x07,0x00,    //    0034        . . . . . # # # . . . . . . . . 
    0x06,0x00,    //    0036        . . . . . # # . . . . . . . . . 
    0x06,0x60,    //    0038        . . . . . # # . . # # . . . . . 
    0x03,0xC0,    //    003A        . . . . . . # # # # . . . . . . 
    0x03,0xFF,    //    003C        . . . . . . # # # # # # # # # # 
    0x06,0x00,    //    003E        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0040        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0042        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0044        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0046        . . . . . # # . . . . . . . . . 
    0x26,0x00,    //    0048        . . # . . # # . . . . . . . . . 
    0x63,0xC0,    //    004A        . # # . . . # # # # . . . . . . 
    0x03,0x00,    //    004C        . . . . . . # # . . . . . . . . 
    0x06,0x00,    //    004E        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0050        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0052        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0054        . . . . . # # . . . . . . . . . 
    0x07,0xC0,    //    0056        . . . . . # # # # # . . . . . . 
    0x07,0xA0,    //    0058        . . . . . # # # # . # . . . . . 
    0x03,0x30,    //    005A        . . . . . . # # . . # # . . . . 
    0x03,0x30,    //    005C        . . . . . . # # . . # # . . . . 
    0x03,0x30,    //    005E        . . . . . . # # . . # # . . . . 
    0x03,0x20,    //    0060        . . . . . . # # . . # . . . . . 
    0x00,0x00,    //    0062        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    0064        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    0066        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    0068        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    006A        . . . . . . . . . . . . . . . . 
    0x03,0xFF,    //    006C        . . . . . . # # # # # # # # # # 
    0x06,0x00,    //    006E        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0070        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0072        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0074        . . . . . # # . . . . . . . . . 
    0x66,0x40,    //    0076        . # # . . # # . . # . . . . . . 
    0x66,0xE0,    //    0078        . # # . . # # . # # # . . . . . 
    0x03,0xC0,    //    007A        . . . . . . # # # # . . . . . . 
    0x03,0xFF,    //    007C        . . . . . . # # # # # # # # # # 
    0x06,0x00,    //    007E        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0080        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0082        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0084        . . . . . # # . . . . . . . . . 
    0x66,0x40,    //    0086        . # # . . # # . . # . . . . . . 
    0x66,0xE0,    //    0088        . # # . . # # . # # # . . . . . 
    0x03,0xC0,    //    008A        . . . . . . # # # # . . . . . . 
    0x00,0x00,    //    008C        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    008E        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    0090        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    0092        . . . . . . . . . . . . . . . . 
    0x03,0x80,    //    0094        . . . . . . # # # . . . . . . . 
    0x06,0x00,    //    0096        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    0098        . . . . . # # . . . . . . . . . 
    0x06,0x38,    //    009A        . . . . . # # . . . # # # . . . 
    0x06,0x30,    //    009C        . . . . . # # . . . # # . . . . 
    0x02,0x60,    //    009E        . . . . . . # . . # # . . . . . 
    0x03,0xC0,    //    00A0        . . . . . . # # # # . . . . . . 
    0x00,0x00,    //    00A2        . . . . . . . . . . . . . . . . 
    0x80,0x00,    //    00A4        # . . . . . . . . . . . . . . . 
    0x83,0x80,    //    00A6        # . . . . . # # # . . . . . . . 
    0xC6,0x60,    //    00A8        # # . . . # # . . # # . . . . . 
    0xE6,0x60,    //    00AA        # # # . . # # . . # # . . . . . 
    0x77,0xE0,    //    00AC        . # # # . # # # # # # . . . . . 
    0x3F,0x80,    //    00AE        . . # # # # # # # . . . . . . . 
    0x06,0x00,    //    00B0        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    00B2        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    00B4        . . . . . # # . . . . . . . . . 
    0x06,0x40,    //    00B6        . . . . . # # . . # . . . . . . 
    0x02,0xE0,    //    00B8        . . . . . . # . # # # . . . . . 
    0x03,0xE0,    //    00BA        . . . . . . # # # # # . . . . . 
    0x03,0xE0,    //    00BC        . . . . . . # # # # # . . . . . 
    0x06,0xC0,    //    00BE        . . . . . # # . # # . . . . . . 
    0x06,0x00,    //    00C0        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    00C2        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    00C4        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    00C6        . . . . . # # . . . . . . . . . 
    0x01,0xC0,    //    00C8        . . . . . . . # # # . . . . . . 
    0x06,0x00,    //    00CA        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    00CC        . . . . . # # . . . . . . . . . 
    0x03,0x00,    //    00CE        . . . . . . # # . . . . . . . . 
    0x03,0xC0,    //    00D0        . . . . . . # # # # . . . . . . 
    0x06,0x00,    //    00D2        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    00D4        . . . . . # # . . . . . . . . . 
    0x03,0x80,    //    00D6        . . . . . . # # # . . . . . . . 
    0x03,0x80,    //    00D8        . . . . . . # # # . . . . . . . 
    0x06,0x00,    //    00DA        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    00DC        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    00DE        . . . . . # # . . . . . . . . . 
    0x06,0x00,    //    00E0        . . . . . # # . . . . . . . . . 
    0x03,0xC0,    //    00E2        . . . . . . # # # # . . . . . . 
    0x07,0x60,    //    00E4        . . . . . # # # . # # . . . . . 
    0x06,0x60,    //    00E6        . . . . . # # . . # # . . . . . 
    0x06,0xE0,    //    00E8        . . . . . # # . # # # . . . . . 
    0x03,0xC0,    //    00EA        . . . . . . # # # # . . . . . . 
    0x00,0x00,    //    00EC        . . . . . . . . . . . . . . . . 
    0x00,0x00,    //    00EE        . . . . . . . . . . . . . . . . 
    0x00,0x00     //    00F0        . . . . . . . . . . . . . . . .   
    };
    
#define r OCR3BL 
#define g OCR3CL
#define b OCR1BL 

#define DATA1 PORTA 
#define DATA2 PORTC

void main()
{

DDRA=0xff;  //dataport
DDRC=0xff; //data port


DDRE=0x30;//OCR3B & OCR3C are output

DDRG=0x06;//buffers enable

DDRB=0x40;//OCR1B

// INT0: On
// INT0 Mode: Rising Edge
EICRA=0x03;
EICRB=0x00;
EIMSK=0x01;
EIFR=0x01;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 125.000 kHz
// Mode: Normal top=0xFF
// OC0 output: Disconnected
// Timer Period: 2.048 ms
ASSR=0<<AS0;
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (1<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0=0x00;



// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 1000.000 kHz
// Mode: Fast PWM top=OCR1A
// OC1A output: Disconnected
// OC1B output: Non-Inverted PWM
// OC1C output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer Period: 1 us
// Output Pulse(s):
// OC1B Period: 1 us
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (0<<COM1C1) | (0<<COM1C0) | (1<<WGM11) | (1<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;
OCR1CH=0x00;
OCR1CL=0x00;



// Timer/Counter 3 initialization
// Clock source: System Clock
// Clock value: 1000.000 kHz
// Mode: Fast PWM top=OCR3A
// OC3A output: Disconnected
// OC3B output: Non-Inverted PWM
// OC3C output: Non-Inverted
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer Period: 1 us
// Output Pulse(s):
// OC3B Period: 1 us
// OC3C Period: 1 us
// Timer3 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR3A=(0<<COM3A1) | (0<<COM3A0) | (1<<COM3B1) | (0<<COM3B0) | (1<<COM3C1) | (0<<COM3C0) | (1<<WGM31) | (1<<WGM30);
TCCR3B=(0<<ICNC3) | (0<<ICES3) | (1<<WGM33) | (1<<WGM32) | (0<<CS32) | (1<<CS31) | (0<<CS30);
TCNT3H=0x00;
TCNT3L=0x00;
ICR3H=0x00;
ICR3L=0x00;
OCR3AH=0x00;
OCR3AL=0x00;
OCR3BH=0x00;
OCR3BL=0x00;
OCR3CH=0x00;
OCR3CL=0x00;

OCR1A=OCR3AL=100;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (1<<TOIE0);
ETIMSK=(0<<TICIE3) | (0<<OCIE3A) | (0<<OCIE3B) | (0<<TOIE3) | (0<<OCIE3C) | (0<<OCIE1C);

DATA1=DATA2=0xff;
b=100;
#asm("sei")

    while(1)
    {
     PORTA=im[2*((TCNT0+(i<<8))/t)];
     PORTC=im[2*((TCNT0+(i<<8))/t)+1];
       
       r++;
       g++;
       b--;
       if(r==100) {r=g=0;b=100;}
       
    }
}
