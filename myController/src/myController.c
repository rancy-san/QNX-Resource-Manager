#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/resmgr.h>
#include <string.h>

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL
#define ATTACH_POINT "mydevice"

typedef union {
	// from neutrino.
	struct _pulse pulse;
	char msg[255];
} my_message_t;

int main(void) {
	// from dispatch.h
	name_attach_t *attach;
	// mydevice file
	FILE *file;
	// store status of the device
	char status[255];
	// store value of the device
	char value[255];
	// store value of string comparison
	int match;
	// store receive ID of pulse
	int rcvid;
	// my_message_t struct
	my_message_t msg;

	/* Create a local name (/dev/name/local/...) */
	attach = name_attach(NULL, ATTACH_POINT, 0);

	if(attach == NULL)
		return EXIT_FAILURE;

	file = fopen("/dev/local/mydevice", "r+");

	fscanf(file, "%s%s", status, value);

	// get value of string comparison
	match = strcmp(status, "status");

	if(match == 0) {
		printf("Status: %s\n", value);
		match = strcmp(value, "closed");

		if(match == 0) {
			// has chid on attach
			// from iofunc.h, dispatch.h
			// int name_detach( name_attach_t * attach, unsigned flags );
			name_detach(attach, 0);
			fclose(file);
			return EXIT_SUCCESS;
		}
	}

	for(;;) {
		// from neutrino.h
		// int MsgReceive( int chid, void * msg, int bytes, struct _msg_info * info );
		rcvid = MsgReceivePulse(attach->chid, &msg, sizeof(msg), NULL);

		if(rcvid == 0) {
			/*
			 // inside a pulse. from neutrino.h

			    struct _pulse {
			    uint16_t        type;
			    uint16_t        subtype;
			    int8_t          code;
			    uint8_t         zero [3];
			    union sigval    value;
			    int32_t         scoid;
			};
			 */
			// A safe range of pulse values is _PULSE_CODE_MINAVAIL through _PULSE_CODE_MAXAVAIL.
			switch(msg.pulse.code){
			case MY_PULSE_CODE: {
				/*
				 union sigval {
    				int     sival_int;
    				void    *sival_ptr;
				};
				 */
				printf("Small integer: %d\n", msg.pulse.value.sival_int);
				file = fopen("/dev/local/mydevice", "r+");
				fscanf(file, "%s%s", status, value);
				match = strcmp(status, "status");

				if(match == 0) {
					match = strcmp(value, "closed");

					if(match == 0) {
						printf("Status: %s\n", value);
						// from iofunc.h, dispatch.h
						// int name_detach( name_attach_t * attach, unsigned flags );
						name_detach(attach, 0);
						fclose(file);
						return EXIT_SUCCESS;
					} else {
						printf("Status: %s\n", value);
					}
				}

				MsgReply(rcvid, EOK, 0, 0);

				break;
			}
			default: {
				printf("Switch failed.");
				return EXIT_FAILURE;
				break;
			}
			}


			MsgReply(rcvid, EOK, 0, 0);

		} else {
			printf("Message receive failed.");
			return EXIT_FAILURE;
		}

		MsgReply(rcvid, EOK, 0, 0);
	}

	name_detach(attach, 0);

	return EXIT_SUCCESS;
}
