# Parking Management System

A command-line parking management system developed in C as part of the **Introduction to Algorithms and Data Structures (IAED)** course at **Instituto Superior Técnico**.


The system manages parking lots, vehicle entries and exits, vehicle history and parking revenue, while enforcing chronological records and the project's billing rules.

## Features

* Create and list parking lots
* Register vehicle entries and exits
* Track vehicle parking history
* Calculate parking fees
* Calculate parking lot revenue for a given period
* Remove parking lots
* Sort and compact parking lot data
* Validate dates, license plates and parking lot information
* Handle vehicles remaining in a parking lot for multiple days
* Handle the special closure of February 29th

## Commands

| Command | Description                      |
| ------- | -------------------------------- |
| `q`     | Quit the program                 |
| `p`     | Create or list parking lots      |
| `e`     | Register a vehicle entry         |
| `s`     | Register a vehicle exit          |
| `v`     | Show a vehicle's parking history |
| `f`     | Calculate parking lot revenue    |
| `r`     | Remove a parking lot             |

## Billing System

Each parking lot defines three billing values:

* **X** — price per 15 minutes during the first hour
* **Y** — price per 15 minutes after the first hour
* **Z** — maximum charge for a period of up to 24 hours

The first hour is divided into four 15-minute periods, charged using `X`. Each additional 15-minute period is charged using `Y`.

For stays shorter than 24 hours, the total charge is capped at `Z`.

For stays longer than 24 hours, each complete 24-hour period costs `Z`, with the remaining time calculated separately and also capped at `Z`.

February 29th is treated as a closed day. Vehicles cannot enter or leave a parking lot on that day, and the corresponding 24-hour period is not charged to vehicles that remain parked across it.

## Implementation

The system is implemented in C using:

* Arrays for managing parking lots
* Linked lists for vehicle entries and exits
* Dynamic memory allocation for variable-sized data
* Date and time validation
* Sorting and array compaction
* Separate functions for input processing, validation, parking management and billing

The project uses a `Park` structure to represent each parking lot and a `Car` structure to store vehicle information and its associated parking history.

## Building

The project includes a `Makefile` for compilation.

Build the project with:

```bash
make
```

This produces the `proj` executable.

To remove the compiled executable:

```bash
make clean
```

## Running

Run the program with:

```bash
./proj
```

The program reads commands from standard input.

## Example

For example, creating a parking lot and registering a vehicle entry and exit produces:

```text
p "Central Park" 100 0.20 0.30 12.00
e "Central Park" AA-00-AA 01-01-2024 10:00
Central Park 99
s "Central Park" AA-00-AA 01-01-2024 11:30
AA-00-AA 01-01-2024 10:00 01-01-2024 11:30 1.40
q
```


The system validates each operation and calculates the corresponding parking charge when a vehicle exits.

## Academic Context

This project was originally developed for the Introduction to Algorithms and Data Structures (IAED) course at Instituto Superior Técnico during the 2023/24 academic year.

The original assignment focused on implementing a complete parking management system in C, with particular emphasis on data structures, input validation, dynamic memory management and algorithmic problem solving.
