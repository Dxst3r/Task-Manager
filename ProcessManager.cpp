#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <fstream>
#include<conio.h>
#include<iomanip>
using namespace std;
int PrintAll( void );
void PrintProcessNameAndID( DWORD processID );
void PrintMemoryInfo( DWORD processID );
void PrintCPUInfo(DWORD processID);


ofstream myfile;

int main (void){
  
   // Print the ID, Name, memory used, %memory  and CPU Time of the all the running processes
    PrintAll();
 
  return 0; 

}

int PrintAll( void )
{
    // Get the list of process identifiers.

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return 1;
    }


    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.

    for ( i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
        {
            
            PrintProcessNameAndID( aProcesses[i] );
            PrintMemoryInfo( aProcesses[i] ); 
            PrintCPUInfo(aProcesses[i]);
                     
        }
    }
return 0;
    
}


void PrintProcessNameAndID( DWORD processID )
{
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
    myfile.open ("example.txt", ios::app);
   
    myfile << "(PID: " << processID << " )" << setw(15)  << "\t "  << szProcessName ;
    myfile.close();
    
    // Release the handle to the process.
    CloseHandle( hProcess );

}

void PrintMemoryInfo( DWORD processID )
{
    // Get a handle to the process.
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS_EX pmc;


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
        
        myfile.open ("example.txt", ios::app);
        myfile << setw(30) << "WorkingSetSize: " << pmc.WorkingSetSize << setw(2) << "bytes";
       myfile << "\tPercentage: " << (pmc.WorkingSetSize*100)/(double)totalVirtualMem << setprecision(2) << setw(8);
       myfile.close();       
    }

    CloseHandle( hProcess );
}

void PrintCPUInfo(DWORD processID){


    // Get a handle to the process.
    HANDLE hProcess;
    

    hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ,
                                    FALSE, processID );
    if (NULL == hProcess)
        return;


    // get the CPu time of the process by adding the user time and kernel time
    FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

     if (GetProcessTimes(hProcess, &ftProcCreation,&ftProcExit, &ftProcKernel, &ftProcUser)){
        LARGE_INTEGER lKernel, lUser, CpuTime;
        lKernel.LowPart = ftProcKernel.dwLowDateTime;
        lKernel.HighPart = ftProcKernel.dwHighDateTime;
        lUser.LowPart = ftProcUser.dwLowDateTime;
        lUser.HighPart = ftProcUser.dwHighDateTime;
        CpuTime.LowPart = lKernel.LowPart + lUser.LowPart;
        CpuTime.HighPart = lKernel.HighPart + lUser.HighPart;
       myfile.open ("example.txt", ios::app);
       myfile << "\tCPU Time: " << CpuTime.QuadPart << "\n";
       myfile.close();
     }
}






