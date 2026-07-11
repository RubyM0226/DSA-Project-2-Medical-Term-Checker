# DSA-Project-2-Medical-Term-Checker


John: Make sure to clean the files into a .txt file with one term per line and no blank lines (lowercase is fine); then we can just use a single getline() loop with no parsing needed; name it something like terms.txt

Kaden: in the HashMap class, it needs a search(string) method that returns bool in the main.cpp file
Heh okay! all done i htinks

to 
- we should be using C++ 17 (it is the most compatible with everything)

(I made this so we can use this for part of the read me (looks nice so :3))
## How to build and run

Requires a C++17 compiler (g++). Run these from inside the project
folder (the one containing main.cpp and terms.txt).

**Main search tool:**
g++ -std=c++17 -O2 -static -o search.exe main.cpp
./search.exe


Then pick from the menu:
- 1 = exact search (hash map)
- 2 = autocomplete (trie)
- 4 = exit

**Benchmark (trie vs hash map):**
g++ -std=c++17 -O2 -static -o benchmark.exe benchmark.cpp


./benchmark.exe

Prints a timing table and writes benchmark_results.csv (open in Excel or just read the data in excel/compiler)

(Side note: My PC had to use -static apparently cuz of a mismatch so keep that in mind if you run into probs)