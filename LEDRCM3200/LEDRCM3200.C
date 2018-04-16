#class auto

#define PORTA_AUX_IO

#memmap xmem
#use rcm3200.lib

#define OFF 0
#define ON 1

#define LIGHT_LEFT_NS 1
#define LIGHT_RIGHT_NS 2
#define LIGHT_LEFT_EW 3
#define LIGHT_RIGHT_EW 4

fontInfo fi6x8, fi8x10;

enum directions{
    NS = 0,
    EW = 1
};

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

void main(){

    static int currentState;
    static int stopped;
    static int running;
    static int delayTime;

    currentState = 0;
    delayTime = 500;
    stopped = NS;
    running = EW;

    brdInit();
    dispInit();

    glBackLight(1);
	 glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);          // Initialize 6x8 font
	 glXFontInit(&fi8x10, 8, 10, 32, 127, Font8x10);			//	initialize 8x10 font
	 glBlankScreen();

    glPrintf (0,  0, &fi6x8,  "DO NOT DOUBT");
    glPrintf (0,  8, &fi6x8,  "MY POWER");
    glPrintf (0,  16, &fi6x8,  "Hi Ed");

    for(;;){
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
            printf("%d\n", currentState);
            waitfor(DelayMs(delayTime));
        }
    }
}