/*
 * SmartTrip - Chandigarh Travel Recommendation System
 * ---------------------------------------------------
 * A console-based C project demonstrating core C programming concepts.
 *
 * Build:
 *   gcc -std=c11 -Wall -Wextra -pedantic main.c -o smarttrip
 *
 * Run:
 *   ./smarttrip          (Linux/macOS)
 *   smarttrip.exe        (Windows)
 *
 * Weather is entered by the user; no external API is required.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_DESTINATIONS 20
#define MAX_NAME 64
#define MAX_TYPE 32
#define MAX_WEATHER 24
#define MAX_TRANSPORT 32
#define MAX_LINE 256
#define DATA_FILE "trip_plans.txt"
#define CURRENCY "INR"

/* A union demonstrates that a destination can store one of several
   optional notes while sharing the same memory. */
typedef union {
    char hill_note[64];
    char heritage_note[64];
    char nature_note[64];
} DestinationNote;

typedef struct {
    int id;
    char name[MAX_NAME];
    int distance_km;             /* one-way road-distance estimate */
    double estimated_cost;       /* estimated per-person round-trip cost */
    char type[MAX_TYPE];         /* Hill, Heritage, Nature, Adventure, City */
    char suitable_weather[MAX_WEATHER]; /* Sunny, Cloudy, Rainy, Any */
    char transport[MAX_TRANSPORT];
    int recommended_days;
    DestinationNote note;
    int note_kind;               /* 1=hill, 2=heritage, 3=nature */
} Destination;

typedef struct {
    char weather[MAX_WEATHER];
    double budget;
    int max_distance;
    char preferred_type[MAX_TYPE];
    int days_available;
    char transport[MAX_TRANSPORT];
} Preferences;

typedef struct {
    Destination destination;
    double score;
    double estimated_total;
    int weather_match;
    int type_match;
} Recommendation;

/* ---------- Utility functions ---------- */

static void trim_newline(char *text) {
    size_t length = strlen(text);
    if (length > 0 && text[length - 1] == '\n') {
        text[length - 1] = '\0';
    }
}

static void read_line(const char *prompt, char *buffer, size_t size) {
    int ch;
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        printf("\nInput ended. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    if (strchr(buffer, '\n') == NULL) {
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* discard excess input */
        }
    }
    trim_newline(buffer);
}

static int read_int(const char *prompt, int min_value, int max_value) {
    char buffer[MAX_LINE];
    char *end;
    long value;

    for (;;) {
        read_line(prompt, buffer, sizeof(buffer));
        value = strtol(buffer, &end, 10);
        while (isspace((unsigned char)*end)) {
            end++;
        }
        if (end != buffer && *end == '\0' &&
            value >= min_value && value <= max_value) {
            return (int)value;
        }
        printf("Please enter a whole number between %d and %d.\n",
               min_value, max_value);
    }
}

static double read_double(const char *prompt, double min_value,
                          double max_value) {
    char buffer[MAX_LINE];
    char *end;
    double value;

    for (;;) {
        read_line(prompt, buffer, sizeof(buffer));
        value = strtod(buffer, &end);
        while (isspace((unsigned char)*end)) {
            end++;
        }
        if (end != buffer && *end == '\0' &&
            value >= min_value && value <= max_value) {
            return value;
        }
        printf("Please enter a value between %.2f and %.2f.\n",
               min_value, max_value);
    }
}

static int equals_ignore_case(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

static int is_any(const char *value) {
    return equals_ignore_case(value, "Any");
}

/* ---------- Destination database ---------- */

static void set_destination(Destination *d, int id, const char *name,
                            int distance, double cost, const char *type,
                            const char *weather, const char *transport,
                            int days, int note_kind, const char *note) {
    d->id = id;
    snprintf(d->name, sizeof(d->name), "%s", name);
    d->distance_km = distance;
    d->estimated_cost = cost;
    snprintf(d->type, sizeof(d->type), "%s", type);
    snprintf(d->suitable_weather, sizeof(d->suitable_weather), "%s", weather);
    snprintf(d->transport, sizeof(d->transport), "%s", transport);
    d->recommended_days = days;
    d->note_kind = note_kind;

    if (note_kind == 1) {
        snprintf(d->note.hill_note, sizeof(d->note.hill_note), "%s", note);
    } else if (note_kind == 2) {
        snprintf(d->note.heritage_note, sizeof(d->note.heritage_note), "%s", note);
    } else {
        snprintf(d->note.nature_note, sizeof(d->note.nature_note), "%s", note);
    }
}

static int load_destinations(Destination destinations[]) {
    int count = 0;

    set_destination(&destinations[count++], 1, "Kasauli", 60, 900, "Hill",
                    "Sunny", "Bus / Car", 1, 1, "Quiet hill town and scenic walks");
    set_destination(&destinations[count++], 2, "Shimla", 115, 2200, "Hill",
                    "Cloudy", "Bus / Train / Car", 2, 1, "Hill views and colonial architecture");
    set_destination(&destinations[count++], 3, "Manali", 290, 5200, "Adventure",
                    "Cloudy", "Bus / Car", 3, 1, "Mountain scenery and adventure activities");
    set_destination(&destinations[count++], 4, "Amritsar", 225, 2100, "Heritage",
                    "Sunny", "Bus / Train / Car", 2, 2, "Golden Temple and historic sites");
    set_destination(&destinations[count++], 5, "Dharamshala", 245, 3900, "Nature",
                    "Cloudy", "Bus / Car", 3, 3, "Mountain landscapes and monasteries");
    set_destination(&destinations[count++], 6, "Rishikesh", 210, 2800, "Adventure",
                    "Sunny", "Bus / Car", 2, 3, "Riverfront, rafting season and outdoor activities");
    set_destination(&destinations[count++], 7, "Chandigarh", 0, 500, "City",
                    "Any", "Local transport", 1, 2, "Gardens, museums and architecture");
    set_destination(&destinations[count++], 8, "Morni Hills", 45, 700, "Nature",
                    "Sunny", "Bus / Car", 1, 3, "Lake views and short nature outings");
    set_destination(&destinations[count++], 9, "Kurukshetra", 95, 1100, "Heritage",
                    "Sunny", "Bus / Train / Car", 1, 2, "Museums and places of historical interest");
    set_destination(&destinations[count++], 10, "Ludhiana", 105, 1000, "City",
                    "Any", "Bus / Train / Car", 1, 2, "Urban attractions and local food");
    set_destination(&destinations[count++], 11, "Dalhousie", 315, 4800, "Hill",
                    "Cloudy", "Bus / Car", 3, 1, "Hill station with panoramic viewpoints");
    set_destination(&destinations[count++], 12, "Nahan", 85, 1300, "Nature",
                    "Sunny", "Bus / Car", 1, 3, "Peaceful town and nearby nature spots");

    return count;
}

/* ---------- Recommendation engine ---------- */

static int weather_is_suitable(const char *selected, const char *destination) {
    return is_any(destination) || is_any(selected) ||
           equals_ignore_case(selected, destination);
}

static double calculate_score(const Destination *d, const Preferences *p,
                              int *weather_match, int *type_match,
                              double *estimated_total) {
    double score = 0.0;
    double budget_ratio;

    *weather_match = weather_is_suitable(p->weather, d->suitable_weather);
    *type_match = is_any(p->preferred_type) ||
                  equals_ignore_case(p->preferred_type, d->type);

    /* Base score rewards destinations that fit the user's constraints. */
    score += *weather_match ? 25.0 : 0.0;
    score += *type_match ? 25.0 : 0.0;

    /* Prefer shorter journeys and affordable destinations. */
    score += (p->max_distance > 0)
                 ? 20.0 * (1.0 - (double)d->distance_km / (p->max_distance + 1.0))
                 : 0.0;

    *estimated_total = d->estimated_cost;
    if (p->days_available < d->recommended_days) {
        score -= 20.0;
    } else {
        score += 10.0;
    }

    if (d->estimated_cost <= p->budget) {
        budget_ratio = p->budget > 0 ? d->estimated_cost / p->budget : 1.0;
        score += 10.0 + (1.0 - budget_ratio) * 10.0;
    } else {
        score -= 35.0;
    }

    if (!equals_ignore_case(p->transport, "Any") &&
        strstr(d->transport, p->transport) == NULL) {
        score -= 10.0;
    }

    return score;
}

static int is_eligible(const Destination *d, const Preferences *p) {
    return d->distance_km <= p->max_distance &&
           d->estimated_cost <= p->budget;
}

/* Recursive insertion sort, used to demonstrate recursion. */
static void sort_recommendations(Recommendation items[], int count, int index) {
    int j;
    Recommendation key;

    if (index >= count) {
        return;
    }

    key = items[index];
    j = index - 1;

    while (j >= 0 && items[j].score < key.score) {
        items[j + 1] = items[j];
        j--;
    }
    items[j + 1] = key;

    sort_recommendations(items, count, index + 1);
}

static int build_recommendations(const Destination destinations[], int count,
                                 const Preferences *preferences,
                                 Recommendation results[]) {
    int i;
    int result_count = 0;

    for (i = 0; i < count; i++) {
        if (is_eligible(&destinations[i], preferences)) {
            Recommendation *r = &results[result_count++];
            r->destination = destinations[i];
            r->score = calculate_score(&destinations[i], preferences,
                                       &r->weather_match, &r->type_match,
                                       &r->estimated_total);
        }
    }

    sort_recommendations(results, result_count, 1);
    return result_count;
}

/* ---------- User interaction ---------- */

static void show_welcome(void) {
    printf("\n====================================================\n");
    printf("       SMARTTRIP - CHANDIGARH TRAVEL PLANNER\n");
    printf("====================================================\n");
    printf("Plan a trip using weather, distance, budget and interests.\n");
    printf("Starting point: Chandigarh, India\n\n");
}

static void show_menu(void) {
    printf("\n---------------- MAIN MENU ----------------\n");
    printf("1. Find recommended destinations\n");
    printf("2. View all destinations\n");
    printf("3. Project information\n");
    printf("4. Exit\n");
}

static void collect_preferences(Preferences *p) {
    printf("\nEnter your travel preferences.\n");
    printf("Weather options: Sunny, Cloudy, Rainy, Any\n");
    read_line("Preferred weather: ", p->weather, sizeof(p->weather));

    printf("\nDestination types: Hill, Heritage, Nature, Adventure, City, Any\n");
    read_line("Preferred destination type: ",
              p->preferred_type, sizeof(p->preferred_type));

    p->budget = read_double("Maximum total budget per person (INR): ", 1, 1000000);
    p->max_distance = read_int("Maximum one-way distance from Chandigarh (km): ",
                               0, 2000);
    p->days_available = read_int("How many days are available? ", 1, 30);

    printf("\nTransport options: Bus, Train, Car, Any\n");
    read_line("Preferred transport: ", p->transport, sizeof(p->transport));

    if (p->weather[0] == '\0') {
        snprintf(p->weather, sizeof(p->weather), "Any");
    }
    if (p->preferred_type[0] == '\0') {
        snprintf(p->preferred_type, sizeof(p->preferred_type), "Any");
    }
    if (p->transport[0] == '\0') {
        snprintf(p->transport, sizeof(p->transport), "Any");
    }
}

static void save_destination_to_file(const Destination *d) {
    FILE *file = fopen(DATA_FILE, "a");

    if (file == NULL) {
        perror("Could not open trip plan file");
        return;
    }

    fprintf(file,
            "Selected destination: %s\n"
            "Starting point: Chandigarh\n"
            "Destination ID: %d\n"
            "Distance: %d km one way\n"
            "Estimated cost: INR %.2f per person\n"
            "Type: %s\n"
            "Suitable weather: %s\n"
            "Suggested duration: %d day(s)\n"
            "Available transport: %s\n"
            "----------------------------------------\n",
            d->name, d->id, d->distance_km, d->estimated_cost,
            d->type, d->suitable_weather, d->recommended_days, d->transport);

    fclose(file);
    printf("\n%s (ID %d) added to '%s' successfully!\n",
           d->name, d->id, DATA_FILE);
}

static void show_all_destinations(const Destination destinations[], int count) {
    int i;
    int selected_id;

    printf("\n%-3s %-18s %-10s %-12s %-12s %-8s\n",
           "ID", "Destination", "Distance", "Est. cost", "Type", "Days");
    printf("---------------------------------------------------------------------\n");
    for (i = 0; i < count; i++) {
        printf("%-3d %-18s %-10d %-12.0f %-12s %-8d\n",
               destinations[i].id, destinations[i].name,
               destinations[i].distance_km, destinations[i].estimated_cost,
               destinations[i].type, destinations[i].recommended_days);
    }
    printf("\nCosts and distances are illustrative estimates, not live quotes.\n");
    printf("Enter the ID number of a destination to add it to your trip plans.\n");
    printf("Enter 0 to return to the main menu without saving.\n");
    selected_id = read_int("Destination ID: ", 0, MAX_DESTINATIONS);

    if (selected_id == 0) {
        printf("No destination selected. Returning to the main menu.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        if (destinations[i].id == selected_id) {
            save_destination_to_file(&destinations[i]);
            return;
        }
    }

    printf("No destination exists with ID %d. Nothing was saved.\n", selected_id);
}

static void print_note(const Destination *d) {
    if (d->note_kind == 1) {
        printf("       Note: %s\n", d->note.hill_note);
    } else if (d->note_kind == 2) {
        printf("       Note: %s\n", d->note.heritage_note);
    } else {
        printf("       Note: %s\n", d->note.nature_note);
    }
}

static void display_recommendations(const Recommendation results[], int count) {
    int i;

    printf("\n================ RECOMMENDED TRIPS ================\n");
    if (count == 0) {
        printf("No destinations match both your distance and budget limits.\n");
        printf("Try increasing your budget or maximum distance.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        const Recommendation *r = &results[i];
        printf("\n%d. %s  | Match score: %.1f/100\n",
               i + 1, r->destination.name, r->score);
        printf("   Distance: %d km one way | Estimated cost: INR %.0f/person\n",
               r->destination.distance_km, r->estimated_total);
        printf("   Type: %s | Suitable weather: %s | Suggested days: %d\n",
               r->destination.type, r->destination.suitable_weather,
               r->destination.recommended_days);
        printf("   Transport: %s | Weather match: %s | Type match: %s\n",
               r->destination.transport,
               r->weather_match ? "Yes" : "No",
               r->type_match ? "Yes" : "No");
        print_note(&r->destination);
    }
}

/* ---------- File handling ---------- */

static void save_trip(const Recommendation *r, const Preferences *p) {
    FILE *file = fopen(DATA_FILE, "a");

    if (file == NULL) {
        perror("Could not open trip plan file");
        return;
    }

    fprintf(file,
            "Destination: %s\n"
            "Starting point: Chandigarh\n"
            "Distance: %d km one way\n"
            "Estimated cost: INR %.2f per person\n"
            "Weather preference: %s\n"
            "Trip type: %s\n"
            "Maximum budget: INR %.2f\n"
            "Maximum distance: %d km\n"
            "Available days: %d\n"
            "Transport: %s\n"
            "----------------------------------------\n",
            r->destination.name, r->destination.distance_km,
            r->estimated_total, p->weather, p->preferred_type,
            p->budget, p->max_distance, p->days_available, p->transport);

    fclose(file);
    printf("Trip saved successfully to '%s'.\n", DATA_FILE);
}

static void project_information(void) {
    printf("\nSMARTTRIP - PROJECT INFORMATION\n");
    printf("----------------------------------------\n");
    printf("A menu-driven C application that ranks destinations\n");
    printf("from Chandigarh using user-entered travel preferences.\n\n");
    printf("Concepts demonstrated:\n");
    printf("- Variables, data types, operators and control statements\n");
    printf("- Arrays, strings and string library functions\n");
    printf("- Pointers and dynamic memory allocation\n");
    printf("- Functions, structures, unions and recursion\n");
    printf("- File handling and preprocessor macros\n\n");
    printf("Note: Weather, costs and distances are sample/offline data.\n");
}

static void find_trip(const Destination destinations[], int destination_count) {
    Preferences preferences;
    Recommendation *results;
    int result_count;
    int selection;

    collect_preferences(&preferences);

    /* Dynamic memory allocation: reserve space for all possible results. */
    results = (Recommendation *)malloc(
        (size_t)destination_count * sizeof(Recommendation));

    if (results == NULL) {
        printf("Memory allocation failed. Please try again.\n");
        return;
    }

    result_count = build_recommendations(destinations, destination_count,
                                         &preferences, results);
    display_recommendations(results, result_count);

    if (result_count > 0) {
        printf("\nEnter recommendation number to save (0 to skip).\n");
        selection = read_int("Your choice: ", 0, result_count);
        if (selection > 0) {
            save_trip(&results[selection - 1], &preferences);
        }
    }

    free(results);
}

int main(void) {
    Destination destinations[MAX_DESTINATIONS];
    int destination_count = load_destinations(destinations);
    int choice;

    show_welcome();

    do {
        show_menu();
        choice = read_int("Select an option: ", 1, 4);

        switch (choice) {
            case 1:
                find_trip(destinations, destination_count);
                break;
            case 2:
                show_all_destinations(destinations, destination_count);
                break;
            case 3:
                project_information();
                break;
            case 4:
                printf("\nThank you for using SmartTrip. Happy travelling!\n");
                break;
            default:
                /* read_int prevents this branch from being reached. */
                printf("Invalid option.\n");
        }
    } while (choice != 4);

    return 0;
}
