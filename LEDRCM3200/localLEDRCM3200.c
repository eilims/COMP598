#class auto

#define PORTA_AUX_IO

#use rcm3200.lib

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 7
#define USE_RABBITWEB 1

/*
 * TCP/IP modification - reduce TCP socket buffer
 * size, to allow more connections. This can be increased,
 * with increased performance, if the number of sockets
 * are reduced.  Note that this buffer size is split in
 * two for TCP sockets--1024 bytes for send and 1024 bytes
 * for receive.
 */
#define TCP_BUF_SIZE 2048

/*
 * Web server configuration
 */

/*
 * Define the number of HTTP servers and socket buffers.  Note
 * that we need one TCP socket for SMTP support. With
 * tcp_reserveport(), fewer HTTP servers are needed.
 */
#define HTTP_MAXSERVERS 2
#define MAX_TCP_SOCKET_BUFFERS 3

#define REDIRECTHOST "192.168.0.104"

#define OFF 0
#define ON 1

#define LIGHT_LEFT_NS 1
#define LIGHT_RIGHT_NS 2
#define LIGHT_LEFT_EW 3
#define LIGHT_RIGHT_EW 4

#define REDIRECTTO      "http://" REDIRECTHOST ""

#memmap xmem

#use "dcrtcp.lib"
#use "http.lib"

#ximport "C:/Users/db217620/Repositories/COMP598/LEDRCM3200/testSendNumber.html"   index_html



//BEGIN DEFINITIONS

/* the default for / must be first */
const HttpType http_types[] =
{
   { ".shtml", "text/html", zhtml_handler}, // ssi
   { ".html", "text/html", NULL},           // html
   { ".cgi", "", NULL},                     // cgi
   { ".gif", "image/gif", NULL}
};

typedef struct {
	char *name;
	char *value;
} FORMType;

enum directions{
    NS = 0,
    EW = 1
};

//END DEFINITIONS


//BEGIN GLOBAL VARIABLES
FORMType FORMSpec;
fontInfo fi6x8, fi8x10;
int currentSpeed;
#web currentSpeed;
//END GLOBAL VARIABLES

int parsePost(HttpState *state){
    int retval;
    retval = sock_aread(&state->s, state->p, (state->content_length < TCP_BUF_SIZE-1)?(int)state->content_length:TCP_BUF_SIZE-1);

    if(retval < 0) return 1;

    state->subsubstate += retval;

    if (state->subsubstate >= state->content_length) {
        state->buffer[(int)state->content_length] = '\0';
        http_scanpost(FORMSpec.name, state->buffer, FORMSpec.value, 20);
        return 1;
    }
    return 0;
}

int Submit(HttpState* state){
	if (state->length) {
		/* buffer to write out */
		if (state->offset < state->length) {
			state->offset += sock_fastwrite(&state->s,
					state->buffer + (int)state->offset,
					(int)state->length - (int)state->offset);
		} else {
			state->offset = 0;
			state->length = 0;
		}
    } else {
        FORMSpec.value[0] = '\0';

        state->p = state->buffer;
        if(parsePost(state)){
            currentSpeed = atoi(FORMSpec.value);
        }
    }
    state->substate = 0;
    cgi_redirectto(state,REDIRECTTO);
    return 0;
}

void stopSignal(int direction){
    if(direction == 0){
        dispLedOut(LIGHT_LEFT_NS, 0);
        dispLedOut(LIGHT_RIGHT_NS, 0);
    } else if(direction == 1){
        dispLedOut(LIGHT_LEFT_EW, 0);
        dispLedOut(LIGHT_RIGHT_EW, 0);
    }
}

void yellowSignal(int direction){
    if(direction == 0){
        dispLedOut(LIGHT_LEFT_NS, 0);
        dispLedOut(LIGHT_RIGHT_NS, 1);
    } else if(direction == 1){
        dispLedOut(LIGHT_LEFT_EW, 0);
        dispLedOut(LIGHT_RIGHT_EW, 1);
    }
}

void greenSignal(int direction){
    if(direction == 0){
        dispLedOut(LIGHT_LEFT_NS, 1);
        dispLedOut(LIGHT_RIGHT_NS, 1);
    } else if(direction == 1){
        dispLedOut(LIGHT_LEFT_EW, 1);
        dispLedOut(LIGHT_RIGHT_EW, 1);
    }
}

void advancedGreenSignal(int direction){
    if(direction == 0){
        dispLedOut(LIGHT_LEFT_NS, 1);
        dispLedOut(LIGHT_RIGHT_NS, 0);
    } else if(direction == 1){
        dispLedOut(LIGHT_LEFT_EW, 1);
        dispLedOut(LIGHT_RIGHT_EW, 0);
    }
}




//Required to be defined before main due to requirements for function callbacks
const HttpSpec http_flashspec[] =
{
   { HTTPSPEC_FILE, 		"/",				index_html,    NULL, 	0, NULL, NULL},
   { HTTPSPEC_FILE, 		"/index.html",	index_html,    NULL, 	0, NULL, NULL},
   { HTTPSPEC_FUNCTION,	"/parsenumber.cgi",	0,  		Submit,	0, NULL, NULL}
};

void main(){

    int currentState;
    int stopped;
    int running;
    char data[20];

    //Inital values for data packet -- this must be done
    FORMSpec.name = "numerical_input";
    FORMSpec.value = data;
    currentSpeed = 500;

    //Initial states for variables
    currentState = 0;
    stopped = NS;
    running = EW;

    //init system
    brdInit();
    dispInit();

    sock_init();
    http_init();
    tcp_reserveport(80);

    glBackLight(1);
	 glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);          // Initialize 6x8 font
	 glXFontInit(&fi8x10, 8, 10, 32, 127, Font8x10);			//	initialize 8x10 font
	 glBlankScreen();

    glPrintf (0,  0, &fi6x8,  "DO NOT DOUBT");
    glPrintf (0,  8, &fi6x8,  "MY POWER");
    glPrintf (0,  16, &fi6x8,  "Hi Ed");

    for(;;){
        http_handler();
        costate{
        switch(currentState){
            case 0:
				    stopSignal(stopped);
					 advancedGreenSignal(running);
					 break;
			   case 1:
                stopSignal(stopped);
					 greenSignal(running);
					 break;
			   case 2:
				    stopSignal(stopped);
					 yellowSignal(running);
					 break;
				case 3:
				    stopSignal(stopped);
					 stopSignal(running);
					 break;
        }

				if(currentState == 3){
				    if(stopped == NS){
					     stopped = EW;
						  running = NS;
					 } else {
						  stopped = NS;
						  running = EW;
					 }
				}
		  		currentState = (currentState + 1) % 4;
            //printf("%d\n", currentState);
            waitfor(DelayMs(currentSpeed));
        }
    }
}