#include "simpletools.h"
#include "servo.h"
#include "ping.h"

//stacks for cogs
unsigned int stack1[40+25];
unsigned int stack2[45+25];
unsigned int stack3[45+25];
unsigned int stack4[45+25];

static volatile int led1,led2,ping1,ping2;

//constants
#define servo1 13 //Left
#define servo2 15 //Right
#define led1 1 //Red
#define led2 2 //Green
#define ping1 7//Center ultrasonic
#define ping2 4//Left Ultrasonic

int junc = 0;
//int co = 0;
//int avg_dist = 0;
int cmDist1;
int cmDist2;
int cd = 0;
int rot = 0;
int cog1,cog2,cog3,cog4;
int ir1, ir2, ir3;
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

//Main Program
int main()
{
  while(1)
  {
    int c = input(0);
    pause(50);
    if(c==1)
    {
      while(1)
      {
        ir1 = input(10);//Right
        ir2 = input(9);//Middle
        ir3 = input(11);//Left
        line_follow();
     }      
    }
  }
}            
/*
int main()                                    
{
  while(1)
  { 
    int c;
    c = input(0);
    pause(50);
    if(c==1)
    {
      pause(1000);
      while(1)
      {
        if(rot==0 && obj<2)
         {
          //pause(100);//before 300
          ir1 = input(10);
          ir3 = input(11);
          ir2 = input(9);
          line_follow();
        }
        else if(rot==1 && obj<2)
        {
          cd = 1;
          //pause(1000);
          rot=0;
          //cogstop(cog3);
          pause(100);
          //pause(100);
          uturn(1000);
          back_junc(100);
          //servo_stop();
          //cd = 1;
        }
        else
        {
          print("Junction number = %d \n",junc);
          servo_stop();
          cogstop(cog4);
          for(int n=0;n<5;n++)
          {
            high(26);
            high(27);
            pause(500);
            low(26);
            low(27);
            pause(500);
          }          
          pause(10000);
        }                              
      }    
    }
  }    
}
*/
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
  pause(500);
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
    //cmDist11= ping_cm(ping1);
    //if((cmDist1+cmDist11)/2<=7)
    if(cmDist1<=7)
    {
      //cd = 1;
      rot = 1;
      cog2 = cogstart((void*)led2_glow,NULL,stack2,sizeof(stack2));
      //cogstop(cog3);
      //co=1;
    }           
  }
  cogstop(cog3); 
}   
void object_detect(void *par4)
{
  while(1)
  {
    cmDist2 = ping_cm(ping2);
    pause(100);
    if(cmDist2<15)
    {
      cog2 = cogstart((void*)led2_glow,NULL,stack2,sizeof(stack2));
      obj = obj+1;
      pause(3000);
    }
  }
}
void junction_maneuver()
{
  junc = junc+1;
  if(junc<3)
  {
    //cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
    while(ir1==1 && ir2==1 && ir3==1)
      {
        forward();
        ir1 = input(10);
        ir2 = input(9);
        ir3 = input(11);
      }
  }
  else if(junc==3)
  {
    cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
    //cog3 = cogstart((void*)object_detect,NULL,stack3,sizeof(stack3));
    forward_junc(200);
    pause(100);
    left_turn();
    //print("Junction number = %d \n",junc);
  }
  else if(junc==4)
  {
    cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
    forward_junc(200);
    pause(100);
    right_turn();
    //print("Junction number = %d \n",junc);
  }
  else if(junc==7)
  {
    cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
    forward_junc(200);
    pause(100);
    right_turn();
    //print("Junction number = %d \n",junc);
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
    forward_junc(200);
    //pause(1000);
    //cogstop(cog3);
    pause(20000);
    //right_turn();
    //print("Junction number = %d \n",junc);
  }
  else
  {
    cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
    while(ir1==1 && ir2==1 && ir3==1)
      {
        forward();
        ir1 = input(10);
        ir2 = input(9);
        ir3 = input(11);
      }
   }      
}      
/*    
void junction_maneuver()
{
  if(cd==0)
  {
    junc = junc+1;
    //pause(100);
    print("Junction number = %d \n",junc);
    //cog3 = cogstart((void*)check_obstacle,NULL,stack3,sizeof(stack3));
    if(junc<=1)
    {
      //pause(100);
      print("Junction number = %d \n",junc);
      if(junc==1)
      {
        cog3 = cogstart((void*)check_obstacle,NULL,stack3,sizeof(stack3));
      }        
      while(ir1==1 && ir2==1 && ir3==1)
      {
        forward();
        ir1 = input(10);
        ir2 = input(9);
        ir3 = input(11);
      }
    }
    else
    {
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      print("Junction number = %d \n",junc);
      //cog3 = cogstart((void*)check_obstacle,NULL,stack3,sizeof(stack3));
      while(ir1==1 && ir2==1 && ir3==1)
      {
        forward();
        ir1 = input(10);
        ir2 = input(9);
        ir3 = input(11);
     }               
    }     
  }
  else if(cd==1)
    {
        //junc=junc-1;
        //cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
        print("Junction number = %d \n",junc);     
      if(junc==3)
      {
        cd = 2;
        cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
        print("Junction number = %d \n",junc); 
        forward_junc(200);
        pause(100);
        right_turn();
      }
      else
      {
        junc=junc-1;
        print("Junction number = %d \n",junc);
        cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
        if(junc==6)
        {  
          while(ir1==1 && ir2==1 && ir3==1)
         {
          forward();
          ir1 = input(10);
          ir2 = input(9);
          ir3 = input(11);
         } 
       }
       else
       {
         junc=8;
         cd=2;
         forward_junc(400);
         left_turn();
         print("Junction number = %d \n",junc);       
       }       
      }
    }
 else                  
   {     
    junc = junc+1;//After coming back to junction I1
    if(junc>2)
    {
                  
      cog1 = cogstart((void*)led1_glow,NULL,stack1,sizeof(stack1));
      print("Junction number = %d \n",junc);
      //cog4 = cogstart((void*)object_detect,NULL,stack4,sizeof(stack4));
      if(junc==3)
      {
        //pause(100);
        forward_junc(200);
        pause(100);
        left_turn();
        //pause(100);
        //cog4 = cogstart((void*)object_detect,NULL,stack4,sizeof(stack4));
        print("Junction number = %d \n",junc);
      }
      else if(junc==4)
      {
        forward_junc(400);
        pause(100);
        right_turn();
        print("Junction number = %d \n",junc);
        cog4 = cogstart((void*)object_detect,NULL,stack4,sizeof(stack4));
      }
      else if(junc==7)
      {
        forward_junc(400);
        pause(100);
        right_turn();
        print("Junction number = %d \n",junc);
      }
      else if(junc==9)
      {
        forward_junc(400);
        pause(100);
        right_turn();
        print("Junction number = %d \n",junc);
      }
      else if(junc==12)
      {
        forward_junc(400);
        pause(100);
        right_turn();
        print("Junction number = %d \n",junc);
      }
      else if(junc==14)
      {
        forward_junc(400);
        pause(100);
        right_turn();
        print("Junction number = %d \n",junc);
      }
      else if(junc==18)
      {
        print("Junction number = %d \n",junc);
        servo_stop();
        cogstop(cog4);
        for(int n=0;n<5;n++)
        {
          high(26);
          high(27);
          pause(500);
          low(26);
          low(27);
          pause(500);
        }          
        pause(10000);
        
      }
      else
      {
        while(ir1==1 && ir2==1 && ir3==1)
        {
          forward();
          ir1 = input(10);
          ir2 = input(9);
          ir3 = input(11);
        }
      }          
    }  
  }  
}
*/