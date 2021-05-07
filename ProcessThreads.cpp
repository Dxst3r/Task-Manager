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
void* PrintAll( void* );
void PrintProcessNameAndID( DWORD processID );
void PrintMemoryInfo( DWORD processID );
void PrintCPUInfo(DWORD processID);

pthread_t tid[2]; 
ofstream myfile;

int main (void){
  //  ofstream myfile;
    int i = 0; 
    int error; 
  
    while (i < 10) { 
        error = pthread_create(&(tid[i]), NULL, &PrintAll, NULL); 
        if (error != 0) 
            printf("\nThread can't be created : [%s]", strerror(error)); 
         
        Sleep(10000);
        i++;
    } 
  
    pthread_join(tid[0], NULL); 
   // pthread_join(tid[1], NULL); 

  //  PrintAll();
    
 // printf(x);
  
  /*myfile.open ("example.txt");
  
  myfile.close();*/
  return 0; 


}

void* PrintAll( void* )
{
    // Get the list of process identifiers.

 //   pthread_mutex_lock(&mutex);
    DWORD aProcesses[1024], cbNeeded, cProcesses;
   
  /*  if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return 1;
    }
    */

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.

        //unsigned int x = (rand() % cProcesses) +1  ;
        unsigned int  i =  0 ;
            
            PrintProcessNameAndID( aProcesses[i] );
            PrintMemoryInfo( aProcesses[i] ); 
            PrintCPUInfo(aProcesses[i]);
                                          
        
   // pthread_mutex_unlock(&mutex); 

    
}


// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

void PrintProcessNameAndID( DWORD processID )
{
    processID = 9428;
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );

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

   
    // Print the process name and identifier.
   /* myfile.open ("example.txt", ios::app);
  
    myfile << "(PID: " << processID << " )" << setw(15)  << "\t "  << szProcessName ;
    myfile.close();*/

    _tprintf( TEXT("(PID: %u) \t%s  "), processID, szProcessName );
    

    // Release the handle to the process.

    CloseHandle( hProcess );

   // return to_string, szProcessName;
}

void PrintMemoryInfo( DWORD processID )
{
    processID = 9428;
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS_EX pmc;

    // Print the process identifier.

  //  printf( "\nProcess ID: %u", processID );

    // Print information about the memory usage of the process.

    hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ,
                                    FALSE, processID );

    if (NULL == hProcess)
        return;

    //total memory
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;


    if ( GetProcessMemoryInfo( hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)) )
    {
        
        //long li_auto = std::stol (pmc.WorkingSetSize,nullptr,0);
        printf("\tWorkingSetSize: %ld bytes", pmc.WorkingSetSize );
        printf("\tPercentage: %.3f ", (pmc.WorkingSetSize*100)/(double)totalVirtualMem );
        /*myfile.open ("example.txt", ios::app);
        myfile << setw(30) << "WorkingSetSize: " << pmc.WorkingSetSize << setw(2) << "bytes";
      
       myfile << "\tPercentage: " << (pmc.WorkingSetSize*100)/(double)totalVirtualMem << setprecision(2) << setw(8);
       myfile.close();*/
      //  return pmc.WorkingSetSize, (pmc.WorkingSetSize*100)/(double)totalVirtualMem;
       
    }

    CloseHandle( hProcess );
}

void PrintCPUInfo(DWORD processID){
     processID = 9428;

    HANDLE hProcess;
    

    hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ,
                                    FALSE, processID );
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
       // return CpuTime.QuadPart;
      /* myfile.open ("example.txt", ios::app);
       myfile << "\tCPU Time: " << CpuTime.QuadPart << "\n";
       myfile.close();*/
     }
}
