/* response R1 bit defination */
#define OUT_OF_RANGE 	(1<<31)
#define ADDRESS_ERROR 	(1<<30)
#define BLOCK_LEN_ERRO 	(1<<29)
#define ERASE_SEQ_ERRO 	(1<<28)
#define ERASE_PARAM 	(1<<27)
#define WP_VIOLATION 	(1<<26)
#define CARD_IS_LOCKED	(1<<25)
#define LOCK_UNLOCK_FA	(1<<24)
#define COM_CRC_ERROR 	(1<<23)
#define ILLEGAL_COMMAN	(1<<22)
#define CARD_ECC_FAILE	(1<<21)
#define CC_ERROR 	(1<<20)
#define ERROR		(1<<19)
#define CSD_OVERWRITE 	(1<<16)
#define WP_ERASE_SKIP 	(1<<15)
#define CARD_ECC_DISAB	(1<<14)
#define ERASE_RESET  	(1<<13)
/*12:9*/ 
#define CURRENT_STATE_MASK (0xF<<9) 
#define CURRENT_STATE_IDLE (0<<9) 
#define CURRENT_STATE_READY (1<<9) 
#define CURRENT_STATE_IDENT (2<<9) 
#define CURRENT_STATE_STBY (3<<9) 
#define CURRENT_STATE_TRAN (4<<9) 
#define CURRENT_STATE_DATA (5<<9) 
#define CURRENT_STATE_RCV (6<<9) 
#define CURRENT_STATE_PRG (7<<9) 
#define CURRENT_STATE_DIS (8<<9) 

#define READY_FOR_DATA 	(1<<8)
#define APP_CMD 	(1<<5)
#define AKE_SEQ_ERROR 	(1<<3)

void sd_init( void );
int sd_cmd( unsigned int id, unsigned int argument, unsigned int * response );
int sd_probe( void );
int sd_read( int addr, unsigned int len, unsigned char * buf);
