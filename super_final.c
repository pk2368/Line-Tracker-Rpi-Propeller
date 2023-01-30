#include "simpletools.h"
#include "servo.h"
#include "ping.h"

//stacks for cogs
unsigned int stack1[40+25];
unsigned int stack2[45+25];
unsigned int stack3[45+25];
unsigned int stack4[45+25];
unsigned int stack5[45+25];

static volatile int led1,led2,ping1,ping2;

//constants
#define servo1 13 //Left
#define servo2 15 //Right
#define led1 1 //Red
#define led2 2 //Green
#define ping1 6//Center ultrasonic
#define ping2 4//Left Ultrasonic

int junc = 0;
int cmDist1;
int cmDist2;
int local_Dist;
int cd = 0;
int rot = 0;
int cog1,cog2,cog3,cog4,cog5;
int ir1, ir2, ir3;
int j_count = 0;
int turn_flag = 0;
int detect = 0;
int sflag = 0;
//int stop_flag = 0;
int obj = 0;

//Function Prototypes
void line_follow();
void forward();
void forward_junc(int a);
void back_junc(int a);
void back();
void left(int a);
void right(int a);
void junction_maneuver();
void led1_glow(void *par1);
void led2_glow(void *par2);
void check_obstacle(void *par3);
void left_turn();
void right_turn();
void uturn(int val);
void object_detect(void *par4);
void just_line_follow();
void check_obs(void *par5);
void line_follow_2();

//Main Program
int main()
{
  low(12);
  while(1)
  {
    int c = input(0);
    pause(50);
    if(c==1)
    {
      while(1)
      {
        if(rot==0 && obj<8)
        {
          ir1 = input(10);//Right
          ir2 = input(5);//Middle
          ir3 = input(11);//Left
          line_follow();
         }
         /*
         else if((rot==0 && stop_flag==1 && obj<8) || (rot==1 && stop_flag==1 && obj<8))
         {
           print("Stop_flag==1 \n");
           high(12);//signal to pi
           //pause(500);
           forward_junc(50);
           servo_stop();
           low(12);//stopping signal
           pause(3000);//stopping the bot for 3 seconds
           stop_flag=0;
         }*/
         else if(rot==1 && obj<8)//U-turn procedure
         {
          print("Distance detected just before taking a uturn: %d\n",cmDist1);
          print("Rotation Var pos1: %d\n",rot);
          back_junc(100);
          cd = 1;
          rot = 0;
          pause(100);
          print("Rotating...... \n");
          uturn(1100);
          back_junc(250);
          print("Rotation Var pos2: %d\n",rot);
         }
         else
         {
           print("Done!!");
           forward_junc(300);
           pause(50);
           servo_stop();
           //cogstop(cog3);
           for(int i=0;i<5;i++)
           {
             high(led1);
             high(led2);
             pause(500);
             low(led1);
             low(led2);
             pause(500);
           }             
           pause(10000);
         }                  
       }      
     }
   }
}            

//Functions
void forward()
{
  servo_speed(servo1, 20);
  servo_speed(servo2, -25);
}
void forward_junc(int val)
{
  pause(100);
  servo_set(servo1,1590);
  servo_set(servo2,1490);
  pause(val);
}  
void back()
{
  servo_speed(servo1,-30);
  servo_speed(servo2,30);
}
void back_junc(int a)
{
  pause(100);
  servo_set(servo1,1400);
  servo_set(servo2,1590);
  pause(a);
}  
void left(int a)
{
  servo_speed(servo1,a);
  servo_speed(servo2,a+5);
  pause(100);
}
void right(int a)
{
  servo_speed(servo1,-a);
  servo_speed(servo2,-a-5);
  pause(100);
}
void left_turn()
{
  pause(100);
  servo_set(servo1,1400);
  servo_set(servo2,1400);
  pause(600);
}
void right_turn()
{
  pause(100);
  servo_set(servo1,1600);
  servo_set(servo2,1600);
  pause(300);
}
void uturn(int val)
{
  pause(100);
  servo_set(servo1,1400);
  servo_set(servo2,1400);
  pause(val);
}   
     
void line_follow()
{
  if(ir1 == 0 && ir2 == 1 && ir3 == 0)
  {
    forward();
  }
  else if(ir1==1 && ir2==0 && ir3==0)
  {
    right(10);
  }
  else if(ir1 == 1 && ir2 == 1 && ir3 == 0)
  {
    right(20);
  }
  else if(ir1 == 0 && ir2 == 1 && ir3 == 1)
  {
    left(10);
  }
  else if(ir1 == 0 && ir2 == 0 && ir3 == 1)
  {
    left(20);
  }
  else if(ir1 == 0 && ir2 == 0 && ir3 == 0)
  {
    back();
  }
  else if(ir1 == 1 && ir2 == 1 && ir3 == 1)
  {
    junction_maneuver();
  }         
}
void just_line_follow()
{
  if(ir1 == 0 && ir2 == 1 && ir3 == 0)
  {
    forward();
  }
  else if(ir1==1 && ir2==0 && ir3==0)
  {
    right(20);
  }
  else if(ir1 == 1 && ir2 == 1 && ir3 == 0)
  {
    right(20);
  }
  else if(ir1 == 0 && ir2 == 1 && ir3 == 1)
  {
    left(20);
  }
  else if(ir1 == 0 && ir2 == 0 && ir3 == 1)
  {
    left(20);
  }
  else if(ir1 == 0 && ir2 == 0 && ir3 == 0)
  {
    back();
  }
  else if(ir1 == 1 && ir2 == 1 && ir3 == 1)
  {
    if(sflag==0)
    {
      j_count=j_count+1;
      print("Junction number = %d \n",junc);
      print("J count = %d \n",j_count);
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      while(ir1==1 && ir2==1 && ir3==1)
          {
            forward();
            ir1 = input(10);
            ir2 = input(5);
            ir3 = input(11);
          }
    }        
  }         
}
void line_follow_2()
{
  if(ir1 == 0 && ir2 == 1 && ir3 == 0)
  {
    forward();
  }
  else if(ir1==1 && ir2==0 && ir3==0)
  {
    right(20);
  }
  else if(ir1 == 1 && ir2 == 1 && ir3 == 0)
  {
    right(20);
  }
  else if(ir1 == 0 && ir2 == 1 && ir3 == 1)
  {
    left(20);
  }
  else if(ir1 == 0 && ir2 == 0 && ir3 == 1)
  {
    left(20);
  }
  else if(ir1 == 0 && ir2 == 0 && ir3 == 0)
  {
    back();
  }
}  
void led1_glow(void *par1)
{
  high(led1);
  pause(500);
  low(led1);
  pause(500);
  cogstop(cog1);
}
void led2_glow(void *par2)
{
  high(led2);
  pause(500);
  low(led2);
  pause(500);
  cogstop(cog2);
}
 
void check_obstacle(void *par3)
{
 while(rot==0)
  {
    cmDist1 = ping_cm(ping1);
    pause(200);
    if(cmDist1<=5)
    {
      rot = 1;
      //print("Object Detected on periphery");
      if(junc<6)//To check between i5 & i3
      {
        turn_flag = 1;
      }            
      cog2 = cogstart((void*)led2_glow,NULL,stack2,sizeof(stack2));
    }          
  }
  pause(200);
  cogstop(cog4); 
}   
void object_detect(void *par4)
{
  while(obj<8)
  {
    cmDist2 = ping_cm(ping2);
    pause(100);
    if(cmDist2<15)
    {
      cog2 = cogstart((void*)led2_glow,NULL,stack2,sizeof(stack2));
      pause(1000);
      //stop_flag = 1;
      obj=obj+1;
      pause(2000);//Add 6 seconds for stop flag
    }
  }
  cogstop(cog3);
}
void junction_maneuver()
{
  if(cd==0)
  {
    junc = junc+1;
    print("going straight \n");
    if(junc<2)
    {
      //cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      print("Junction number = %d \n",junc);
      while(ir1==1 && ir2==1 && ir3==1)
        {
          forward();
          ir1 = input(10);
          ir2 = input(5);
          ir3 = input(11);
        }
    }
    else
    {
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      //pause(50);
      if(junc==3)
      {
        print("Starting the obstacle detection!\n");
        cog4 = cogstart((void*)check_obstacle,NULL,stack4,sizeof(stack4));
        print("Starting object detection also!! \n");
        cog3 = cogstart((void*)object_detect,NULL,stack3,sizeof(stack3));
      }        
      print("Junction number = %d \n",junc);
      print("Checking for obstacle \n");
      print("Distance detected: %d\n",cmDist1);
      while(ir1==1 && ir2==1 && ir3==1)
        {
          forward();
          ir1 = input(10);
          ir2 = input(5);
          ir3 = input(11);
        }
    }        
  }
  else if(cd==1)
  {
    print("coming back \n");
    if(junc==3)
      {
        cd = 2;
        cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
        print("Junction number = %d \n",junc); 
        forward_junc(200);
        pause(100);
        right_turn();
        print("At the start junction, Turning right");
      }
    else
    {
      junc=junc-1;
      print("Returning after detecting an obstruction \n");
      print("Junction number = %d \n",junc);
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      while(ir1==1 && ir2==1 && ir3==1)
       {
        forward();
        ir1 = input(10);
        ir2 = input(5);
        ir3 = input(11);
       }
    }
  }                        
  else
  {  
    junc = junc+1;
    //print("approaching junction 4 \n");
    print("Junction number = %d \n",junc);
   if(junc==4)
    {
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      pause(50);
      //cog3 = cogstart((void*)object_detect,NULL,stack3,sizeof(stack3));
      forward_junc(200);
      pause(100);
      right_turn();
      //print("Junction number = %d \n",junc);
    }
    else if(junc==7)
    {
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      //pause(500);
      servo_stop();
      pause(300);
      forward_junc(300);
      pause(100);
      right_turn();
      //print("Junction number = %d \n",junc);
    }
    else if(junc==8)
    {
      print("At junction 8 \n");
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      if(turn_flag==1)
      {
       print("Taking right at junction 8 \n");
       forward_junc(400);
       pause(100);
       right_turn();
       //pause(50);
       //int detect = 0;
       cog5 = cogstart((void*)check_obs,NULL,stack5,sizeof(stack5));
       print("Checking for obstacle... \n");
       while(detect==0)
       {
         ir1 = input(10);
         ir2 = input(5);
         ir3 = input(11);
         just_line_follow();  
       }
       print("Uturn taken, Going to i5... \n");       
       uturn(1100);
       back_junc(100);
       //pause(4000);
       print("Returning to junction 8 \n");
       if(j_count==1)
       {
         print("Obstacle at i2\n");
         while(j_count<4)
         {
           ir1 = input(10);
           ir2 = input(5);
           ir3 = input(11);
           just_line_follow();
         }
       }
       else
       {
         print("Obstacle at i3\n");
         while(j_count<2)
         {
           ir1 = input(10);
           ir2 = input(5);
           ir3 = input(11);
           just_line_follow();
         }
       }
       print("i5 reached..\n");
       forward_junc(200);
       uturn(1200);
       back_junc(100);
       //left();
       sflag = 1;
       while(!(ir1==1 && ir2==1 && ir3==1))
       {
         print("Searching for i4\n");
         ir1 = input(10);
         ir2 = input(5);
         ir3 = input(11);
         line_follow_2();
       }
       print("Turning Left\n");
       forward_junc(400);
       pause(100);
       left_turn();                                                          
      }  
      else
      {
       print("Normal Junction");
       //cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
       while(ir1==1 && ir2==1 && ir3==1)
        {
          forward();
          ir1 = input(10);
          ir2 = input(5);
          ir3 = input(11);
        }
      }
    }                       
    else if(junc==9)
    {
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      forward_junc(200);
      pause(100);
      right_turn();
      //print("Junction number = %d \n",junc);
    }
    else if(junc==12)
    {
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      forward_junc(200);
      pause(100);
      right_turn();
      if(obj==7)
      {
        print("Stopping object detection");
        cogstop(cog3);
      }
      //print("Junction number = %d \n",junc);
    }
    else if(junc==14)
    {
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));        
      forward_junc(200);
      pause(100);
      right_turn();
      //print("Junction number = %d \n",junc);
    }
    else if(junc==18)
    {
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      print("Starting object detection again!! \n");
      cog3 = cogstart((void*)object_detect,NULL,stack3,sizeof(stack3));
      forward_junc(200);
      //pause(1000);
      //cogstop(cog3);
      servo_stop();
      if(obj<8)
      {
        obj=8;
      }        
      //pause(20000);
      //right_turn();
      //print("Junction number = %d \n",junc);
    }
    else//TO cross normal junction
    {
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      while(ir1==1 && ir2==1 && ir3==1)
        {
          forward();
          ir1 = input(10);
          ir2 = input(5);
          ir3 = input(11);
        }
    }
  }          
}

void check_obs(void *par5)
{
  while(detect==0)
  {  
    local_Dist = ping_cm(ping1);
    pause(200);
    if(local_Dist<=7)
    {   
      detect = 1;
      //cog2 = cogstart((void*)led2_glow,NULL,stack2,sizeof(stack2));
      pause(100);
    }
  }
  cogstop(cog5);    
}
