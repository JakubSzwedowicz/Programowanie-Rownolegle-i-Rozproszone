# Programowanie-Rownolegle-i-Rozproszone


## Treść zadania
W ramach projektu należy:

1. Napisać program sekwencyjny w wybranym języku (nieobiektowo, tzn. korzystając z klas tylko tam, gdzie jest to konieczne), np.: C (domyślnie), Python, Julia, Rust, Go, JavaScript, Kotlin, Ruby, Dart, Swift, uruchomić go, zrobić testy.
2. Zrównoleglić swój program za pomocą wątków (domyślnie w C: OpenMP) lub tablic rozproszonych na dowolnej maszynie z procesorem wielordzeniowym, zrobić testy dla różnej liczby rdzeni, policzyć przyśpieszenia według definicji z wykładu S(n,p)=\frac{T(n,1)}{T(n,p)}.
3. Rozproszyć (ze zrównolegleniem) swój program za pomocą pakietu narzędzi opartych na przesyłaniu komunikatów (domyślnie w C: MPI) lub z rodziny RPC na tej samej maszynie  lub do obliczeń GPGPU. 
4. Oprócz wersji sekwencyjnej, zespół realizuje zrównoleglenie korzystając z tylu narzędzi ile jest osób w zespole.

Do zrobienia części sekwencyjnej i równoległej projektu wystarczy własny komputer z dowolnym kompilatorem języka C (zaleca się użycie kompilatora GNU GCC pod systemem Linux).

Uwaga! Po każdym etapie, tzn. po części:

* sekwencyjnej i równoległej przy użyciu wątków lub tablic rozproszonych, 
* rozproszonej przy użyciu narzędzi opartych na przesyłaniu komunikatów lub z rodziny RPC lub GPGPU,

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
./scripts/run_regressions_fast.sh
```
Wyniki czasowe na moim komputerze (AMD 9800x3d) po uruchomieniu skryptu `./scripts/run_commands_from_README_table.sh`:
* (-f) oznacza funkcję do obliczenia
* (-s) oznacza wymiar
* (-e) oznacza wykładnik epsilona
* (-l) oznacza plik logów

|                            Komenda                        |    Czas    |
|-----------------------------------------------------------|------------|
| ./cmake-build-debug/app -f 1 -s 1000 -e 6 -l logs/logs_20250105_214114/run0.log | 64.098 s |
| ./cmake-build-debug/app -f 1 -s 1000 -e 9 -l logs/logs_20250105_214114/run1.log | 82.180 s |
| ./cmake-build-debug/app -f 16 -s 200 -e 6 -l logs/logs_20250105_214114/run2.log | 54.974 s |
| ./cmake-build-debug/app -f 16 -s 250 -e 6 -l logs/logs_20250105_214114/run3.log | 132.745 s |
| ./cmake-build-debug/app -f 17 -s 500 -e 6 -l logs/logs_20250105_214114/run4.log | 64.635 s |
| ./cmake-build-debug/app -f 17 -s 500 -e 9 -l logs/logs_20250105_214114/run5.log | 104.715 s |


Można wygenerować powyższą tabelę z dowolnych plików logów (bez względu na nazwę, aczkolwiek jeśli mają być posortowane to musi w nazwie być człon `run...`) za pomocą skryptu `./scripts/generate_table.sh <logs_dir>`
, np. `./scripts/generate_table.sh logs/logs_20250105_214114`.