# Programowanie-Rownolegle-i-Rozproszone


## Treść zadania
W ramach projektu należy:

1. Napisać program sekwencyjny w wybranym języku (nieobiektowo, tzn. korzystając z klas tylko tam, gdzie jest to konieczne), np.: C (domyślnie), Python, Julia, Rust, Go, JavaScript, Kotlin, Ruby, Dart, Swift, uruchomić go, zrobić testy.
2. Zrównoleglić swój program za pomocą wątków (domyślnie w C: OpenMP) lub tablic rozproszonych na dowolnej maszynie z procesorem wielordzeniowym, zrobić testy dla różnej liczby rdzeni, policzyć przyśpieszenia według definicji z wykładu S(n,p)=\frac{T(n,1)}{T(n,p)}.
3. Rozproszyć (ze zrównolegleniem) swój program za pomocą pakietu narzędzi opartych na przesyłaniu komunikatów (domyślnie w C: MPI) lub z rodziny RPC na tej samej maszynie  lub do obliczeń GPGPU. 
4. Oprócz wersji sekwencyjnej, zespół realizuje zrównoleglenie korzystając z tylu narzędzi ile jest osób w zespole.

Do zrobienia części sekwencyjnej i równoległej projektu wystarczy własny komputer z dowolnym kompilatorem języka C (zaleca się użycie kompilatora GNU GCC pod systemem Linux).

Uwaga! Po każdym etapie, tzn. po części:
a) sekwencyjnej i równoległej przy użyciu wątków lub tablic rozproszonych,
b) rozproszonej przy użyciu narzędzi opartych na przesyłaniu komunikatów lub z rodziny RPC lub GPGPU,
należy załadować w odpowiedniej aktywności zzipowany plik z kodem źródłowym programu  oraz sprawozdaniem w terminach podanych w Harmonogramie.

## Uruchomienie programu
Program podąża za typowym:
```bash
mkdir build
cd build
cmake ..
make
./app <args>
```
Po zbudowaniu programu można uruchomić go skryptem:
```bash
./scripts/run_tests_fast.sh
```
Wyniki czasowe na moim komputerze (AMD 9800x3d) po uruchomieniu skryptu `./scripts/run_commands_from_README_table.sh`:

| Komenda                  | Czas     |
|--------------------------|----------|
| ./app -f 1 -s 1500 -e 6  | 101.685s |
| ./app -f 1 -s 1000 -e 9  | 40.675s  |
| ./app -f 16 -s 500 -e 9  | 57.741s  |
| ./app -f 16 -s 750 -e 6  | 142.663s |
| ./app -f 17 -s 1000 -e 9 | 40.893s  |
| ./app -f 17 -s 1500 -e 6 | 102.108s |
