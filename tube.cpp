#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <cstring>
#include <cctype>
#include <cstdlib>

using namespace std;

const int MAX_TEXT_LENGTH = 50;
const int DIRECTION_LENGTH = 3;
const int MAX_FILENAME_LENGTH = 50;

#include "tube.h"

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

/* internal helper function which allocates a dynamic 2D array */
char **allocate_2D_array(int rows, int columns) {
  char **m = new char *[rows];
  assert(m);
  for (int r=0; r<rows; r++) {
    m[r] = new char[columns];
    assert(m[r]);
  }
  return m;
}

/* internal helper function which deallocates a dynamic 2D array */
void deallocate_2D_array(char **m, int rows) {
  for (int r=0; r<rows; r++)
    delete [] m[r];
  delete [] m;
}

/* internal helper function which gets the dimensions of a map */
bool get_map_dimensions(const char *filename, int &height, int &width) {
  char line[512];
  
  ifstream input(filename);

  height = width = 0;

  input.getline(line,512);  
  while (input) {
    if ( (int) strlen(line) > width)
      width = strlen(line);
    height++;
    input.getline(line,512);  
  }

  if (height > 0)
    return true;
  return false;
}

/* pre-supplied function to load a tube map from a file*/
char **load_map(const char *filename, int &height, int &width) {

  bool success = get_map_dimensions(filename, height, width);
  
  if (!success)
    return NULL;

  char **m = allocate_2D_array(height, width);
  
  ifstream input(filename);

  char line[512];
  char space[] = " ";

  for (int r = 0; r<height; r++) {
    input.getline(line, 512);
    strcpy(m[r], line);
    while ( (int) strlen(m[r]) < width )
      strcat(m[r], space);
  }
  
  return m;
}

/* pre-supplied function to print the tube map */
void print_map(char **m, int height, int width) {
  cout << setw(2) << " " << " ";
  for (int c=0; c<width; c++)
    if (c && (c % 10) == 0) 
      cout << c/10;
    else
      cout << " ";
  cout << endl;

  cout << setw(2) << " " << " ";
  for (int c=0; c<width; c++)
    cout << (c % 10);
  cout << endl;

  for (int r=0; r<height; r++) {
    cout << setw(2) << r << " ";    
    for (int c=0; c<width; c++) 
      cout << m[r][c];
    cout << endl;
  }
}

/* pre-supplied helper function to report the errors encountered in Question 3 */
const char *error_description(int code) {
  switch(code) {
  case ERROR_START_STATION_INVALID: 
    return "Start station invalid";
  case ERROR_ROUTE_ENDPOINT_IS_NOT_STATION:
    return "Route endpoint is not a station";
  case ERROR_LINE_HOPPING_BETWEEN_STATIONS:
    return "Line hopping between stations not possible";
  case ERROR_BACKTRACKING_BETWEEN_STATIONS:
    return "Backtracking along line between stations not possible";
  case ERROR_INVALID_DIRECTION:
    return "Invalid direction";
  case ERROR_OFF_TRACK:
    return "Route goes off track";
  case ERROR_OUT_OF_BOUNDS:
    return "Route goes off map";
  }
  return "Unknown error";
}

/* presupplied helper function for converting string to direction enum */
Direction string_to_direction(const char *token) {
  const char *strings[] = {"N", "S", "W", "E", "NE", "NW", "SE", "SW"};
  for (int n=0; n<8; n++) {
    if (!strcmp(token, strings[n])) 
      return (Direction) n;
  }
  return INVALID_DIRECTION;
}


bool get_symbol_position(char **map, const int height, const int width, const char target, int &r, int &c) {
    for (auto i = 0; i < height; ++i) {
        for (auto j = 0; j < width; ++j) {
            if (map[i][j] == target) {
                r = i;
                c = j;
                return true;
            }
        }
    }

    r = -1;
    c = -1;
    return false;
}


char get_symbol_for_station_or_line(char *name) {
    char symbol = get_symbol(true, name);  // line
    if (symbol != ' ') return symbol;

    return get_symbol(false, name);  // station
}


char get_symbol(bool flag, char *name) {
    char fileName[MAX_FILENAME_LENGTH];
    if (flag) strcpy(fileName, "lines.txt");
    else strcpy(fileName, "stations.txt");

    char symbol;
    char fullLine[MAX_TEXT_LENGTH], fullName[MAX_TEXT_LENGTH];

    ifstream in_stream;
    in_stream.open(fileName);

    while (in_stream.getline(fullLine, MAX_TEXT_LENGTH)) {
        symbol = fullLine[0];
        memcpy(fullName, fullLine + 2, MAX_TEXT_LENGTH);

        if (strcmp(name, fullName) == 0) {
            in_stream.close();
            return symbol;
        }
    }

    in_stream.close();
    return ' ';
}


int validate_route(char **map, int height, int width, char *start_station, char *route, char *destination) {
    char stationSymbol = get_symbol_for_station_or_line(start_station);
    if (stationSymbol == ' ') return ERROR_START_STATION_INVALID;

    int row = -1, col = -1;
    bool success = get_symbol_position(map, height, width, stationSymbol, row, col);
    if (!success) return ERROR_START_STATION_INVALID;

    string direction;
    char lastSymbol = stationSymbol;
    char currentLine;
    string lastDirection;
    int numLineChanged = 0;
    int steps = 0;
    string routeStr = route;

    for (auto i = 0; i < routeStr.length(); ++i) {
        if (route[i] != ',') {
            direction += route[i];

            if (i != routeStr.length() - 1) continue;
        }

        steps += 1;

        Direction directionDir = string_to_direction(const_cast<char*>(direction.c_str()));

        switch (directionDir) {
            case N:
                row -= 1;
                break;

            case S:
                row += 1;
                break;

            case W:
                col -= 1;
                break;

            case E:
                col += 1;
                break;

            case NE:
                row -= 1;
                col += 1;
                break;

            case NW:
                row -= 1;
                col -= 1;
                break;

            case SE:
                row += 1;
                col += 1;
                break;

            case SW:
                row += 1;
                col -= 1;
                break;

            case INVALID_DIRECTION:
                return ERROR_INVALID_DIRECTION;
        }

        if (steps > 1 && is_opposite(lastDirection, direction))
            if (is_line(lastSymbol)) return ERROR_BACKTRACKING_BETWEEN_STATIONS;
        lastDirection = direction;

        if (row >= height || row < 0 || col >= width || col < 0)
            return ERROR_OUT_OF_BOUNDS;

        if (map[row][col] == ' ') return ERROR_OFF_TRACK;

        if (is_line(map[row][col]) && is_line(lastSymbol) && map[row][col] != lastSymbol)
            return ERROR_LINE_HOPPING_BETWEEN_STATIONS;
        lastSymbol = map[row][col];

        if (steps == 2) currentLine = map[row][col];
        if (steps > 2 && is_line(map[row][col]) && currentLine != map[row][col]) numLineChanged += 1;
        currentLine = ((is_line(map[row][col]) ? map[row][col] : currentLine));

        direction.clear();
    }

    if (is_line(map[row][col])) return ERROR_ROUTE_ENDPOINT_IS_NOT_STATION;

    success = get_station_name(map[row][col], destination);

    return numLineChanged;
}


bool is_line(char symbol) {
    char symbolSet[] = {'#', '-', '*', '&', '$', '+', '|', '>', '<'};
    for (auto i = 0; i < strlen(symbolSet); ++i) {
        if (symbol == symbolSet[i]) return true;
    }

    return false;
}


bool get_station_name(char ch, char *destination) {
    char oneLine[MAX_TEXT_LENGTH];

    ifstream in_stream("stations.txt");
    if (in_stream.fail()) exit(0);

    while (in_stream.getline(oneLine, MAX_TEXT_LENGTH)) {
        if (oneLine[0] == ch) {
            memcpy(destination, oneLine + 2, MAX_TEXT_LENGTH);
            return true;
        }
    }

    return false;
}


bool is_opposite(const string &one, const string &two) {
    if ((one == "N" && two == "S") || (one == "S" && two == "N")) return true;
    if ((one == "W" && two == "E") || (one == "E" && two == "W")) return true;
    if ((one == "NE" && two == "SW") || (one == "SW" && two == "NE")) return true;
    if ((one == "NW" && two == "SE") || (one == "SE" && two == "NW")) return true;

    return false;
}