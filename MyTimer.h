#ifndef MYTIMER_H
#define MYTIMER_H

#include <windows.h>
#include <winbase.h>


/*****************************************************************************
                            class MyTimer
*****************************************************************************/
//This class implements the functions: void      MySleep(inSeconds)
//                                     void      StopwatchStart(void)
//                                     ULONGLONG StopwatchEnd(void)  
//                                      
//
//The timer class counts the ticks per second of  the CPU/Hardware Timer
//and using this value implements the given functionality.
//
//It uses the windows performance counter if available.
//If not it uses the RDTSC(Read Time Stamp Counter) assembly instruction.
/****************************************************************************/
class MyTimer
{

private:
    
    bool            bPCounterAvailable;         //If 1 -Performance counter is used
                                                //If 0 -RDTSC assembly instruction is used
    ULONGLONG       iNumberOfTicksPerSecond;    

    //Variables used only if performance timer is available
    LARGE_INTEGER   largeIntTemp;
    LARGE_INTEGER   liStopwatchStart, liStopwatchEnd;
 
    //Variables used only when RDTSC assembly code is used
    ULONGLONG       iStopwatchStart, iStopwatchEnd;

public:
    

    //------------------------------------------------------------------------
    //Purpose:      <Inits the Timer class>
    //
    //Arguments:    <none>
    //
    //Design Info:  <Uses the Performance Timers or the RDTSC assembly 
    //               instruction.>
    //
    //------------------------------------------------------------------------
    MyTimer(void)
    {   
        //First try with the performance counter if available
        if( QueryPerformanceFrequency(&largeIntTemp) )
        {
           iNumberOfTicksPerSecond = largeIntTemp.QuadPart;
           bPCounterAvailable = true;
        }

        //if performance counter is  unavailable use the RDTSC
        else
        {
            GetTicksPer1Second();
            bPCounterAvailable = false;
        }
    }

    
    //------------------------------------------------------------------------
    //Purpose:      <Sleeps the  program for the specified amount of time in
    //               seconds>
    //
    //Arguments:    <The needed time in milliseconds>
    //
    //Returns:      <none>  
    //
    //Design Info:  <Uses the Performance Timers or the RDTSC assembly 
    //               instruction.>
    //
    //------------------------------------------------------------------------
    void MySleep(unsigned iInMilliseconds)
    {
       //If performance counter is availabe
       if( bPCounterAvailable )
       {
           LARGE_INTEGER liTemp;
           
           QueryPerformanceCounter(&liTemp);//Get number of ticks
           QueryPerformanceCounter(&largeIntTemp);
          
                   //  ticks that must be made so the needed time elapses      //  > //  current ticks  //          
           while( ( (ULONGLONG)liTemp.QuadPart + (iNumberOfTicksPerSecond/1000)*iInMilliseconds ) > (ULONGLONG)largeIntTemp.QuadPart)
           {
               QueryPerformanceCounter(&largeIntTemp);
           }

           return;
       }

       //if not...
       ULONGLONG iBefore;

       iBefore = Rdtsc();

       while( (iBefore + (iNumberOfTicksPerSecond/1000)*iInMilliseconds) >  Rdtsc() );
    }

    //------------------------------------------------------------------------
    //Purpose:      <If a time is needed to be measured call this function.
    //               After that, call the StopWatchEnd() function>
    //
    //Arguments:    <none>
    //
    //Returns:      <none>  
    //
    //Design Info:  <Uses the Performance Timers or the RDTSC assembly 
    //               instruction>
    //
    //------------------------------------------------------------------------
    void StopwatchStart(void)
    {
        //If Performance Timer is available
        if( bPCounterAvailable )
        {
            //Get the current value of the counter
            QueryPerformanceCounter(&liStopwatchStart);
            return;
        }

        //else use the RDTSC assembly instruction
        iStopwatchStart = Rdtsc();
    }


    //------------------------------------------------------------------------
    //Purpose:      <Stops counting the elapsed time >
    //
    //Arguments:    <none>
    //
    //Returns:      <The elapsed time in hundred of Micro Seconds between 
    //               the two calls of StopwatchStart() and StopwatchEnd() >                  
    //
    //Design Info:  <Uses the Performance Timers or the RDTSC assembly 
    //               instruction.
    //               Example: If the returned result is 120236, this means
    //                        12,0236seconds.>
    //------------------------------------------------------------------------
    ULONGLONG StopwatchEnd(void)
    {
        //If Performance Timer is available
        if( bPCounterAvailable )
        {
            QueryPerformanceCounter(&liStopwatchEnd);

            ULONGLONG itest =0;
            itest =  (liStopwatchEnd.QuadPart - liStopwatchStart.QuadPart) / (iNumberOfTicksPerSecond/10000) ;
            return itest;
        }

        //else use the RDTSC assembly instruction
        iStopwatchEnd = Rdtsc();

        return (iStopwatchEnd - iStopwatchStart) / (iNumberOfTicksPerSecond/10000);
    }


    //------------------------------------------------------------------------
    //Purpose:      <Get the raw ticks from the counter>
    //
    //Arguments:    <none>
    //
    //Returns:      <The CPU ticks since it started running>                  
    //------------------------------------------------------------------------
    static ULONGLONG GetRawTicks(void)
    {
        LARGE_INTEGER liTemp;

        //If Performance Timer is available
        if( QueryPerformanceCounter(&liTemp) )
        {
            return liTemp.QuadPart;
        }

        //else use the RDTSC assembly instruction
        return Rdtsc();
    }


    //------------------------------------------------------------------------
    //Purpose:      <Get the ticks from the counter. Each tick represents 1 millisecond>
    //
    //Arguments:    <none>
    //
    //Returns:      <The CPU ticks since it started running>                  
    //------------------------------------------------------------------------
    ULONGLONG GetTicksInMilliseconds(void)
    {
        LARGE_INTEGER liTemp;

        //If Performance Timer is available
        //First try with the performance counter if available
        if( QueryPerformanceCounter(&liTemp) )
        {
            return liTemp.QuadPart/(iNumberOfTicksPerSecond/1000);
        }

        //else use the RDTSC assembly instruction
        return Rdtsc()/(iNumberOfTicksPerSecond/1000);
    }





private:
//-----------------------------------------------------
    
    //returns number of ticks of the CPU from the time that is was switched on
    static ULONGLONG Rdtsc(void){
        _asm{RDTSC}    //Assembly code
    }

    //Get the iNumberOfTicksPerSecond
    void GetTicksPer1Second(void){   
        ULONGLONG ullBefore, ullAfter, ullTemp;
        
        
        ullTemp  = GetTickCount();
        ullBefore = Rdtsc();
        while( ullTemp + 2000 > GetTickCount() ); //w8 2 seconds
        ullAfter  = Rdtsc();

        iNumberOfTicksPerSecond = (ullAfter - ullBefore)/2;
    }

};

#endif //MYTIMER_H