#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <fstream>
#include<conio.h>
#include<iomanip>
#include <pthread.h> 
#include<mutex>
using namespace std;
void* PrintAll1( void* );
void* PrintAll2( void* );
void PrintProcessNameAndID( DWORD processID );
void PrintMemoryInfo( DWORD processID );
void PrintCPUInfo(DWORD processID);

pthread_t tid[2]; 
ofstream myfile;


int main (void){

    int i = 0; 
    int error; 
  
    // creates 2 threads to watch and print the metrics of 2 processes every 10 seconds 10 times 
    while(i < 10) {
        error = pthread_create(&(tid[0]), NULL, &PrintAll1, NULL); 
        if (error != 0) 
            printf("\nThread can't be created : [%s]", strerror(error)); 
        
        pthread_join(tid[0], NULL);
         
          

        error = pthread_create(&(tid[1]), NULL, &PrintAll2, NULL); 
        if (error != 0) 
            printf("\nThread can't be created : [%s]", strerror(error)); 
        
        pthread_join(tid[1], NULL); 

        Sleep(10000);
        i++;    
    }
        

  return 0; 


}

void* PrintAll1( void* )
{

    // Print the ID, Name, memory and CPU usage of the first process     
            PrintProcessNameAndID( 9428);
            PrintMemoryInfo( 9428 ); 
            PrintCPUInfo(9428);
               
}
void* PrintAll2( void* )
{

    // Print the ID, Name, memory and CPU usage of the second process

            PrintProcessNameAndID( 9248);
            PrintMemoryInfo( 9248 ); 
            PrintCPUInfo(9248);
        
  

    
}


void PrintProcessNameAndID( DWORD processID )
{
    DWORD aprocessID = processID;
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, aprocessID );

    // Get the process name.

    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModulesEx( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded, LIST_MODULES_ALL) )
        {
            GetModuleBaseName( hProcess, hMod, szProcessName, 
                               sizeof(szProcessName)/sizeof(TCHAR) );
        }
    }

   
    _tprintf( TEXT("(PID: %u) \t%s  "), processID, szProcessName );
    

    // Release the handle to the process.

    CloseHandle( hProcess );
  }

void PrintMemoryInfo( DWORD processID )
{
    DWORD aprocessID = processID;
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS_EX pmc;

  
    // Print information about the memory usage of the process.

    hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ,
                                    FALSE, aprocessID );

    if (NULL == hProcess)
        return;

    //total memory
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;


    if ( GetProcessMemoryInfo( hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)) )
    {
        
        
        printf("\tWorkingSetSize: %ld bytes", pmc.WorkingSetSize );
        printf("\tPercentage: %.3f ", (pmc.WorkingSetSize*100)/(double)totalVirtualMem );
               
    }

    CloseHandle( hProcess );
}

void PrintCPUInfo(DWORD processID){
     DWORD aprocessID = processID;

    HANDLE hProcess;
    

    hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ,
                                    FALSE, aprocessID );
    if (NULL == hProcess)
        return;

    FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

     if (GetProcessTimes(hProcess, &ftProcCreation,&ftProcExit, &ftProcKernel, &ftProcUser)){
        LARGE_INTEGER lKernel, lUser, CpuTime;
        lKernel.LowPart = ftProcKernel.dwLowDateTime;
        lKernel.HighPart = ftProcKernel.dwHighDateTime;
        lUser.LowPart = ftProcUser.dwLowDateTime;
        lUser.HighPart = ftProcUser.dwHighDateTime;
        CpuTime.LowPart = lKernel.LowPart + lUser.LowPart;
        CpuTime.HighPart = lKernel.HighPart + lUser.HighPart;
        printf( "\tCPU Time: %lld\n", CpuTime.QuadPart );
    }
}
