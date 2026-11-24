#include <iostream>
#include <vector> 
#include <string> 
#include <algorithm> 

using namespace std; 
const int TOTAL_TIME = 360;   // total beach day time 12:00pm-6:00pm 

// Step 1: Create Shark Class 
// Shark attributes 
class Sharks
{
    private: 
        string type;        // type of shark
        int shark_time;     // how long a shark will stay in the water 

// Shark constructor with attributes 
    public: 
        Sharks(string t, int st)
            : type(t), shark_time(st)  {}

// getter for shark time (how long the shark stays)

   int get_shark_time() const 
   {
      return shark_time; 
   }

   //function to determine general shark schedule ( pink and blue)
   void shark_schedule( int start_time, vector<bool>& is_time_unsafe, vector<bool>& shark_timeline ) const 
   {
          // validate start time
          if(start_time < 0 || start_time >= TOTAL_TIME)
              return;

          // tracks duration of shark apperance 
          for ( int i = start_time; i < start_time + shark_time && i < TOTAL_TIME; i++)
           { 
                    is_time_unsafe[i] = true;       // not safe 
                    shark_timeline[i] = true;      // add time in timeline 
           }

   }

}; 

// Step 2: Generate Class to track the Beach Day 
class Beach
{
// private data for tracking shark schedules 
private: 

    vector<bool> is_time_unsafe; 
    vector<bool> pink_shark;        // pink shark moves 
    vector<bool> blue_shark;        // blue shark moves 
    vector<bool> purple_shark;      // purple shark moves 
    
public: 
// Beach Day constructor 
    Beach(): is_time_unsafe(TOTAL_TIME, false), 
    pink_shark(TOTAL_TIME, false) ,
    blue_shark(TOTAL_TIME, false) , 
    purple_shark(TOTAL_TIME, false) {}
 
// Get Shark schedules, unsafe swim times 
vector<bool>& get_pink() {return pink_shark; }
vector<bool>& get_blue() {return blue_shark; }
vector<bool>& get_purp() {return purple_shark; }
vector<bool>& get_unsafe() {return is_time_unsafe; }

// Helper fucntion to input shark apperance
    void input_sharks( const Sharks& shark, vector<bool>& timeline)
    {
        int st_time; 

        for( int i = 0; i < 5; i++)     // must have 5 apperences each 
        {
            cin >> st_time;

            // validate input range
            if(st_time < 0 || st_time >= TOTAL_TIME)
            {
                cout << "Invalid start time. Must be between 0 and 359.\n";
                i--;  // retry this input
                continue;
            }

            shark.shark_schedule(st_time, is_time_unsafe, timeline); 
        }                
    }


// fuction to check for valid beach day 
    bool is_valid_beach_day() const 
    {
        for( int t = 0; t < 6; t++)
        {   // variables for shark apperance 
            bool pk_shark = false; 
            bool bl_shark = false; 
            bool pr_shark = false; 

            for( int i = t * 60; i < (t + 1) * 60; i++)
            {
                if(pink_shark[i]) pk_shark = true; 
                if(blue_shark[i]) bl_shark = true;
                if(purple_shark[i]) pr_shark = true; 
            }

            if(!pk_shark || !bl_shark || !pr_shark)
            {
                return false;                  // return invalid if all sharks not present at least once 
            }

        }

        return true; 
    }


// find longest safe block 
    int find_longest_safe_block() const 
    {
        int max_time = 0;       // longest safe block 
        int current_time = 0;   // current safe time in schedule 

        for( int i = 0; i < TOTAL_TIME; i++)
        {
            if(!is_time_unsafe[i])  
            {
                    current_time++;  //if safe add time 
                max_time = max(max_time, current_time);
            }
            else 
            {
                current_time = 0;  // reset current time tracker
            }
        } 

        return max_time;  // return longest safe block 
    }

// Find total safe minutes in the day 
int safe_time() const 
{
    int total_time = 0; 

    for( int i = 0; i < TOTAL_TIME; i++)
    {
        if(!is_time_unsafe[i])           // check function and loop 
        {
            total_time++; 
        }
    }
    return total_time; 
}


// find max swim time in the whole day 
    int max_swim() const 
    {
        int total_swim_time = 0; 
        int current_time_block = 0; 

        for(int i = 0; i < TOTAL_TIME; i++)
        {
            if(!is_time_unsafe[i])
            {
                current_time_block++;       
            }
            else 
            {
                if(current_time_block >= 10)
                {
                    total_swim_time += (current_time_block - 10);
                }

                current_time_block = 0;   
            }
        }

        // handle case if last block reaches end of day
        if(current_time_block >= 10)
        {
            total_swim_time += (current_time_block - 10);
        }

        return total_swim_time;  
    }

// Determine hours where all three sharks appear 
    vector<int> all_three_shark_hours() const 
    {
        vector<bool> hours_seen(6, false);   
        vector<int> total_seen;

        for( int i = 0; i < TOTAL_TIME; i++)
        {
            if(pink_shark[i] && blue_shark[i] && purple_shark[i]) 
            {
                hours_seen[i/60] = true;  
            }
        }

        for ( int hr = 0; hr < 6; hr++)
        {
            if(hours_seen[hr])
            {
                int convert_12_hr = ( hr == 0) ? 12 : hr;  
                total_seen.push_back(convert_12_hr);  
            }
        }
        return total_seen; 
    }

// Step 3: display results 
    void display() const 
    {
        if(!is_valid_beach_day())  
        {
            cout << " Error Invalid Day Time(s)"; 
            return; 
        }

        cout << "The longest safe time block is: " << find_longest_safe_block() << endl; 
        cout << "The total safe time is: " << safe_time() << endl; 
        cout << "The max swim time is: " << max_swim() << endl; 

        vector<int> hours = all_three_shark_hours(); 

        cout << "All three sharks were seen in the hour(s): "; 
        if( hours.empty())
        {
            cout << "No hours"; 
        }
        else 
        {
            for( int h: hours)
                cout << h << ":00 PM "; 

            cout << endl; 
        }
    }

}; 

int main()
{
   Beach b;  

   Sharks pink("Pink", 10); 
   Sharks blue("Blue", 15); 
   Sharks purple("Purple", 3); 

   cout << "Enter 5 start times for the pink shark: \n"; 
   b.input_sharks(pink, b.get_pink()); 

   cout << "Enter 5 start times for the blue shark: \n"; 
   b.input_sharks(blue, b.get_blue()); 

   cout << "Enter 5 start times for the purple shark: \n"; 
   b.input_sharks(purple, b.get_purp()); 

   b.display(); 

   return 0; 
}