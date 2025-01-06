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
Po zbudowaniu programu można uruchomić szybkie testy regresji za pomocą skryptu:
```bash
./scripts/run_regressions_fast.sh
```
Wyniki czasowe na moim komputerze (AMD 9800x3d) po uruchomieniu skryptu `./scripts/run_parallel_sequential_runs_slow.sh`:
* (-f) oznacza funkcję do obliczenia
* (-s) oznacza wymiar
* (-e) oznacza wykładnik epsilona
* (-l) oznacza plik logów
* (-t) oznacza ilość wątków (domyślnie 1)

| Function | Komenda | Czas |
|----------|-----------------|------------|
| f(x) = sum3->N(100(x_i^2 + x_{i-1}^2) + x_{i-2}^2) | ./cmake-build-debug/app -f 1 -s 1000 -e 6 -l logs/parallel_sequential_runs_readme_table/logs_20250106_210612/run0.log | 66.733026 s |
| f(x) = sum3->N(100(x_i^2 + x_{i-1}^2) + x_{i-2}^2) | ./cmake-build-debug/app -f 1 -s 1000 -e 9 -l logs/parallel_sequential_runs_readme_table/logs_20250106_210612/run1.log | 84.47374 s |
| f(x) = sum1->N(n - sum1->N(cos(x_j)) + (i)(1 - cos(x_i)) - sin(x_i))^2) | ./cmake-build-debug/app -f 16 -s 200 -e 6 -l logs/parallel_sequential_runs_readme_table/logs_20250106_210612/run2.log | 54.962484 s |
| f(x) = sum1->N(n - sum1->N(cos(x_j)) + (i)(1 - cos(x_i)) - sin(x_i))^2) | ./cmake-build-debug/app -f 16 -s 250 -e 6 -l logs/parallel_sequential_runs_readme_table/logs_20250106_210612/run3.log | 134.388142 s |
| f(x) = (sum1->N(i * x_i^2))^2 | ./cmake-build-debug/app -f 17 -s 500 -e 6 -l logs/parallel_sequential_runs_readme_table/logs_20250106_210612/run4.log | 69.433453 s |
| f(x) = (sum1->N(i * x_i^2))^2 | ./cmake-build-debug/app -f 17 -s 500 -e 9 -l logs/parallel_sequential_runs_readme_table/logs_20250106_210612/run5.log | 109.897441 s |

Można wygenerować powyższą tabelę z dowolnych plików logów (bez względu na nazwę, 
aczkolwiek jeśli mają być posortowane to musi w nazwie być człon `run...`) 
za pomocą skryptu `scripts/process_logs.py` (należy wgrać `requirements.txt`):
```bash
(.venv) jakubszwedowicz@Desktop-Kuba:~/Dev/Programowanie-Rownolegle-i-Rozproszone$ python3 scripts/process_logs.py 
usage: process_logs.py [-h] --log_dir LOG_DIR [--output_table OUTPUT_TABLE] [--output_graph OUTPUT_GRAPH] [--graph] [--table]
                       [--plot_by {threads,size,epsilon,function}]
process_logs.py: error: the following arguments are required: --log_dir

(.venv) jakubszwedowicz@Desktop-Kuba:~/Dev/Programowanie-Rownolegle-i-Rozproszone$ python3 scripts/process_logs.py --log_dir logs/logs_20250106_002328 --graph --table --plot_by function
Markdown table written to logs/logs_20250106_002328/table.md
Graph saved to logs/logs_20250106_002328/elapsed_time_graph.png
```

### Testowanie
Zachęcam do uruchamiania skryptów z folderu `scripts` w celu przetestowania programu.
```bash
(.venv) jakubszwedowicz@Desktop-Kuba:~/Dev/Programowanie-Rownolegle-i-Rozproszone$ ls scripts/
process_logs.py                       run_parallel_runs_medium-f16-s200.sh  run_parallel_runs_very_slow-f16-s500.sh  run_regressions_fast.sh
run_parallel_runs_fast-f1-s200.sh     run_parallel_runs_slow-f1-s1500.sh    run_parallel_sequential_runs_fast.sh     utils.sh
run_parallel_runs_medium-f1-s1000.sh  run_parallel_runs_slow-f16-s250.sh    run_parallel_sequential_runs_readme_table_slow.sh
(.venv) jakubszwedowicz@Desktop-Kuba:~/Dev/Programowanie-Rownolegle-i-Rozproszone$ ./scripts/run_parallel_sequential_runs_fast.sh 
...
```
Dostępne również są krótkie testy jednostkowe (bez gtesta):
```bash
(.venv) jakubszwedowicz@Desktop-Kuba:~/Dev/Programowanie-Rownolegle-i-Rozproszone/build$ ./Task13/Tests/tests 
```

## Zależności
* OpenMP
* OpenMPI (`sudo apt install libopenmpi-dev`, może dodatkowo `sudo apt install openmpi-bin openmpi-common`)