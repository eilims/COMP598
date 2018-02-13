include <stdio.h>
#include <ncurses.h>

#include <ev3.h>
#include <ev3_port.h>
#include <ev3_tacho.h>

int main(int argc, char** argv)
{
   //control variables
   int i = 0;
   char s[256];


   //Enabling ev3 setup
   printf("Welcome to Crane Game!\n");
   printf("Currently Setting and checking for all motors!\n");

   //Check if we are connected to a brick
   int returnValue = ev3_init();
   if(returnValue == 0)
     {
        printf("No brick found sorry!\n");
        return 1;
     }
   else if(returnValue == 1)
     {
        printf("Brick found continuing setup!\n");
     }
   else
     {
        printf("An error has occured detecting the brick!\n");
        return 2;
     }

   //Get all ports
   //Should display 8
   returnValue = ev3_port_init();
   if(returnValue == -1)
     {
        printf("An error has occured detecting the ports!\n");
        return 3;
     }
   printf("There are %d ports detected\n", returnValue);

   //All relevant motors
   //Should display 3
   returnValue = ev3_tacho_init();
   if(returnValue == -1)
     {
        printf("An error has occured detecting the tacho motors!\n");
        return 5;
     }
   printf("There are %d tacho motors detected\n", returnValue);

   //Check which ports have the correct motors
   for ( i = 0; i < DESC_LIMIT; i++ )
     {
        if ( ev3_tacho[ i ].type_inx != TACHO_TYPE__NONE_ )
          {
             printf( " Sequence Number: %d\n", i);
             printf( "  type = %s\n", ev3_tacho_type( ev3_tacho[ i ].type_inx ));
             printf( "  port = %s\n", ev3_tacho_port_name( i, s ));

          }
     }

   //MotorA is 0, MotorB is 1, MotorC is 2
   //Configure tacho motors
   uint8_t carMotor = 0;
   uint8_t pulleyMotor = 1;
   uint8_t clawMotor = 2;
   uint8_t ev3_sn[] =  {carMotor, pulleyMotor, clawMotor};

   //Getting motor max speeds
   int max_speed_A;
   int max_speed_B;
   int max_speed_C;
   get_tacho_max_speed(carMotor, &max_speed_A);
   get_tacho_max_speed(pulleyMotor, &max_speed_B);
   get_tacho_max_speed(clawMotor, &max_speed_C);
   printf("Max speed Motor A: %d\n", max_speed_A);
   printf("Max speed Motor B: %d\n", max_speed_B);
   printf("Max speed Motor C: %d\n", max_speed_C);
   int driving_speed = max_speed_A/4;
   int pulley_speed = max_speed_B/4;
   int claw_speed = max_speed_C/2;
   int direction_assist = 1;

   //Set stopping operation
   for (i = 0; i < 3; i++)
     {
        set_tacho_stop_action_inx(i, TACHO_HOLD);
     }

   //Enabling keyboard input
   //Enable curses mode
   //Use printw from here on in!
   initscr();
   //Recieve raw data from keyboard
   raw();
   //Do not echo the input keystrokes
   noecho();
   //no delay between key presses
   nodelay(stdscr, 1);

   int key;
   int isQPressed = 0;
   int count = 0;
   printw("Press H to view the help menu. Q to quit. Good Luck!\n");

   while(!isQPressed)
     {

        //Get user input
        key = getch();
        switch(key)
          {

           //Left
           case 'a':
             //printw("You pressed A!\n");
             set_tacho_speed_sp(carMotor, direction_assist * driving_speed);
             set_tacho_time_sp(carMotor, 50);

           //Right
           case 'd':
             //printw("You pressed D!\n");
             set_tacho_speed_sp(carMotor, -direction_assist * driving_speed);
             set_tacho_time_sp(carMotor, 50);
             set_tacho_command_inx(carMotor, TACHO_RUN_TIMED);
             break;

           //Pulley Up
           case 'w':
             printw("You pressed W!\n");
             break;

           //Pulley Down
           case 's':
             printw("You pressed S!\n");
             break;

           //Claw open
           case 'k':
             printw("You pressed K!\n");
             break;

           //Claw close
           case 'l':
             printw("You pressed L!\n");
             break;

           //Auto Routine
           case 'i':
             printw("You pressed I!\n");
             break;

           //Quit
           case 'q':
             printw("You pressed Q!\n");
             printw("Quitting\n");
             isQPressed = 1;
             break;

            //Help Menu
            case 'h':
             printw("You pressed H!\n");

            //Help Menu
            case 'h':
             printw("You pressed H!\n");
             printw("Help Menu\n");
             printw("A is Left. D is Right\n");
             printw("W raises the claw. S lowers the claw\n");
             printw("K opens the claw. L closes the claw\n");
             printw("I initiates automatic routine for grabbing and return to base\n");
             printw("H is the help menu\n");
             printw("Q is quit\n");
             break;

            //Default for any other key
            default:
             count = 0;
             break;
          }

     }

   //Disable curses mode
   endwin();
   printf("Goodbye\n");
   return 0;

}

