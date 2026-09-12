/* iaed24 - ist1110339 - project */

#include "project.h"

/**
 * @file project.c
 * @brief Parking lot management system.
 *
 * The goal is to build a parking lot management system. The system allows
 * the definition of parking lots and the registration of vehicle entries
 * and exits, as well as their consultation and billing.
 *
 * Interaction with the program should occur through a set of lines composed
 * of a letter (command) and a number of arguments depending on the command.
 *
 * @author Miguel Mateus
 * @bug No known bugs.
 */

/*~~~~~~~~~~~~~~~~~~~~~~~~~MAIN~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/** @brief Program entry point
 * @return returns 0
 */
int main(){
    Park park_array[MAX_PARKS]; /* Parking lot array */

    initialize_parks(&park_array);
    command(&park_array);

    /* Free all parking lots */
    for (int i = MAX_PARKS-1; i >= 0; i--){
        if (park_array[i].name[0] != '\0')
            free_park(&park_array[i]);
    }
    return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~COMMANDS~~~~~~~~~~~~~~~~~~~~~~~~~*/

void command(Park (*park_array)[]){
    char recent_date[DATE_SIZE], recent_time[TIME_SIZE];
    char c, first_letter, rest_command[MAX_COMMAND_BYTES];
    int i, state = 1;

    strcpy(recent_date, "00-00-0000");
    strcpy(recent_time, "00:00");
    while(state == 1){
        i = 0;
        first_letter = '\0';
        rest_command[0] = '\0';
        while ((c = getchar()) != '\n')
        {
            if (i == 0)
                first_letter = c;
            else if (i > 1){
                rest_command[i - 2] = c;
                rest_command[i - 1] = '\0';
            }
            i++;
        }
        switch (first_letter) {
            case 'q':
                state = 0;
                break;
            case 'p':
                if (strlen(rest_command) == 0) show_park(park_array);
                else create_park(park_array, rest_command);
                break;
            case 'e':
                register_entry(park_array, rest_command, recent_date, recent_time);
                break;
            case 's':
                register_exit(park_array, rest_command, recent_date, recent_time);
                break;
            case 'v':
                entries_and_exits(park_array, rest_command);
                break;
            case 'f':
                revenue(park_array, rest_command);
                break;
            case 'r':
                remove_park(park_array, rest_command);
                break;
            default:
                break;
        }
    }
}

void show_park(Park (*park_array)[])
{
    for (int i = 0; i < MAX_PARKS; i++)
        if (strlen((*park_array)[i].name) != 0)
            printf("%s %d %d\n", (*park_array)[i].name,
            (*park_array)[i].maximum_capacity,
            (*park_array)[i].available_spaces);
        else break;
}

void create_park(Park (*park_array)[], char parameters[])
{
    Park new_park;

    /* Initialize linked lists */
    new_park.Car_List_Head = NULL;
    new_park.Exit_List_Head = NULL;
    new_park.Car_List_Tail = NULL;
    new_park.Exit_List_Tail = NULL;

    if (parameters[0] == '"'){
        sscanf(parameters, "\"%[^\"]\" %d %f %f %f", new_park.name,
        &new_park.maximum_capacity, &new_park.X, &new_park.Y, &new_park.Z);
    }
    else
        sscanf(parameters, "%s %d %f %f %f", new_park.name,
        &new_park.maximum_capacity, &new_park.X, &new_park.Y, &new_park.Z);

    new_park.available_spaces = new_park.maximum_capacity;

    int empty_position = -1;
    int found = 0;
    for (int i = 0; i < MAX_PARKS; i++){
        if (strcmp((*park_array)[i].name, new_park.name) == 0){
            printf("%s: parking already exists.\n", new_park.name);
            return;
        }

        if ((strlen((*park_array)[i].name) == 0) && (found == 0)){
            empty_position = i;
            found = 1;
        }
    }

    if (new_park.maximum_capacity <= 0){
        printf("%d: invalid capacity.\n", new_park.maximum_capacity);
        return;
    }
    if (is_rate(new_park.X, new_park.Y, new_park.Z) == 0){
        printf("invalid cost.\n");
        return;
    }
    if (empty_position == -1){
        printf("too many parks.\n");
        return;
    }
    
    copy_park(&(*park_array)[empty_position], new_park);
}

void register_entry(Park (*park_array)[], char parameters[], char recent_date[], char recent_time[])
{
    Car *parked_car = NULL;
    Car *new_car = malloc(sizeof(Car));
    char name[MAX_COMMAND_BYTES];
    new_car->value = 0;

    if (parameters[0] == '"'){
        sscanf(parameters, "\"%[^\"]\" %s %s %s", name, new_car->license_plate,
        new_car->entry_date, new_car->entry_time);
    }
    else
        sscanf(parameters, "%s %s %s %s", name, new_car->license_plate,
        new_car->entry_date, new_car->entry_time);

    for (int i = 0; i < MAX_PARKS; i++){
        if (strcmp((*park_array)[i].name, name) == 0){
            if ((*park_array)[i].available_spaces > 0){
                if (is_license_plate(new_car->license_plate) == 0){
                    printf("%s: invalid licence plate.\n", new_car->license_plate);
                    free(new_car);
                    return;
                }
                for (int j = 0; j < MAX_PARKS; j++){
                    if ((*park_array)[j].name[0] == '\0') break;
                    if (car_parked(&(*park_array)[j], new_car->license_plate, &parked_car) == 1){
                        printf("%s: invalid vehicle entry.\n", new_car->license_plate);
                        free(new_car);
                        return;
                    }
                }
                if ((is_date(new_car->entry_date) == 0) || (is_time(new_car->entry_time) == 0)
                || chronological_validation(new_car->entry_date, new_car->entry_time, recent_date, recent_time) == 0){
                    printf("invalid date.\n");
                    free(new_car);
                    return;
                }
                add_car_entry(&(*park_array)[i], new_car, recent_date, recent_time);
                return;
            }
            else{
                printf("%s: parking is full.\n", name);
                free(new_car);
                return;
            }
        }
    }
    printf("%s: no such parking.\n", name);
    free(new_car);
    return;
}

void register_exit(Park (*park_array)[], char parameters[], char recent_date[], char recent_time[]){
    char name[MAX_COMMAND_BYTES];
    char license_plate[LICENSE_PLATE_SIZE], exit_date[DATE_SIZE], exit_time[TIME_SIZE];
    Car *parked_car;

    if (parameters[0] == '"'){
        sscanf(parameters, "\"%[^\"]\" %s %s %s", name, license_plate, exit_date, exit_time);
    }
    else
        sscanf(parameters, "%s %s %s %s", name, license_plate, exit_date, exit_time);

    for (int i = 0; i < MAX_PARKS; i++){
        if (strcmp((*park_array)[i].name, name) == 0){
            if (is_license_plate(license_plate) == 0){
                printf("%s: invalid licence plate.\n", license_plate);
                return;
            }
            if (car_parked(&(*park_array)[i], license_plate, &parked_car) == 0){
                printf("%s: invalid vehicle exit.\n", license_plate);
                return;
            }
            if ((is_date(exit_date) == 0)|| (is_time(exit_time) == 0)
            || chronological_validation(exit_date, exit_time, recent_date,
             recent_time) == 0){
                printf("invalid date.\n");
                return;
            }

            add_car_exit(&(*park_array)[i], parked_car, exit_date,
            exit_time, recent_date, recent_time);
            printf("%s %s %s %s %s %.2f\n", license_plate, parked_car->entry_date,
            parked_car->entry_time, exit_date, exit_time, parked_car->value);
            return;
        }
    }
    printf("%s: no such parking.\n", name);
    return;
}

void entries_and_exits(Park (*park_array)[], char parameters[]){
    int print_used = 0; /* Identifies whether anything has ever been printed */
    char license_plate[LICENSE_PLATE_SIZE];
    Car *current;
    Park temporary_array[MAX_PARKS];


    /* Sort a copy to avoid changing the original parking lot array */
    for (int i = 0; i < MAX_PARKS; i++)
        copy_park(&temporary_array[i], (*park_array)[i]);
    sort_parks(&temporary_array);

    sscanf(parameters, "%s", license_plate);

    if (is_license_plate(license_plate) == 0){
        printf("%s: invalid licence plate.\n", license_plate);
        return;
    }

    for (int j = 0; j < MAX_PARKS; j++){
        if (temporary_array[j].name[0] != '\0'){
            current = temporary_array[j].Car_List_Head;
            while (current != NULL){
                if (strcmp(current->license_plate, license_plate) == 0){
                    printf("%s %s %s", temporary_array[j].name,
                    current->entry_date, current->entry_time);
                    print_used += 1;

                    /* Print exit information if available */
                    if (strcmp(current->exit_date, "00-00-0000") != 0)
                        printf(" %s %s\n", current->exit_date, current->exit_time);
                    else
                        printf("\n");
                }
                current = current->next;
            }
        }
    }
    if (print_used == 0){
        printf("%s: no entries found in any parking.\n", license_plate);
        return;
    }
}

void revenue(Park (*park_array)[], char parameters[]){
    char name[MAX_COMMAND_BYTES], date[DATE_SIZE], current_date[DATE_SIZE], final_date[DATE_SIZE];
    float value;
    date[0] = '\0';

    if (parameters[0] == '"')
        // If the first character is a quotation mark, the parking lot name is enclosed in quotation marks
        sscanf(parameters, "\"%[^\"]\" %s", name, date);
    else
        // Otherwise, there are no quotation marks around the parking lot name
        sscanf(parameters, "%s %s", name, date);

    if (date[0] == '\0'){
        for (int i = 0; i < MAX_PARKS; i++){
            if (strcmp((*park_array)[i].name, name) == 0){
                Car *current = (*park_array)[i].Exit_List_Head;
                while(current != NULL){
                    strcpy(current_date, current->exit_date);
                    value = 0;
                    while(strcmp(current_date, current->exit_date) == 0){
                        value += current->value;
                        current = current->next_exit;
                        if(current == NULL) break;
                    }
                    printf("%s %.2f\n", current_date, value);
                }
                return;
            }
        }
        printf("%s: no such parking.\n", name);
        return;
    }
    else{
        for (int i = 0; i < MAX_PARKS; i++){
            if (strcmp((*park_array)[i].name, name) == 0){
                if ((is_date(date) == 0)){
                    printf("invalid date.\n");
                    return;
                }
                if (((*park_array)[i].Car_List_Tail == NULL) && ((*park_array)[i].Exit_List_Tail == NULL)){
                    printf("invalid date.\n");
                    return;
                }
                else{
                    if ((*park_array)[i].Exit_List_Tail != NULL){
                        if (compare_dates((*park_array)[i].Exit_List_Tail->exit_date, (*park_array)[i].Car_List_Tail->entry_date) == -1)
                            strcpy(final_date, (*park_array)[i].Car_List_Tail->entry_date);
                        else
                            strcpy(final_date, (*park_array)[i].Exit_List_Tail->exit_date);

                        if (compare_dates(date, final_date) == 1){
                                printf("invalid date.\n");
                                return;
                        }
                    }
                    else{
                        if (compare_dates(date, (*park_array)[i].Car_List_Tail->entry_date) == 1){
                            printf("invalid date.\n");
                            return;
                        }
                    }
                }

                Car *current = (*park_array)[i].Exit_List_Head;
                while(current != NULL){
                    if(strcmp(current->exit_date, date) == 0){
                        printf("%s %s %.2f\n", current->license_plate, current->exit_time, current->value);
                        current = current->next_exit;
                    }
                    else{
                        current = current->next_exit;
                    }
                }
                return;
            }
        }
        printf("%s: no such parking.\n", name);
        return;
    }
}

void remove_park(Park (*park_array)[], char parameters[]){
    char name[MAX_COMMAND_BYTES];
    int park_found = 0;
    Park sorted_copy[MAX_PARKS];

    if (parameters[0] == '"')
        // If the first character is a quotation mark, the parking lot name is enclosed in quotation marks
        sscanf(parameters, "\"%[^\"]\"", name);
    else
        // Otherwise, there are no quotation marks around the parking lot name
        sscanf(parameters, "%s", name);

    for (int i = 0; i < MAX_PARKS; i++){
        if (strcmp(name, (*park_array)[i].name) == 0){
            park_found = 1;
            free_park(&(*park_array)[i]);
        }
    }
    if (park_found == 0){
        printf("%s: no such parking.\n", name);
        return;
    }

    /* Compact the array */
    compact_parks(park_array);

    /* Create a copy for sorting */
    for (int k = 0; k < MAX_PARKS; k++)
        copy_park(&sorted_copy[k], (*park_array)[k]);

    /* Sort the copy alphabetically */
    sort_parks(&sorted_copy);

    for (int j = 0; j < MAX_PARKS; j++)
        if (sorted_copy[j].name[0] != '\0')
            printf("%s\n", sorted_copy[j].name);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~HELPERS~~~~~~~~~~~~~~~~~~~~~~~~*/

void initialize_parks(Park (*park_array)[]){
    for (int f = 0; f < MAX_PARKS; f++){
        (*park_array)[f].maximum_capacity = 0;
        (*park_array)[f].available_spaces = 0;
        (*park_array)[f].name[0] = '\0';
        (*park_array)[f].X = 0.00;
        (*park_array)[f].Y = 0.00;
        (*park_array)[f].Z = 0.00;
        (*park_array)[f].Car_List_Head = NULL;
        (*park_array)[f].Car_List_Tail = NULL;
        (*park_array)[f].Exit_List_Head = NULL;
        (*park_array)[f].Exit_List_Tail = NULL;
    }
}

int is_license_plate(char license_plate[])
{
    char pair1[3], pair2[3], pair3[3];
    int number_pairs = 0, letter_pairs = 0;

    if (strlen(license_plate) == 8 && license_plate[2] == '-' && license_plate[5] == '-')
    {
        sscanf(license_plate, "%2s-%2s-%2s", pair1, pair2, pair3);

        /* Check whether each pair consists of uppercase letters
        or numbers. */
        if (isdigit(pair1[0]) && isdigit(pair1[1])) number_pairs++;
        else if (isupper(pair1[0]) && isupper(pair1[1])) letter_pairs++;

        if (isdigit(pair2[0]) && isdigit(pair2[1])) number_pairs++;
        else if (isupper(pair2[0]) && isupper(pair2[1])) letter_pairs++;

        if (isdigit(pair3[0]) && isdigit(pair3[1])) number_pairs++;
        else if (isupper(pair3[0]) && isupper(pair3[1])) letter_pairs++;

        /* Check whether the correct number of letter and number pairs exists */
        if ((number_pairs == 1 && letter_pairs == 2) ||
        (number_pairs == 2 && letter_pairs == 1)) return 1;
        else return 0;
    }
    else return 0;
}

int is_date(char date[])
{
    int year, month, day;
    int days_in_month;

    if (strlen(date) != 10 || date[2] != '-' || date[5] != '-')
        return 0;

    if (sscanf(date, "%d-%d-%d", &day, &month, &year) != 3)
        return 0;

    if (month < 1 || month > 12 || day < 1)
        return 0;

    switch (month) {
        case 2:
            if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
                days_in_month = 29;
            else
                days_in_month = 28;
            break;

        case 4: case 6: case 9: case 11:
            days_in_month = 30;
            break;

        default:
            days_in_month = 31;
            break;
    }

    return day <= days_in_month;
}

int is_time(char time[])
{
    int hours, minutes;
    char time_with_0[TIME_SIZE];

    if ((strlen(time) == 5 && time[2] == ':')
    || (strlen(time) == 4 && time[1] == ':')){
        sscanf(time, "%d:%d", &hours, &minutes);

        /* Add leading zero to single-digit hours */
        if (strlen(time) == 4){
            time_with_0[0] = '0';
            strcpy(time_with_0 + 1, time);
            strcpy(time, time_with_0);
        }
        return (0 <= hours && hours <= 23 && 0 <= minutes && minutes <= 59)? 1 : 0;
    }
    else return 0;
}

int is_rate(float X, float Y, float Z)
{
    // Check whether the costs are valid and increasing
    return ((X <=0) || (Y <=0) || (Z <=0) || (X >= Y) || (Y >= Z))? 0 : 1;
}

int car_parked(Park *parking, char license_plate[], Car **car_ptr){
    Car *current_car;

    current_car = parking->Car_List_Head;

    /* Find a currently parked car */
    while(
        current_car != NULL && ((strcmp(current_car->license_plate, license_plate) != 0)
        || (current_car->value != 0)
        || (strcmp(current_car->exit_date, "00-00-0000") != 0))
    )
        current_car = current_car->next;

    if (current_car == NULL){
        *car_ptr = NULL;
        return 0;
    }
    else{
        *car_ptr = current_car;
        return 1;
    }
}

void add_car_entry(Park *park, Car *car, char recent_date[], char recent_time[]){

    car->next = NULL;

    if (park->Car_List_Head == NULL){
        car->prev = NULL;
        park->Car_List_Head = car;
    }
    else{
        park->Car_List_Tail->next = car;
        car->prev = park->Car_List_Tail;
    }

    park->Car_List_Tail = car;
    park->available_spaces--;

    /* Exit date and time are declared as "null", i.e. set to 0s */
    strcpy(car->exit_date, "00-00-0000");
    strcpy(car->exit_time, "00:00");

    strcpy(recent_date, car->entry_date);
    strcpy(recent_time, car->entry_time);

    printf("%s %d\n", park->name, park->available_spaces);
}

void add_car_exit(Park *park, Car *car, char exit_date[], char exit_time[], char recent_date[], char recent_time[]){

    strcpy(car->exit_date, exit_date);
    strcpy(car->exit_time, exit_time);
    car->value = car_billing(car->entry_date, car->entry_time,
     exit_date, exit_time, park->X, park->Y, park->Z);
    park->available_spaces++;

    car->next_exit = NULL;
    if (park->Exit_List_Head == NULL)
        park->Exit_List_Head = car;
    else
        park->Exit_List_Tail->next_exit = car;

    park->Exit_List_Tail = car;

    strcpy(recent_date, exit_date);
    strcpy(recent_time, exit_time);
}

float car_billing(char entry_date[], char entry_time[], char exit_date[], char exit_time[], float X, float Y, float Z){
    float total_minutes = 0, final_bill = 0;
    float minutes1 = 0, minutes2 = 0;
    float k;
    int i = 0, j = 0;
    int day1, day2, month1, month2, year1, year2, hour1, hour2, initial_minutes1, initial_minutes2;
    const int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    sscanf(entry_date, "%d-%d-%d", &day1, &month1, &year1);
    sscanf(exit_date, "%d-%d-%d", &day2, &month2, &year2);
    sscanf(entry_time, "%d:%d", &hour1, &initial_minutes1);
    sscanf(exit_time, "%d:%d", &hour2, &initial_minutes2);

    /* Convert dates to total days */

    long int total_days1 = day1;
    for (int i = 0; i < month1-1; i++) {
        total_days1 += days_per_month[i];
    }

    long int total_days2 = day2;
    for (int i = 0; i < month2-1; i++) {
        total_days2 += days_per_month[i];
    }

    total_days1 += (year1 * 365);
    total_days2 += (year2 * 365);

    /* Convert times to minutes */
    minutes1 = hour1*60 + initial_minutes1;
    minutes2 = hour2*60 + initial_minutes2;

    /* Calculate total parking time in minutes */
    total_minutes = (total_days2 - total_days1)*24*60 + minutes2 - minutes1;

    /* Charge Z for each complete 24-hour period */
    if (total_minutes/60 > 24){
        k = total_minutes/(60*24);
        while(k > 1){
            final_bill += Z;
            total_minutes -= 60*24;
            k--;
        }
    }

    /* Round parking time up to the next 15-minute period */
    float total_15_minutes = total_minutes/15;
    if (total_15_minutes > (int)total_15_minutes)
        total_15_minutes = (int)total_15_minutes + 1;

    if (total_15_minutes < 5)
        i = total_15_minutes;

    else{
        i = 4;
        total_15_minutes -= 4;
        j = total_15_minutes;
    }

    if ((i*X + j*Y) < Z)
        final_bill += i*X + j*Y;
    else
        final_bill += Z;

    return final_bill;
}

void sort_parks(Park (*park_array)[]){
    int i,j;
    for (i = 0; i < MAX_PARKS - 1; i++)
        for (j = 0; j < MAX_PARKS - i - 1; j++)
            if (strcmp((*park_array)[j].name, (*park_array)[j + 1].name) > 0)
                swap_parks(&(*park_array)[j], &(*park_array)[j + 1]);
}

void swap_parks(Park *park1, Park *park2){
    Park aux;

    copy_park(&aux, *park1);
    copy_park(park1, *park2);
    copy_park(park2, aux);
}

int compare_dates(char date1[], char date2[]){
    int day1, day2, month1, month2, year1, year2;
    sscanf(date1, "%d-%d-%d", &day1, &month1, &year1);
    sscanf(date2, "%d-%d-%d", &day2, &month2, &year2);

    if (year1 == year2){
        if (month1 == month2)
            if (day1 == day2) return 0;
            else
                return (day1 > day2)? 1 : -1;

        else
            return (month1 > month2)? 1 : -1;
    }
    else
        return (year1 > year2)? 1 : -1;
}

void compact_parks(Park (*park_array)[]){
    int empty_position = -1;
    int i;

    for (i = 0; i < MAX_PARKS; i++){
        if (((*park_array)[i].name[0] == '\0') && (empty_position == -1))
            empty_position = i;

        else if (((*park_array)[i].name[0] != '\0') && (empty_position != -1)){
            for (int j = empty_position; j < MAX_PARKS-1; j++){
                swap_parks(&(*park_array)[j], &(*park_array)[j+1]);
            }
            return;
        }
    }
}

void copy_park(Park *destination_park, Park copied_park){
    strcpy(destination_park->name, copied_park.name);
    destination_park->maximum_capacity = copied_park.maximum_capacity;
    destination_park->available_spaces = copied_park.available_spaces;
    destination_park->X = copied_park.X;
    destination_park->Y = copied_park.Y;
    destination_park->Z = copied_park.Z;
    destination_park->Car_List_Head = copied_park.Car_List_Head;
    destination_park->Car_List_Tail = copied_park.Car_List_Tail;
    destination_park->Exit_List_Head = copied_park.Exit_List_Head;
    destination_park->Exit_List_Tail = copied_park.Exit_List_Tail;
}

int chronological_validation(char test_date[], char test_time[], char recent_date[], char recent_time[]){
    int hour1, hour2, min1, min2;

    sscanf(test_time, "%d:%d", &hour1, &min1);
    sscanf(recent_time, "%d:%d", &hour2, &min2);

    if (compare_dates(test_date, recent_date) == 1) return 1;
    else if (compare_dates(test_date, recent_date) == -1) return 0;
    else{
        if (hour1 > hour2) return 1;
        else if (hour1 < hour2) return 0;
        else{
            return (min1 < min2)? 0 : 1;
        }
    }
}

void free_park(Park *park){
    Car *current;

    /* Case where the list is not empty */
    if (park->Car_List_Head != NULL){
        current = park->Car_List_Head;

        /* Case where the list only had 1 car */
        if (current->next == NULL){
            free(current);
            park->name[0] = '\0';
            park->Car_List_Head = NULL;
            park->Exit_List_Head = NULL;
            park->Car_List_Tail = NULL;
            park->Exit_List_Tail = NULL;
            park->maximum_capacity = 0;
            park->available_spaces = 0;
            park->X = 0;
            park->Y = 0;
            park->Z = 0;
            return;
        }
        /* Case where there is more than 1 car in the list */
        current = current->next;
        while (current != NULL){
            if (current->next == NULL){
                free(current->prev);
                free(current);
                break;;
            }
            else
                free(current->prev);
            current = current->next;
        }
    }
    park->name[0] = '\0';
    park->Car_List_Head = NULL;
    park->Exit_List_Head = NULL;
    park->Car_List_Tail = NULL;
    park->Exit_List_Tail = NULL;
    park->maximum_capacity = 0;
    park->available_spaces = 0;
    park->X = 0;
    park->Y = 0;
    park->Z = 0;
}
