# SmartTrip – Chandigarh Travel Recommendation System

SmartTrip is a console-based travel recommendation application written in **C**. It helps users explore destinations from Chandigarh by comparing their budget, maximum travel distance, preferred weather, destination type, available days and transport preference.

The program ranks eligible destinations using a simple scoring algorithm and lets users save a selected trip to a text file.

## Features

- Starting location fixed at Chandigarh.
- User-entered weather preference: Sunny, Cloudy, Rainy or Any.
- Budget and maximum one-way distance filters.
- Destination categories: Hill, Heritage, Nature, Adventure and City.
- Transport preference: Bus, Train, Car or Any.
- Ranked recommendations with a match score.
- Estimated per-person trip cost and suggested duration.
- Save selected plans to `trip_plans.txt`.
- Offline destination dataset; no API key or internet connection required.

## Project structure

```text
SmartTrip-Chandigarh/
├── main.c
├── README.md
└── .gitignore
```

`trip_plans.txt` is generated automatically when a user saves a trip. It is intentionally excluded from Git.

## Requirements

- GCC or another C11-compatible compiler.
- Windows, Linux or macOS terminal.

## Compile and run

### Windows (MinGW GCC)

Open Command Prompt or PowerShell in the project folder:

```bash
gcc -std=c11 -Wall -Wextra -pedantic main.c -o smarttrip.exe
.\smarttrip.exe
```

### Linux

```bash
gcc -std=c11 -Wall -Wextra -pedantic main.c -o smarttrip
./smarttrip
```

### macOS

```bash
clang -std=c11 -Wall -Wextra -pedantic main.c -o smarttrip
./smarttrip
```

## How to use

1. Run the program.
2. Select **Find recommended destinations**.
3. Enter weather, destination type, budget, maximum distance, available days and transport preference.
4. Review the ranked recommendations.
5. Enter a recommendation number to save it, or enter `0` to skip.
6. Saved plans are appended to `trip_plans.txt` in the program's working directory.

You can also select **View all destinations** to inspect the built-in destination dataset.

## Sample destinations

The offline sample dataset includes Kasauli, Shimla, Manali, Amritsar, Dharamshala, Rishikesh, Chandigarh, Morni Hills, Kurukshetra, Ludhiana, Dalhousie and Nahan.

## Recommendation logic

The application first filters destinations using the user's maximum distance and budget. Remaining destinations receive a score based on:

- Weather match
- Preferred destination type
- Distance relative to the user's limit
- Whether the available trip duration is sufficient
- How comfortably the estimated cost fits the budget
- Whether the requested transport option appears in the destination's transport options

The score is a project-level heuristic, not a guaranteed prediction of trip quality. A destination with a lower score may still appear if it meets the hard budget and distance filters.

## Important data disclaimer

All distances and estimated costs are **illustrative sample values** intended for demonstrating C programming. They are not live route distances, current fares, hotel prices or verified travel quotes. Actual costs depend on travel dates, transport, accommodation and personal choices.

Weather is entered manually by the user. SmartTrip does not fetch live weather data.

## C syllabus mapping

| C programming concept | Where it is demonstrated |
|---|---|
| Variables and data types | Preferences, destination details and scores |
| Input/output | Menu, prompts and recommendation display |
| Operators and expressions | Budget checks and recommendation scoring |
| `if`, `else`, `switch` | Validation, scoring and menu selection |
| Loops | Input validation and destination processing |
| Arrays | Destination and recommendation collections |
| Strings and string functions | Destination names, categories and comparisons |
| Pointers | Structure pointers and function parameters |
| Dynamic memory allocation | `malloc()` and `free()` for recommendation storage |
| Functions | Modular program design |
| Structures | `Destination`, `Preferences`, `Recommendation` |
| Union | Alternative destination notes |
| Recursion | Recursive insertion sort |
| File handling | Saving selected trips using `fopen`, `fprintf`, `fclose` |
| Macros | Constants defined using `#define` |

> **Note:** The program uses a union to demonstrate the concept. The active note member is selected by `note_kind`; only that member should be read.

## Possible future improvements

- Read destination data from a CSV or text file.
- Add user accounts and trip history.
- Add return-trip and group-size cost estimates.
- Add a live weather service in a later version.
- Add more destinations and configurable transport fares.
- Improve the recommendation algorithm with user ratings.

## License

This project is provided for educational use. You may adapt it for coursework and personal learning.
