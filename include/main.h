
#define CMD_FUNC_DEF(f)  int f(int argc, const char*const argv[])

typedef CMD_FUNC_DEF((*CMD_FUNC));

typedef struct {
	const char* name;
	CMD_FUNC func;
	//const char* usage;
} CMD;


CMD_FUNC_DEF( cmd_help );
CMD_FUNC_DEF( cmd_mem );
CMD_FUNC_DEF( cmd_word );
CMD_FUNC_DEF( cmd_config );
CMD_FUNC_DEF( cmd_ping );
CMD_FUNC_DEF( cmd_tftp );
CMD_FUNC_DEF( cmd_go );

unsigned long random();
