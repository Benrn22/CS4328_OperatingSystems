#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <list>
#include <vector>

using namespace std;

const int PAGE_LENGTH = 100;    // Page-reference string length
const int FRAMES_TESTING = 30;  // Max number of frames testing

// Function Declarations
int FIFO( deque<string>, int );
int LRU( deque<string>, int );
int OPT( deque<string>, int );

bool pageExist( list<string>, string );     // To be used by FIFO
bool pageExist( vector<string>, string );   // To be used by LRU & OPT

bool pageUsed( vector<string> &, string, int, int );   // To be used by LRU
bool pageUsed( deque<string> &, string, int );         // To be used by OPT

int main( int argc, char* argv[] )
{
    int numFaultsFIFO;      // Stores num of page faults from FIFO algorithm
    int numFaultsLRU;       // Stores num of page faults from LRU algorithm
    int numFaultsOPT;       // Stores num of page faults from OPT algorithm
    stringstream ss;        // Stream int page value to string
    string page;            // Temporarily holds the string value of a page val
    deque<string> pageDeque; // Stores the random page-reference string

    // Opening output file to store test results in
    ofstream fout;
    fout.open("dataOutput.txt");

    // Error if output file not found - Program Ends
    if ( !fout )
    {
        cout << "ERROR - Failed to open output file. Make sure output file and "
             << "source code are in the same directory.";

        system( "PAUSE>NUL" );
        return 1;
    }

    // Generating the virtual page numbers for the random page-refernce string
    srand((int)time(0));
    for( int i = 0; i < PAGE_LENGTH; i++ )
    {
        int pageNum;
        pageNum = rand() % 49;
        ss.clear();
        ss << pageNum;
        ss >> page;
        pageDeque.push_back( page );
    }

    // Printing Header to Data File
    fout << "Benjamin Nye" << endl;
    fout << "CS 4328.253" << endl;
    fout << "Program 2" << endl;
    fout << "DUE - May 2, 2021" << endl << endl;
    fout << "PAGE FAULT TEST - OUTPUT DATA" << endl;
    fout << "******************************" << endl << endl;

    // Printing random page-reference string to output file
    fout << "Page-reference String:" << endl;
    for( int j = 0; j < pageDeque.size(); j++ )
    {
            fout << pageDeque[j] << " ";
            if( j != 0 && j % 20 == 0 )
            {
                fout << endl;
            }
    }
    fout << endl << endl;

    // Starting tests of algorithms with 1-X amount of frames
    for( int test = 1; test <= FRAMES_TESTING; test++ )
    {
        // Output test header to file
        fout << "-----------TEST #" << test << "-----------" << endl << endl;

        // Output num of frames for the test
        fout << "Number of frames: " << test << endl << endl;

        // Run & print algorithm test results
        numFaultsFIFO = FIFO( pageDeque, test );
        fout << "FIFO - Number of page faults: " << numFaultsFIFO << endl;
        numFaultsLRU = LRU( pageDeque, test );
        fout << "LRU - Number of page faults: " << numFaultsLRU << endl;
        numFaultsOPT = OPT( pageDeque, test );
        fout << "OPT - Number of page faults: " << numFaultsOPT << endl;
        fout << endl << endl;

    }

    return 0;
}

/*
    Function: FIFO

    The function, FIFO, uses the FIFO page replacement algorithm to insert the
    pages into memory and check for page faults.
    If space exists in memory and the page isnt already in mem, it is inserted
    and a page fault is recorded.
    If memory is full and the page does not exist in memory then, of the pages
    in mem, the one that was inserted first is removed and the new one is
    inserted and a page fault is recorded.

    Recieves: deque of strings (pageDeque)
              int for num of frames in mem (numFrames)

    Returns: Number of page faults
*/
int FIFO( deque<string> pageDeque, int numFrames )
{
    int faultCount = 0;
    list<string> pageFrames;    // Represents the memory
    list<string>::iterator it;

    while( pageDeque.size() != 0 ) // For each page reference
    {
        if( pageFrames.size() < numFrames ) // If there is space in memory
        {
            // If the page is not in memory, add it, record page fault
            // else, move on to the next page reference
            if( pageExist( pageFrames, pageDeque.front() ) == false )
            {
                pageFrames.push_back( pageDeque.front() );
                pageDeque.pop_front();
                faultCount++;
            }
            else
            {
                pageDeque.pop_front();
            }
        }
        else if( pageFrames.size() == numFrames ) // If memory is full
        {
            // If the page is not in memory, remove first added page, add new
            // page, record page fault.
            // else, move on to the next page reference
            if( pageExist( pageFrames, pageDeque.front() ) == false )
            {
                pageFrames.pop_front();
                pageFrames.push_back( pageDeque.front() );
                pageDeque.pop_front();
                faultCount++;
            }
            else
            {
                pageDeque.pop_front();
            }
        }
    }

    pageFrames.clear(); // Clear memory
    return faultCount;  // Return number of faults
}

/*
    Function: LRU

    The function, LRU, uses the LRU page replacement algorithm to insert pages
    into the memory and check for page faults.
    If space exists in memory and the page isnt already in mem, it is inserted
    and a page fault is recorded.
    If memory is full and the page does not exist in memory then, of the pages
    in mem, the one that last used is removed and the new one is
    inserted and a page fault is recorded.

    Recieves: deque of strings (pageDeque)
              int for num of frames in mem (numFrames)

    Returns: Number of page faults
*/
int LRU( deque<string> pageDeque, int numFrames )
{
    int faultCount = 0;
    vector<string> pageFrames;  // Represents Memory
    vector<string> pageDequeP;  // Hold the page references that have been used
    vector<string>::iterator it;
    int tNum;

    while( pageDeque.size() != 0 ) // For each page reference
    {
        if( pageFrames.size() < numFrames ) // If there is space in memory
        {
            // If the page is not in memory, add it, record page fault
            // else, move on to the next page reference
            if( pageExist( pageFrames, pageDeque.front() ) == false )
            {
                pageFrames.push_back( pageDeque.front() );
                pageDequeP.push_back( pageDeque.front() );
                pageDeque.pop_front();
                faultCount++;
            }
            else
            {
                pageDequeP.push_back( pageDeque.front() );
                pageDeque.pop_front();
            }
        }
        else if( pageFrames.size() == numFrames ) // If memory is full
        {
            // If the page is in memory, move on to the next page reference
            if( pageExist( pageFrames, pageDeque.front() ) )
            {
                pageDequeP.push_back( pageDeque.front() );
                pageDeque.pop_front();
            }
            // Else, find the page reference in memory that was used least
            // recently and replace it with the new page reference. Record a
            // Page fault
            else
            {
                int dif = ( PAGE_LENGTH - pageDeque.size() - 1 );
                string lruPg;
                tNum = numFrames;

                while( tNum > 0 && dif >= 0 )
                {
                    for( dif = PAGE_LENGTH - pageDeque.size() - 1; dif >= 0; dif--)
                    {
                        if( pageExist( pageFrames, pageDequeP[dif] ) )
                        {
                            int tdif = PAGE_LENGTH - pageDeque.size() - 1;
                            if( pageUsed( pageDequeP, pageDequeP[dif], tdif, dif ) )
                            {
                                tNum--;
                                lruPg = pageDequeP[dif];
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else
                        {
                            continue;
                        }
                    }
                }

                for( it = pageFrames.begin(); it != pageFrames.end(); it++ )
                {
                    if( lruPg == *it )
                    {
                        pageFrames.erase( it );
                        pageFrames.insert( it, pageDeque.front() );
                    }
                }

                pageDequeP.push_back( pageDeque.front() );
                pageDeque.pop_front();
                faultCount++;
            }
        }
    }

    pageFrames.clear(); // Clear memory
    pageDequeP.clear(); // Clear memory
    return faultCount;  // Return the number of page faults
}

/*
    Function: OPT

    The function, OPT, uses the OPT page replacement algorithm to insert pages
    into the memory and check for page faults.
    If space exists in memory and the page isnt already in mem, it is inserted
    and a page fault is recorded.
    If memory is full and the page does not exist in memory then, of the pages
    in mem, the page that wont be used for the longest period of time is removed
    and the new one is inserted and a page fault is recorded.

    Recieves: deque of strings (pageDeque)
              int for num of frames in mem (numFrames)

    Returns: Number of page faults
*/
int OPT( deque<string> pageDeque, int numFrames )
{
    int faultCount = 0;
    vector<string> pageFrames;  // Represents memory
    vector<string>::iterator it;
    deque<string>::iterator itt;
    int tNum;

    while( pageDeque.size() != 0 ) // For each page reference
    {
        if( pageFrames.size() < numFrames ) // If there is space in memory
        {
            // If the page is not in memory, add it, record page fault
            // else, move on to the next page reference
            if( pageExist( pageFrames, pageDeque.front() ) == false )
            {
                string tmp;
                tmp = pageDeque.front();
                pageFrames.push_back( tmp );
                pageDeque.pop_front();
                faultCount++;
            }
            else
            {
                pageDeque.pop_front();
            }
        }
        else if( pageFrames.size() == numFrames ) // If memory is full
        {
            // If the page is in memory, move on to the next page reference
            if( pageExist( pageFrames, pageDeque.front() ) == true )
            {
                pageDeque.pop_front();
            }
            // Else, find the page reference in memory that wont be called for
            // the longest time and replace that with the new page reference.
            // Record Page Fault.
            else
            {
                string optPage; // Store value of optimal page to replace
                tNum = numFrames; // Total number of frames
                bool bPage = true;

                while( tNum > 0 && bPage == true )
                {
                    for( itt = pageDeque.begin(); itt != pageDeque.end(); itt++ )
                    {
                        if( pageExist( pageFrames, *itt ) )
                        {
                            int dqSize = pageDeque.size();
                            if( pageUsed( pageDeque, *itt, dqSize ) )
                            {
                                tNum--;
                                optPage = *itt;
                            }
                        }
                        else
                        {
                            optPage = *itt;
                            bPage = false;
                            break;
                        }
                    }
                }
                // Replacing the Optimal page reference in mem with the new one
                for( it = pageFrames.begin(); it != pageFrames.end(); it++ )
                {
                    if( optPage == *it )
                    {
                        pageFrames.insert( it, pageDeque.front() );
                        pageFrames.erase( it );
                    }
                }

                pageDeque.pop_front();
                faultCount++;
            }
        }
    }

    pageFrames.clear(); // Clear memory
    return faultCount;  // Return the number of page faults
}

/*
    Function: pageExist

    The function, pageExist, checks to see if the current page reference is
    already in memory. If it is the function returns true, otherwise it returns
    false.
    This function is used by the FIFO function

    Recieves: list of strings (tPages)
              string value to be searched for (itt)

    Returns: If the value of itt was found in tPages or not
*/
bool pageExist( list<string> tPages, string itt )
{
    list<string>::iterator it;

    for( it = tPages.begin(); it != tPages.end(); it++ )
    {
        if( *it == itt )
        {
            return true;
        }
    }

    return false;
}

/*
    Function: pageExist

    The function, pageExist, checks to see if the current page reference is
    already in memory. If it is the function returns true, otherwise it returns
    false.
    This function is used by the LRU & OPT functions

    Recieves: vector of strings (tPages)
              string value to be searched for (itt)

    Returns: If the value of itt was found in tPages or not
*/
bool pageExist( vector<string> tPages, string itt )
{
    for(int it = 0; it < tPages.size(); it++)
    {
        if( tPages.at(it) == itt )
        {
            return true;
        }
    }
    return false;
}

/*
    Function: pageUsed

    The function, pageUsed, checks to see if the page reference has been used in
    memory. If so, it returns true. Otherwise, returns false.
    This function is used by the LRU function

    Recieves: address of vector of strings (&tPages)
              string value to be searched for (itt)
              int representing position in tPages (num)
              int representing position searching to (cNum)

    Returns: Returns if the value of itt has been used
*/
bool pageUsed( vector<string> &tPages, string itt, int num, int cNum)
{
    for( int it = num; it < cNum; it++ )
    {
        if( tPages[it] == itt )
        {
            return false;
        }
    }

    return false;
}

/*
    Function: pageUsed

    The function, pageUsed, hecks to see if the page reference has been used in
    memory. If so, it returns true. Otherwise, returns false.
    This function is used by the OPT function

    Recieves: address of vector of strings (&tPages)
              string value to be searched for (itt)
              int representing num of pages remaining (num)

    Returns: Returns if the value of itt has been used
*/
bool pageUsed( deque<string> &tPages, string itt, int num)
{
    for( int it = ( num - 1 ); it < PAGE_LENGTH; it++ )
    {
        if( tPages[it] == itt )
        {
            return true;
        }
    }

    return false;
}
