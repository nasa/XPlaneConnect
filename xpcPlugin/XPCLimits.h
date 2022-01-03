#define XPC_MAX_MESSAGE_SIZE	16384	// Max message size in Message.h; was 4096
#define XPC_MAX_DREF_VALUES		  255 	// Max dref elements, must fit in single byte
#define XPC_MAX_DREF_COUNT		  255   // Max number of drefs requested per message, encoded as single byte
#define XPC_MAX_COLS 			  134 	// Max number of columns in	data array, MessageHandlers