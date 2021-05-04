#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

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

    _tprintf( TEXT("(PID: %u) \t%s  "), processID, szProcessName );


    // Release the handle to the process.

    CloseHandle( hProcess );
}

void PrintMemoryInfo( DWORD processID )
{
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
        int percentage =  pmc.WorkingSetSize * 100;
       // percentage /= totalVirtualMem;
        printf("\tPercentage: %.3f ", (pmc.WorkingSetSize*100)/(double)totalVirtualMem );
       
    }

    CloseHandle( hProcess );
}

void PrintCPUInfo(DWORD processID){

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
     }
}



int main( void )
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