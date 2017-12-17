#include "stdafx.h"


#define PM_SIZE 1536 /* Max: 144KB/3/32=1536 PM rows for 30F. */
#define EE_SIZE 128 /* 4KB/2/16=128 EE rows */
#define CM_SIZE 8




HANDLE OpenConnection (HANDLE *pComDev,  char *pPortName, char *pBaudRate);
BOOL   WriteCommBlock (HANDLE *pComdDev, char *pBuffer ,  int BytesToWrite);
int    ReadCommBlock  (HANDLE *pComdDev, char *pBuffer,   int MaxLength);
BOOL   CloseConnection(HANDLE *pComdDev);

BOOL    ReceiveData(HANDLE *pComDev, char * pBuffer, int BytesToReceive);
void    PrintUsage(void);
eFamily ReadID(HANDLE *pComDev);
void    ReadPM(HANDLE *pComDev, char * pReadPMAddress, eFamily Family);
void    ReadEE(HANDLE *pComDev, char * pReadEEAddress, eFamily Family);
void    SendHexFile(HANDLE *pComDev, FILE * pFile, eFamily Family);

sDevice Device[] = 
{
	{"dsPIC30F2010",      0x040, 1, dsPIC30F},
	{"dsPIC30F2011",      0x0C0, 1, dsPIC30F},
	{"dsPIC30F2011",      0x240, 1, dsPIC30F},
	{"dsPIC30F2012",      0x0C2, 1, dsPIC30F},
	{"dsPIC30F2012",      0x241, 1, dsPIC30F},
	{"dsPIC30F3010",      0x1C0, 1, dsPIC30F},
	{"dsPIC30F3011",      0x1C1, 1, dsPIC30F},
	{"dsPIC30F3012",      0x0C1, 1, dsPIC30F},
	{"dsPIC30F3013",      0x0C3, 1, dsPIC30F},
	{"dsPIC30F3014",      0x160, 1, dsPIC30F},
	{"dsPIC30F4011",      0x101, 1, dsPIC30F},
	{"dsPIC30F4012",      0x100, 1, dsPIC30F},
	{"dsPIC30F4013",      0x141, 1, dsPIC30F},
	{"dsPIC30F5011",      0x080, 1, dsPIC30F},
	{"dsPIC30F5013",      0x081, 1, dsPIC30F},
	{"dsPIC30F5015",      0x200, 1, dsPIC30F},
	{"dsPIC30F5016",      0x201, 1, dsPIC30F},
	{"dsPIC30F6010",      0x188, 1, dsPIC30F},
	{"dsPIC30F6010A",     0x281, 1, dsPIC30F},
	{"dsPIC30F6011",      0x192, 1, dsPIC30F},
	{"dsPIC30F6011A",     0x2C0, 1, dsPIC30F},
	{"dsPIC30F6012",      0x193, 1, dsPIC30F},
	{"dsPIC30F6012A",     0x2C2, 1, dsPIC30F},
	{"dsPIC30F6013",      0x197, 1, dsPIC30F},
	{"dsPIC30F6013A",     0x2C1, 1, dsPIC30F},
	{"dsPIC30F6014",      0x198, 1, dsPIC30F},
	{"dsPIC30F6014A",     0x2C3, 1, dsPIC30F},
	{"dsPIC30F6015",      0x280, 1, dsPIC30F},

	{"dsPIC33FJ64GP206",  0xC1, 3, dsPIC33F},
	{"dsPIC33FJ64GP306",  0xCD, 3, dsPIC33F},
	{"dsPIC33FJ64GP310",  0xCF, 3, dsPIC33F},
	{"dsPIC33FJ64GP706",  0xD5, 3, dsPIC33F},
	{"dsPIC33FJ64GP708",  0xD6, 3, dsPIC33F},
	{"dsPIC33FJ64GP710",  0xD7, 3, dsPIC33F},
	{"dsPIC33FJ128GP206", 0xD9, 3, dsPIC33F},
	{"dsPIC33FJ128GP306", 0xE5, 3, dsPIC33F},
	{"dsPIC33FJ128GP310", 0xE7, 3, dsPIC33F},
	{"dsPIC33FJ128GP706", 0xED, 3, dsPIC33F},
	{"dsPIC33FJ128GP708", 0xEE, 3, dsPIC33F},
	{"dsPIC33FJ128GP710", 0xEF, 3, dsPIC33F},
	{"dsPIC33FJ256GP506", 0xF5, 3, dsPIC33F},
	{"dsPIC33FJ256GP510", 0xF7, 3, dsPIC33F},
	{"dsPIC33FJ256GP710", 0xFF, 3, dsPIC33F},
	{"dsPIC33FJ64MC506",  0x89, 3, dsPIC33F},
	{"dsPIC33FJ64MC508",  0x8A, 3, dsPIC33F},
	{"dsPIC33FJ64MC510",  0x8B, 3, dsPIC33F},
	{"dsPIC33FJ64MC706",  0x91, 3, dsPIC33F},
	{"dsPIC33FJ64MC710",  0x97, 3, dsPIC33F},
	{"dsPIC33FJ128MC506", 0xA1, 3, dsPIC33F},
	{"dsPIC33FJ128MC510", 0xA3, 3, dsPIC33F},
	{"dsPIC33FJ128MC706", 0xA9, 3, dsPIC33F},
	{"dsPIC33FJ128MC708", 0xAE, 3, dsPIC33F},
	{"dsPIC33FJ128MC710", 0xAF, 3, dsPIC33F},
	{"dsPIC33FJ256MC510", 0xB7, 3, dsPIC33F},
	{"dsPIC33FJ256MC710", 0xBF, 3, dsPIC33F},

	{"dsPIC33FJ12GP201", 0x802, 3, dsPIC33F},
	{"dsPIC33FJ12GP202", 0x803, 3, dsPIC33F},
	{"dsPIC33FJ12MC201", 0x800, 3, dsPIC33F},
	{"dsPIC33FJ12MC202", 0x801, 3, dsPIC33F},

	{"dsPIC33FJ32GP204", 0xF0F, 3, dsPIC33F},
	{"dsPIC33FJ32GP202", 0xF0D, 3, dsPIC33F},
	{"dsPIC33FJ16GP304", 0xF07, 3, dsPIC33F},
	{"dsPIC33FJ32MC204", 0xF0B, 3, dsPIC33F},
	{"dsPIC33FJ32MC202", 0xF09, 3, dsPIC33F},
	{"dsPIC33FJ16MC304", 0xF03, 3, dsPIC33F},

	{"dsPIC33FJ128GP804", 0x62F, 3, dsPIC33F},
	{"dsPIC33FJ128GP802", 0x62D, 3, dsPIC33F},
	{"dsPIC33FJ128GP204", 0x627, 3, dsPIC33F},
	{"dsPIC33FJ128GP202", 0x625, 3, dsPIC33F},
	{"dsPIC33FJ64GP804",  0x61F, 3, dsPIC33F},
	{"dsPIC33FJ64GP802",  0x61D, 3, dsPIC33F},
	{"dsPIC33FJ64GP204",  0x617, 3, dsPIC33F},
	{"dsPIC33FJ64GP202",  0x615, 3, dsPIC33F},
	{"dsPIC33FJ32GP304",  0x607, 3, dsPIC33F},
	{"dsPIC33FJ32GP302",  0x605, 3, dsPIC33F},
	{"dsPIC33FJ128MC804", 0x62B, 3, dsPIC33F},
	{"dsPIC33FJ128MC802", 0x629, 3, dsPIC33F},
	{"dsPIC33FJ128MC204", 0x623, 3, dsPIC33F},
	{"dsPIC33FJ128MC202", 0x621, 3, dsPIC33F},
	{"dsPIC33FJ64MC804",  0x61B, 3, dsPIC33F},
	{"dsPIC33FJ64MC802",  0x619, 3, dsPIC33F},
	{"dsPIC33FJ64MC204",  0x613, 3, dsPIC33F},
	{"dsPIC33FJ64MC202",  0x611, 3, dsPIC33F},
	{"dsPIC33FJ32MC304",  0x603, 3, dsPIC33F},
	{"dsPIC33FJ32MC302",  0x601, 3, dsPIC33F},

	{"dsPIC33FJ06GS101",  0xC00, 3, dsPIC33F},
	{"dsPIC33FJ06GS102",  0xC01, 3, dsPIC33F},
	{"dsPIC33FJ06GS202",  0xC02, 3, dsPIC33F},
	{"dsPIC33FJ16GS402",  0xC04, 3, dsPIC33F},
	{"dsPIC33FJ16GS404",  0xC06, 3, dsPIC33F},
	{"dsPIC33FJ16GS502",  0xC03, 3, dsPIC33F},
	{"dsPIC33FJ16GS504",  0xC05, 3, dsPIC33F},

	{"PIC24HJ64GP206",    0x41, 3, PIC24H},
	{"PIC24HJ64GP210",    0x47, 3, PIC24H},
	{"PIC24HJ64GP506",    0x49, 3, PIC24H},
	{"PIC24HJ64GP510",    0x4B, 3, PIC24H},
	{"PIC24HJ128GP206",   0x5D, 3, PIC24H},
	{"PIC24HJ128GP210",   0x5F, 3, PIC24H},
	{"PIC24HJ128GP306",   0x65, 3, PIC24H},
	{"PIC24HJ128GP310",   0x67, 3, PIC24H},
	{"PIC24HJ128GP506",   0x61, 3, PIC24H},
	{"PIC24HJ128GP510",   0x63, 3, PIC24H},
	{"PIC24HJ256GP206",   0x71, 3, PIC24H},
	{"PIC24HJ256GP210",   0x73, 3, PIC24H},
	{"PIC24HJ256GP610",   0x7B, 3, PIC24H},

	{"PIC24HJ12GP201", 0x80A, 3, PIC24H},
	{"PIC24HJ12GP202", 0x80B, 3, PIC24H},

	{"PIC24HJ32GP204", 0xF1F, 3, PIC24H},
	{"PIC24HJ32GP202", 0xF1D, 3, PIC24H},
	{"PIC24HJ16GP304", 0xF17, 3, PIC24H},

	{"PIC24HJ128GP504", 0x67F, 3, PIC24H},
	{"PIC24HJ128GP502", 0x67D, 3, PIC24H},
	{"PIC24HJ128GP204", 0x667, 3, PIC24H},
	{"PIC24HJ128GP202", 0x665, 3, PIC24H},
	{"PIC24HJ64GP504",  0x677, 3, PIC24H},
	{"PIC24HJ64GP502",  0x675, 3, PIC24H},
	{"PIC24HJ64GP204",  0x657, 3, PIC24H},
	{"PIC24HJ64GP202",  0x655, 3, PIC24H},
	{"PIC24HJ32GP304",  0x647, 3, PIC24H},
	{"PIC24HJ32GP302",  0x645, 3, PIC24H},

	{"PIC24FJ64GA006",    0x405, 3, PIC24F},
	{"PIC24FJ64GA008",    0x408, 3, PIC24F},
	{"PIC24FJ64GA010",    0x40B, 3, PIC24F},
	{"PIC24FJ96GA006",    0x406, 3, PIC24F},
	{"PIC24FJ96GA008",    0x409, 3, PIC24F},
	{"PIC24FJ96GA010",    0x40C, 3, PIC24F},
	{"PIC24FJ128GA006",   0x407, 3, PIC24F},
	{"PIC24FJ128GA008",   0x40A, 3, PIC24F},
	{"PIC24FJ128GA010",   0x40D, 3, PIC24F},

	{NULL, 0, 0}
};

/******************************************************************************/
int _tmain(int argc, _TCHAR* argv[])
{
	try{
		printf("\nZX updater Ver 0.58.\n");
		HANDLE   ComDev ;
		cmd_cCmd ProgCommand(argv, "i:b:p:e:");
		char *   pInterfaceName = NULL;
		char *   pReadPMAddress = NULL;
		char *   pReadEEAddress = NULL;
		char *   pBaudRate      = "115200";
		FILE *   pFile          = NULL;
		eFamily  Family;

		while (ProgCommand.Next())
		{
			switch (ProgCommand.Option())
			{
			case 'i': /* Interface option: COM1, COM2, etc */
				if (ProgCommand.Arg() == NULL)
				{
					printf("\n-i requires argument\n");
					PrintUsage();
					return 0;
				}
				else
				{
					pInterfaceName = ProgCommand.Arg();
				}

				break;

			case 'b': /* Baudrate */
				if (ProgCommand.Arg() == NULL)
				{
					printf("\n-b requires argument\n");
					PrintUsage();
					return 0;
				}
				else
				{
					pBaudRate = ProgCommand.Arg();
				}

				break;

			case 'p': /* Read Program Memory */
				if (ProgCommand.Arg() == NULL)
				{
					printf("\n-p requires argument\n");
					PrintUsage();
					return 0;
				}
				else
				{
					pReadPMAddress = ProgCommand.Arg();
					assert(pReadPMAddress[0] == '0' && pReadPMAddress[1] =='x');
					assert(isxdigit(pReadPMAddress[2]));
					assert(isxdigit(pReadPMAddress[3]));
					assert(isxdigit(pReadPMAddress[4]));
					assert(isxdigit(pReadPMAddress[5]));
					assert(isxdigit(pReadPMAddress[6]));
					assert(isxdigit(pReadPMAddress[7]));
				}

				break;

			case 'e': /* Read EEPROM Memory */
				if (ProgCommand.Arg() == NULL)
				{
					printf("\n-e requires argument\n");
					PrintUsage();
					return 0;
				}
				else
				{
					pReadEEAddress = ProgCommand.Arg();
					assert(pReadEEAddress[0] == '0' && pReadEEAddress[1] =='x');
					assert(isxdigit(pReadEEAddress[2]));
					assert(isxdigit(pReadEEAddress[3]));
					assert(isxdigit(pReadEEAddress[4]));
					assert(isxdigit(pReadEEAddress[5]));
					assert(isxdigit(pReadEEAddress[6]));
					assert(isxdigit(pReadEEAddress[7]));
				}

				break;

			case '.':
				if ((pFile = fopen(ProgCommand.Arg(), "r")) == NULL)
				{
					printf("\nCan't open file: %s\n", ProgCommand.Arg());
					return 0;
				}

				break;

			default:
				printf("\nUnknown option `%c'\n", ProgCommand.Option());
				PrintUsage();
				return 0;
			}
		}

		if(pInterfaceName == NULL)
		{
			printf("\nPlease use -i option to specify interface name: COM1, COM2, etc...\n");
			PrintUsage();
			return 0;
		}



		assert(OpenConnection(&ComDev, pInterfaceName, pBaudRate) != NULL);

		/* Read Device ID */
		Family = ReadID(&ComDev);

		/* Process Read PM request and exit */
		if(pReadPMAddress != NULL)
		{
			ReadPM(&ComDev, pReadPMAddress, Family);
			return 0;
		}

		/* Process Read EEPROM request and exit */
		if(pReadEEAddress != NULL)
		{
			ReadEE(&ComDev, pReadEEAddress, Family);
			return 0;
		}

		/* Read Hex file and transfer it to target */
		if(pFile == NULL)
		{
			printf("\nPlease provide HEX file name to read\n");
			PrintUsage();
			return 0;
		}
		SendHexFile(&ComDev, pFile, Family);


		CloseConnection(&ComDev);

		return 0;
	}
	catch(int e)  // Consider using a custom exception type for intentional
	{                             // throws. A good idea might be a `return_exception`.
		printf("Error code %d\n",e);
	}
}
/******************************************************************************/
void SendHexFile(HANDLE *pComDev, FILE * pFile, eFamily Family)
{
	char Buffer[BUFFER_SIZE];
	int  ExtAddr = 0;

	/* Initialize Memory */
	mem_cMemRow ** ppMemory = (mem_cMemRow **)malloc(sizeof(mem_cMemRow *) * PM_SIZE + sizeof(mem_cMemRow *) * EE_SIZE + sizeof(mem_cMemRow *) * CM_SIZE);

	for(int Row = 0; Row < PM_SIZE; Row++)
	{
		ppMemory[Row] = new mem_cMemRow(mem_cMemRow::Program, 0x000000, Row, Family);
	}

	for(int Row = 0; Row < EE_SIZE; Row++)
	{
		ppMemory[Row + PM_SIZE] = new mem_cMemRow(mem_cMemRow::EEProm, 0x7FF000, Row, Family);
	}

	for(int Row = 0; Row < CM_SIZE; Row++)
	{
		ppMemory[Row + PM_SIZE + EE_SIZE] = new mem_cMemRow(mem_cMemRow::Configuration, 0xF80000, Row, Family);
	}

	printf("\nReading HexFile");

	while(fgets(Buffer, sizeof(Buffer), pFile) != NULL)
	{
		int ByteCount;
		int Address;
		int RecordType;

		sscanf(Buffer+1, "%2x%4x%2x", &ByteCount, &Address, &RecordType);

		if(RecordType == 0)
		{
			Address = (Address + ExtAddr) / 2;

			for(int CharCount = 0; CharCount < ByteCount*2; CharCount += 4, Address++)
			{
				bool bInserted = FALSE;

				for(int Row = 0; Row < (PM_SIZE + EE_SIZE + CM_SIZE); Row++)
				{
					if((bInserted = ppMemory[Row]->InsertData(Address, Buffer + 9 + CharCount)) == TRUE)
					{
						break;
					}
				}

				if(bInserted != TRUE)
				{
					printf("Bad Hex file: 0x%xAddress out of range\n", Address);
					assert(0);
				}
			}
		}
		else if(RecordType == 1)
		{
		}
		else if(RecordType == 4)
		{
			sscanf(Buffer+9, "%4x", &ExtAddr);

			ExtAddr = ExtAddr << 16;
		}
		else
		{
			assert(!"Unknown hex record type\n");
		}
	}

	/* Preserve first two locations for bootloader */
	{
		char Data[32];
		int          RowSize;

		if(Family == dsPIC30F)
		{
			RowSize = PM30F_ROW_SIZE;
		}
		else
		{
			RowSize = PM33F_ROW_SIZE;
		}

		Buffer[0] = COMMAND_READ_PM;
		Buffer[1] = 0x00;
		Buffer[2] = 0x00;
		Buffer[3] = 0x00;

		WriteCommBlock(pComDev, Buffer, 4);

		Sleep(100);

		printf("\nReading Target\n");
		BOOL doneRead=ReceiveData(pComDev, Buffer, RowSize * 3);
				if(!doneRead){
			printf("Timeout during waiting read [ZX].\n");
			CloseConnection(pComDev);
			throw 100;

		}

		sprintf(Data, "%02x%02x%02x00%02x%02x%02x00",   Buffer[2] & 0xFF,
			Buffer[1] & 0xFF,
			Buffer[0] & 0xFF,
			Buffer[5] & 0xFF,
			Buffer[4] & 0xFF,
			Buffer[3] & 0xFF);

		ppMemory[0]->InsertData(0x000000, Data);
		ppMemory[0]->InsertData(0x000001, Data + 4);
		ppMemory[0]->InsertData(0x000002, Data + 8);
		ppMemory[0]->InsertData(0x000003, Data + 12);
	}

	for(int Row = 0; Row < (PM_SIZE + EE_SIZE + CM_SIZE); Row++)
	{
		ppMemory[Row]->FormatData();
	}

	printf("\nProgramming Device ");

	for(int Row = 0; Row < (PM_SIZE + EE_SIZE + CM_SIZE); Row++)
	{
		ppMemory[Row]->SendData(pComDev);
	}


	Buffer[0] = COMMAND_RESET; //Reset target device

	WriteCommBlock(pComDev, Buffer, 1);

	Sleep(100);

	printf(" Done.\n");
}
/******************************************************************************/
eFamily ReadID(HANDLE *pComDev)
{
	char                Buffer[BUFFER_SIZE];
	unsigned short int  DeviceId = 0;
	unsigned short int  ProcessId = 0;

	Buffer[0] = COMMAND_READ_ID;

	printf("\nReading Target Device ID");

	WriteCommBlock(pComDev, Buffer, 1);

	BOOL doneRead=ReceiveData(pComDev, Buffer, 8);
			if(!doneRead){
			printf("Timeout during waiting read [ZX].\n");
			CloseConnection(pComDev);
			throw 100;

		}
			//for(int i=0;i<6;i++){
			//	printf("%X,",Buffer[i]);
			//}

	DeviceId  = ((Buffer[1] << 8)&0xFF00) | (Buffer[0]&0x00FF);
	ProcessId = (Buffer[5] >> 4) & 0x0F;

	


	int Count         = 0;
	bool bDeviceFound = FALSE;

	while(bDeviceFound != TRUE)
	{
		if(Device[Count].pName == NULL)
		{
			break;
		}

		if((Device[Count].Id == DeviceId) && (Device[Count].ProcessId == ProcessId))
		{
			bDeviceFound = TRUE;
			break;
		}

		Count++;
	}

	if(!bDeviceFound){
			printf("Error reading device ID.\n");
			CloseConnection(pComDev);
			throw 60;
	}

	printf("..   Found %s (ID: 0x%04x)\n", Device[Count].pName, DeviceId);

	return(Device[Count].Family);

}
/******************************************************************************/
void ReadPM(HANDLE *pComDev, char * pReadPMAddress, eFamily Family)
{
	int          Count;
	unsigned int ReadAddress;
	char         Buffer[BUFFER_SIZE];
	int          RowSize;

	if(Family == dsPIC30F)
	{
		RowSize = PM30F_ROW_SIZE;
	}
	else
	{
		RowSize = PM33F_ROW_SIZE;
	}

	sscanf(pReadPMAddress, "%x", &ReadAddress);

	ReadAddress = ReadAddress - ReadAddress % (RowSize * 2);

	Buffer[0] = COMMAND_READ_PM;
	Buffer[1] = ReadAddress & 0xFF;
	Buffer[2] = (ReadAddress >> 8) & 0xFF;
	Buffer[3] = (ReadAddress >> 16) & 0xFF;

	WriteCommBlock(pComDev, Buffer, 4);

	BOOL doneRead=ReceiveData(pComDev, Buffer, RowSize * 3);
			if(!doneRead){
			printf("Timeout during waiting read [ZX].\n");
			CloseConnection(pComDev);
			throw 100;

		}

	for(Count = 0; Count < RowSize * 3;)
	{
		printf("0x%06x: ", ReadAddress);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x ",Buffer[Count++] & 0xFF);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x ",Buffer[Count++] & 0xFF);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x ",Buffer[Count++] & 0xFF);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x\n",Buffer[Count++] & 0xFF);

		ReadAddress = ReadAddress + 8;
	}
}
/******************************************************************************/
void ReadEE(HANDLE *pComDev, char * pReadEEAddress, eFamily Family)
{
	int          Count;
	unsigned int ReadAddress;
	char         Buffer[BUFFER_SIZE];

	assert(Family != dsPIC33F);
	assert(Family != PIC24H);

	sscanf(pReadEEAddress, "%x", &ReadAddress);

	ReadAddress = ReadAddress - ReadAddress % (EE30F_ROW_SIZE * 2);

	Buffer[0] = COMMAND_READ_EE;
	Buffer[1] = ReadAddress & 0xFF;
	Buffer[2] = (ReadAddress >> 8) & 0xFF;
	Buffer[3] = (ReadAddress >> 16) & 0xFF;

	WriteCommBlock(pComDev, Buffer, 4);

	BOOL doneRead=ReceiveData(pComDev, Buffer, EE30F_ROW_SIZE * 2);
			if(!doneRead){
			printf("Timeout during waiting read [ZX].\n");
			CloseConnection(pComDev);
			throw 100;

		}

	for(Count = 0; Count < EE30F_ROW_SIZE * 2;)
	{
		printf("0x%06x: ", ReadAddress);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x ",Buffer[Count++] & 0xFF);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x ",Buffer[Count++] & 0xFF);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x ",Buffer[Count++] & 0xFF);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x ",Buffer[Count++] & 0xFF);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x ",Buffer[Count++] & 0xFF);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x ",Buffer[Count++] & 0xFF);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x ",Buffer[Count++] & 0xFF);

		printf("%02x",Buffer[Count++] & 0xFF);
		printf("%02x\n",Buffer[Count++] & 0xFF);

		ReadAddress = ReadAddress + 16;
	}
}
/******************************************************************************/
void PrintUsage(void)
{
	printf("\nUsage: \"16-Bit Flash Programmer.exe\" -i interface [-bpe] hexfile\n\n");
	printf("Options:\n\n");
	printf("  -i\n");
	printf("       specifies serial interface name such as COM1, COM2, etc\n\n");
	printf("  -b\n");
	printf("       specifies baudrate for serial interface. Default is 9600\n\n");
	printf("  -p\n");
	printf("       read program flash. Must provide address to read in HEX format: -p 0x000100\n\n");
	printf("  -e\n");
	printf("       read EEPROM. Must provide address to read in HEX format: -e 0x7FFC00\n\n");
}
/******************************************************************************/
BOOL ReceiveData(HANDLE *pComDev, char * pBuffer, int BytesToReceive)
{
int Size = 0;
	int counter=0;
	while(Size != BytesToReceive && counter<500)
	{
		Size += ReadCommBlock(pComDev, pBuffer + Size, BytesToReceive - Size);
		counter++;
	}
	//printf("Counter %d\n",counter);
	return counter<500;
}
/******************************************************************************/
BOOL WriteCommBlock(HANDLE * pComDev, char *pBuffer , int BytesToWrite)
{
	BOOL       bWriteStat   = 0;
	DWORD      BytesWritten = 0;
	COMSTAT    ComStat      = {0};
	OVERLAPPED osWrite      = {0,0,0};

	printf(".");

	if(WriteFile(*pComDev,pBuffer,BytesToWrite,&BytesWritten,&osWrite) == FALSE)
	{
		assert(GetLastError() == ERROR_IO_PENDING);
		return (FALSE);
	}

	return (TRUE);
}
/******************************************************************************/
int ReadCommBlock(HANDLE *pComDev, char * pBuffer, int MaxLength )
{
	DWORD      Length;
	COMSTAT    ComStat    = {0};
	DWORD      ErrorFlags = 0;
	OVERLAPPED osRead     = {0,0,0};

	/* only try to read number of bytes in queue */
	ClearCommError(*pComDev, &ErrorFlags, &ComStat);

	Length = min((DWORD)MaxLength, ComStat.cbInQue);

	if(Length > 0)
	{
		if(ReadFile(*pComDev, pBuffer, Length, &Length, &osRead) == FALSE)
		{
			Length = 0 ;

			ClearCommError(*pComDev, &ErrorFlags, &ComStat);

			assert(ErrorFlags == 0);
		}
	}
	else
	{
		Length = 0;
		Sleep(1);
	}

	return (Length);
}
/******************************************************************************/
HANDLE OpenConnection(HANDLE * pComDev, char * pPortName, char * pBaudRate)
{
	int BaudRate;
	COMMTIMEOUTS CommTimeOuts;
	DCB          Dcb;

	sscanf(pBaudRate, "%d", &BaudRate);
	

	*pComDev = CreateFile(pPortName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if(*pComDev == INVALID_HANDLE_VALUE || *pComDev == NULL){
		printf("Create file error %lu\n",GetLastError());
		throw 80;
	}

	//if(*pComDev == NULL)
	//{
	//	return (*pComDev);
	//}

	/* get any early notifications */
	if(!SetCommMask(*pComDev, EV_RXCHAR)){
		printf("Set Comm Mask error %lu\n",GetLastError());
		throw 90;
	}

	/* setup device buffers */
	if(!SetupComm(*pComDev, 10000, 10000)){
		printf("Setup Comm error %lu\n",GetLastError());
		throw 95;
	}

	/* purge any information in the buffer */
	if(!PurgeComm(*pComDev,  PURGE_TXABORT |
		PURGE_RXABORT |
		PURGE_TXCLEAR |
		PURGE_RXCLEAR)){
		printf("Setup Comm error %lu\n",GetLastError());
		throw 95;
	
	}


	//BOOL rtn=GetCommTimeouts(*pComDev, &CommTimeOuts);
	//printf("Get Com Timeout rtn %d.\n");

	//printf("%lu, %lu, %lu, %lu, %lu.\n ",CommTimeOuts.ReadIntervalTimeout,
	//CommTimeOuts.ReadTotalTimeoutMultiplier,
	//CommTimeOuts.ReadTotalTimeoutConstant,
	//CommTimeOuts.WriteTotalTimeoutMultiplier,
	//CommTimeOuts.WriteTotalTimeoutConstant);

	/* set up for overlapped I/O */
	CommTimeOuts.ReadIntervalTimeout         = MAXDWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier  = 0;
	CommTimeOuts.ReadTotalTimeoutConstant    = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 100;
	CommTimeOuts.WriteTotalTimeoutConstant   = 1000 ;






	
	if (!SetCommTimeouts(*pComDev, &CommTimeOuts)){
		printf("Set Comm Timeout error %lu\n",GetLastError());
		throw 101;
	}


	Dcb.DCBlength = sizeof(DCB);

	if(!GetCommState(*pComDev, &Dcb)){
		printf("Get Comm State Error %lu\n",GetLastError());
		throw 102;
	}

	Dcb.BaudRate     = BaudRate;
	Dcb.ByteSize     = BYTESIZE;
	Dcb.Parity       = PARITY;
	Dcb.StopBits     = STOPBITS;
	Dcb.fOutxDsrFlow = FALSE;
	Dcb.fDtrControl  = DTR_CONTROL_DISABLE;
	Dcb.fOutxCtsFlow = FALSE ;
	Dcb.fRtsControl  = RTS_CONTROL_DISABLE;
	Dcb.fInX         = FALSE;
	Dcb.fOutX        = FALSE;
	//Dcb.XonChar      = ASCII_XON ;
	//Dcb.XoffChar     = ASCII_XOFF ;
	Dcb.XonLim       = 0x800;
	Dcb.XoffLim      = 0x200;
	Dcb.fBinary      = TRUE ;
	Dcb.fParity      = TRUE ;

	assert(SetCommState(*pComDev, &Dcb) == TRUE);

	return (*pComDev);
}
/******************************************************************************/
BOOL CloseConnection(HANDLE * pComDev)
{
	/* purge any outstanding reads/writes and close device handle */
	PurgeComm(*pComDev,  PURGE_TXABORT |
		PURGE_RXABORT |
		PURGE_TXCLEAR |
		PURGE_RXCLEAR);

	return(CloseHandle(*pComDev));
}
