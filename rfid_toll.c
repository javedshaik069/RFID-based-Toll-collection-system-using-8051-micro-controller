// required libraries for C code
#include<reg51.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

// port for LCD display
#define lcdport P1 

// defining ports for variables
sbit rs=P1^0;			// LCD rs = 1 then data register mode. if 0, command register mode (D0-D7)
sbit rw=P1^1;			// LCD rw = 0 then write mode. else read mode
sbit en=P1^2;			// enable pin of lcd
sbit m1=P2^4;			// m1 pin of motor driver L293D (IN1)
sbit m2=P2^5;			// m2 pin of motor driver L293D (IN2)
sbit trg = P2^6;	// 
sbit buz = P2^3;	// 

// data types for data transfer receiving
char i,rx_data[50];		
// data types for RFID
char rfid[13],ch=0,ch1=0;
// count for balance
int count1 = 1000, count2 = 100, count3 = 500;
unsigned char result[5]; 

// function to provide time delay
void del(int itime)  
{
	int i,j;
	for(i=0,i<itime,i++)		
		for(j=0,j<1275,j++); // 1ms delay
}

// data enable function
void daten()  
{
	rs=1;	// select data register mode
	rw=0; // write mode
	en=1; 
	del(5); //5ms delay
	en=0;
}

// Function for displaying single character data
void lcdd(unsigned char ch)
{
	lcdport=ch & 0xf0;
	daten();
	lcdport=(ch<<4) & 0xf0;
	daten();
}

// Function for LCD command initialization
void cmden(void) 
{
	rs=0;		// select command register mode
	en=1;
	del(5);
	en=0;
	
}

// Function for LCD command
void lcdm(unsigned char ch)
{
	lcdport=ch & 0xf0;
	cmden();
	lcdport=(ch<<4) & 0xf0;
	cmden();
}

// function to display string
void lcds(char *str)
{
	while(*str)
	{
		lcdd(*str); //displaye current character which is pointed by *str pointer variable
		str++;
	}
}

// initialize LCD function
void lcd_init(void)
{
	lcdm(0x02);		// return home
	lcdm(0x28);		// 4 bit, 2 lines with character shaping in 5x7 matrix
	lcdm(0x0e);		// display on, cursor blinking off
	lcdm(0x01);		// clear display screen
}

// function for receiving we are monitoring the SBUF register for receiving data
void uart_init()
{
	TMOD=0x20;  	// Timer 1, mo0d e 2(auto-reload}
	SCON=0x50;		// 8-bit, 1 stop, REN enabled
	TH1=0xfd;			// baud rate 9600
	TR1=1;				// start timer
}

// function for receiving data  
char rxdata()
{
	while(!RI);		//RI (receive interrupt) while loop runs when RI = 0
	ch=SBUF;			// sbuf character value is sent to ch variable		
	RI=0;					// flag is made 0
	return ch			
}

void main()
{
	buz = 0;
	uart_init();  					// set baud rate
	lcd_init();							// set required modes for LCD screen
	lcds(" RFID Based "); 	// displaying welcome message 
	lcdm(0x0c); 						// second line of lcd screen
	lcds("Toll Tax System");
	del(400);								// 400ms
	
	while(1)							//infinte loop
	{
		lcdm(1);											// clear screen
		lcds("Place Your Card:");			// request to place RFID card
		lcdm(0xc0);										// next line
		i=0;
		for(i=0;i<12;i++)
		{
			rfid[i]=rxdata();
		}
		// i=13 after coming out from loop
		rfid[i]='\0';  //null character
		
		
		lcdm(1);
		lcds("Your ID No.-");	
		lcdm(0xc0);
		for(i=0;i<12;i++)
		lcdd(rfid[i]);
		del(100);
		
		// compare with the existing reference
		
		// Case 1
		if(strncmp(rfid,"19004307673A",12)==0)
		{
			count1 = count1 - 50;  
			sprintf(result, "%d",count1);
			lcdm(1);
			lcds(" Amount ");
			lcdm(0xc0)
			lcds(" Debited: Rs.50");
			del(200);
			lcdm(1);
			
			lcds(" Dinkar ");
			del(200);
			lcdm(1);
			lcds("Remaining "); // displaying remaining balance
			lcdm(0xc0);
			lcds(" Balance:");
			lcds(result);
			
			del(10);
					// checking for sufficient funds and displaying recharge //message
					if(count1<100)
					{
						lcds("please");
						lcdm(0xc0);
						lcds("recharge");
					}
					del(200);
					
					lcdm(1);
					buz = 1; //buzzer is switched on
					m1=1;
					m2=0;
					del(300); 
					m1=0;
					m2=0;
					del(200);
					
					while(trg == 0){		// manual buzzer triggering
						buz = 1;}
					m1=0;
					m2=1;
					
		}
		
		// comparing with reference 2
		else if(strncmp(rfid,"19004362F6CE",12)==0 )
			{
				count2 = count2 - 50;
				sprintf(result, "%d",count2);
				lcdm(1);
				lcds("Amount");
				lcdm(0xc0);
				lcds("Debited: Rs.50");// display amount debited
				del(200);
				lcdm(1);
				lcds(" Javed ");
				del(200);
				lcdm(1);
				lcds("Remaining ");// displaying remaining balance
				lcdm(0xc0);
				lcds(" Balance:");
				lcds(result);
				del(10);
				lcdm(1);

				if(count2<100){ // checking for sufficient funds
				lcds("please");
				lcdm(0xc0);
				lcds("recharge");}
				buz = 1;
				m1=1;
				m2=0;
				del(300);
				buz = 0;
				m1=0;
				m2=0;
				del(200);
				while(trg == 0)
				buz = 1;
				m1=0;
				m2=1;
				del(300);
				buz = 0;
				m1=0;
				m2=0;
			}
			
		else if(strncmp(rfid,"19004362D6C2",12)==0 ) // comparing with the //references
			{
				count3 = count3-50;
				sprintf(result, "%d",count3);
				lcdm(1);
				lcds("Amount");
				lcdm(0xc0);
				lcds("Debited: Rs.50");
				del(200);
				lcdm(1);
				lcds(" Praneeth ");
				del(200);
				lcdm(1);
				lcds("Remaining");
				lcdm(0xc0);
				lcds("Balance:");
				lcds(result);
				del(10);
				if(count3<100){
				lcds("please");
				lcdm(0xc0);
				lcds("recharge");}
				buz = 1;
				m1=1;
				m2=0;
				del(300);
				buz = 0;
				m1=0;
				m2=0;
				del(200);
				while(trg == 0)
				buz = 1;
				m1=0;
				m2=1;
				del(300);
				buz = 0;
				m1=0;
				m2=0;
			}
		else
			{
				lcdm(1);
				lcds("Invalid Card"); // displaying for invalid credentials
			}
		
		}
}