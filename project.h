#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COMMAND_BYTES 8192 /* Maximum number of bytes possibly allocated for a command */
#define MAX_PARKS 20 /* Maximum number of parking lots that can ever exist */
#define LICENSE_PLATE_SIZE 9 /* Number of bytes occupied by a license plate */
#define DATE_SIZE 11 /* Number of bytes occupied by a date */
#define TIME_SIZE 6 /* Number of bytes occupied by a time */

/* Car element structure */
typedef struct Car_s {
    float value; /* Amount charged */
    char license_plate[LICENSE_PLATE_SIZE], entry_date[DATE_SIZE], exit_date[DATE_SIZE], entry_time[TIME_SIZE], exit_time[TIME_SIZE];
    struct Car_s *next; /* Pointer to the next car in a linked list */
    struct Car_s *prev; /* Pointer to the previous car in a linked list */
    struct Car_s *next_exit; /* Pointer to the next car in a linked list of car exits */
} Car;

/* Parking lot element structure */
typedef struct Park_s {
    char name[MAX_COMMAND_BYTES];
    int maximum_capacity, available_spaces;
    float X; /* Represents the amount charged per 15 minutes during the first hour */
    float Y; /* Represents the amount charged per 15 minutes after the first hour */
    float Z; /* Represents the maximum amount charged after 24 hours */
    Car *Car_List_Head, *Car_List_Tail; /* Head and Tail of the parking lot's car linked list */
    Car *Exit_List_Head, *Exit_List_Tail; /* Head and Tail of the parking lot's exit linked list */
} Park;

/*~~~~~~~~~~~~~~~~~~~~~~~COMMANDS~~~~~~~~~~~~~~~~~~~~~~~~~*/

/** @brief Command line where the command and parameters are received
 * @param park_array Pointer to an array of parking lots
 * @return void
 */
void command(Park (*park_array)[]);

/** @brief Displays a list of all created parking lots and their
 * respective maximum capacities and available spaces.
 * @param park_array Pointer to an array of parking lots
 * @return void
 */
void show_park(Park (*park_array)[]);

/** @brief Creates a parking lot and adds it to an array of parking lots
 * @param park_array Pointer to an array of parking lots
 * @param parameters Command line parameters
 * @return void
 */
void create_park(Park (*park_array)[], char parameters[]);

/** @brief Registers a car entering a parking lot and updates
 * the most recent date and time used throughout the program, if successful.
 * @param park_array Pointer to an array of parking lots
 * @param parameters Command line parameters
 * @param recent_date Most recently used date throughout the program
 * @param recent_time Most recently used time throughout the program
 * @return void
 */
void register_entry(Park (*park_array)[], char parameters[], char recent_date[], char recent_time[]);

/** @brief Registers a car leaving a parking lot, updating
 * the most recent date and time used throughout the program, if successful.
 * @param park_array Pointer to an array of parking lots
 * @param parameters Command line parameters
 * @param recent_date Most recently used date throughout the program
 * @param recent_time Most recently used time throughout the program
 * @return void
 */
void register_exit(Park (*park_array)[], char parameters[], char recent_date[], char recent_time[]);

/** @brief Displays a car's entries and exits, identified by its
 * license plate, in all parking lots, sorted by name and parking date.
 * If a car has entered and has not left a parking lot, only the entry time is displayed.
 * @param park_array Pointer to an array of parking lots
 * @param parameters Command line parameters
 * @return void
 */
void entries_and_exits(Park (*park_array)[], char parameters[]);

/** @brief Displays the revenue of a parking lot over time if no date parameter is provided.
 * If a date is provided in the parameters, displays the parking lot's revenue for that day,
 * sorted by exit time.
 * @param park_array Pointer to an array of parking lots
 * @param parameters Command line parameters
 * @return void
 */
void revenue(Park (*park_array)[], char parameters[]);

/** @brief Removes a parking lot and its respective car entries and exits.
 * @param park_array Pointer to an array of parking lots.
 * @param parameters Command line parameters.
 * @return void
 */
void remove_park(Park (*park_array)[], char parameters[]);


/*~~~~~~~~~~~~~~~~~~~~~~~~HELPERS~~~~~~~~~~~~~~~~~~~~~~~~*/

/** @brief Initializes all parking lots in the parking lot array to 0 or NULL
 * to prevent errors later on.
 * @param park_array Pointer to an array of parking lots
 * @return void
 */
void initialize_parks(Park (*park_array)[]);

/** @brief Checks whether a license plate is valid.
 * @param license_plate License plate to check.
 * @return 1 if it is a valid license plate; 0 otherwise
 */
int is_license_plate(char license_plate[]);

/** @brief Checks whether the given parameter is a valid date.
 * @param date Date to check
 * @return 1 if it is a valid date; 0 otherwise
 */
int is_date(char date[]);

/** @brief Checks whether the given parameter is a valid time.
 * @param time Time to check
 * @return 1 if it is a valid time; 0 otherwise
 */
int is_time(char time[]);

/** @brief Checks whether the given parameters represent a valid pricing scheme.
 * @param x Amount charged during the first hour of parking
 * @param y Amount charged after the first hour of parking
 * @param z Amount charged after 24 hours in a parking lot, or the maximum
 * amount that can be charged per day.
 * @return 1 if it is a valid pricing scheme; 0 otherwise
 */
int is_rate(float x, float y, float z);

/** @brief Checks whether a car is parked in a specific parking lot.
 * @param parking Pointer to the parking lot that will be searched
 * @param license_plate License plate to search for in the parking lot
 * @param car_ptr Pointer to a car pointer that will be modified to point
 * to the car if it is found in the parking lot
 * @return 1 if the car is found in the parking lot; 0 otherwise
 */
int car_parked(Park *parking, char *license_plate, Car **car_ptr);

/** @brief Adds an entry to a parking lot and updates the most recent date and time.
 * @param park Pointer to the parking lot
 * @param car Pointer to a car
 * @param recent_date The most recent date in the program
 * @param recent_time The most recent time in the program
 * @return void
 */
void add_car_entry(Park *park, Car *car, char recent_date[], char recent_time[]);

/** @brief Adds an exit to a parking lot and updates the most recent date and time.
 * @param park Pointer to the parking lot
 * @param car Pointer to a car
 * @param exit_date Car's exit date from the parking lot
 * @param exit_time Car's exit time from the parking lot
 * @param recent_date The most recent date in the program
 * @param recent_time The most recent time in the program
 * @return void
 */
void add_car_exit(Park *park, Car *car, char exit_date[], char exit_time[], char recent_date[], char recent_time[]);

/** @brief Calculates the amount charged between specific dates/times given a pricing scheme.
 * @param entry_date Entry date
 * @param entry_time Entry time
 * @param exit_date Exit date
 * @param exit_time Exit time
 * @param x Amount charged per 15 minutes during the first hour.
 * @param y Amount charged per 15 minutes after the first hour.
 * @param z Amount charged after 24 hours or the maximum daily amount.
 * @return Amount charged during that time interval
 */
float car_billing(char entry_date[], char entry_time[], char exit_date[], char exit_time[], float x, float y, float z);

/** @brief Sorts parking lots alphabetically by name using bubble sort.
 * @param park_array Pointer to an array of parking lots
 * @return void
 */
void sort_parks(Park (*park_array)[]);

/** @brief Swaps two parking lots.
 * @param park1 Parking lot to be swapped
 * @param park2 Parking lot to be swapped
 * @return void
 */
void swap_parks(Park *park1, Park *park2);

/** @brief Compares two dates and determines which one is more recent.
 * @param date1 First date to compare
 * @param date2 Second date to compare
 * @return 1 if date1 > date2; 0 if date1 = date2; -1 if date1 < date2
 */
int compare_dates(char date1[], char date2[]);

/** @brief Compacts all parking lots in an array as far to the "left" as possible.
 * @param park_array Pointer to an array of parking lots
 * @return void
 */
void compact_parks(Park (*park_array)[]);

/** @brief Copies a specific parking lot to a destination parking lot.
 * @param destination_park Pointer to the destination parking lot that will
 * receive the copied information
 * @param copied_park Parking lot that will be copied to another parking lot
 * @return void
 */
void copy_park(Park *destination_park, Park copied_park);

/** @brief Compares two dates and times and determines which one is more recent.
 * @param test_date First date to test
 * @param test_time First time to test
 * @param recent_date Second date to test
 * @param recent_time Second time to test
 * @return 1 if the first complete date/time is more recent, 0 if they are equal,
 * -1 if the second complete date/time is more recent
 */
int chronological_validation(char test_date[], char test_time[], char recent_date[], char recent_time[]);

/** @brief Frees a parking lot and all its registered entries and exits.
 * @param park Pointer to the parking lot that will be freed.
 * @return void
 */
void free_park(Park *park);