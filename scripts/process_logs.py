import os
import re
import argparse
from typing import Optional, List, Dict

import matplotlib.pyplot as plt


class DataPoint:
    def __init__(self, function: str, command: str, time: float, run_number: int):
        self.function = function
        self.command = command
        self.time = time
        self.run_number = run_number
        self._epsilon = None
        self._size = None
        self._threads = None

    def __repr__(self):
        return f"DataPoint(function={self.function}, command={self.command}, time={self.time}, run_number={self.run_number})"

    @property
    def threads(self) -> int:
        if self._threads is None:
            self._threads = extract_thread_count(self.command)
        return self._threads

    @property
    def size(self) -> int:
        if self._size is None:
            self._size = extract_size(self.command)
        return self._size

    @property
    def epsilon(self) -> float:
        if self._epsilon is None:
            self._epsilon = extract_epsilon(self.command)
        return self._epsilon


def extract_thread_count(command) -> Optional[int]:
    """
    Extracts the number of threads from the command string.
    Assumes that the number of threads is specified with the '-t' flag.
    Example: '-t 14'
    """
    thread_pattern = re.compile(r"-t\s+(\d+)")
    match = thread_pattern.search(command)
    if match:
        return int(match.group(1))
    else:
        return None


def extract_size(command) -> Optional[int]:
    """
    Extracts the size from the command string.
    Assumes that the size is specified with the '-s' flag.
    Example: '-s 1000'
    """
    size_pattern = re.compile(r"-s\s+(\d+)")
    match = size_pattern.search(command)
    if match:
        return int(match.group(1))
    else:
        return None


def extract_epsilon(command) -> Optional[float]:
    """
    Extracts the epsilon from the command string.
    Assumes that the epsilon is specified with the '-e' flag.
    Example: '-e 1e-6'
    """
    epsilon_pattern = re.compile(r"-e\s+([0-9]+\.?[0-9]*e?-?[0-9]*)")
    match = epsilon_pattern.search(command)
    if match:
        return float(match.group(1))
    else:
        return None


def parse_arguments():
    parser = argparse.ArgumentParser(description="Generate a table and/or graph from log files.")
    parser.add_argument("--log_dir", type=str, help="Directory containing the log files", required=True)
    parser.add_argument("--output_table", type=str, default="table.md", help="Output Markdown table file")
    parser.add_argument("--output_graph", type=str, default="elapsed_time_graph.png", help="Output graph image file")
    parser.add_argument("--graph", action="store_true", help="Generate a graph of elapsed time"
                        )
    parser.add_argument("--table", action="store_true", help="Generate a Markdown table"
                        )
    parser.add_argument("--plot_by", type=str, choices=["threads", "size", "epsilon", "function"], default="threads",
                        help="Attribute to plot the graph by (threads, size, epsilon, function)",
                        )
    return parser.parse_args()


def plot_graph(data: List[DataPoint], output_graph: str, plot_by: str) -> None:
    if not data:
        print("No data available for plotting.")
        return

    grouped_data: Dict[str, List[DataPoint]] = {}
    for dp in data:
        grouped_data.setdefault(dp.function, []).append(dp)

    plt.figure(figsize=(10, 6))
    if plot_by == "function":
        for dp in data:
            grouped_data.setdefault(dp.function, []).append(dp)

        for function, dps in grouped_data.items():
            # Sort by run_number
            dps_sorted = sorted(dps, key=lambda x: x.run_number)
            x = [dp.run_number for dp in dps_sorted]
            y = [dp.time for dp in dps_sorted]
            plt.plot(x, y, marker='o', linestyle='-', label=function)

        plt.title('Execution Time vs. Run Number by Function')
        plt.xlabel('Run Number')
    else:
        for function, dps in grouped_data.items():
            # Extract x and y based on plot_by attribute
            if plot_by == "threads":
                dps_sorted = sorted(dps, key=lambda x: x.threads if x.threads is not None else 0)
                x = [dp.threads for dp in dps_sorted if dp.threads is not None]
            elif plot_by == "size":
                dps_sorted = sorted(dps, key=lambda x: x.size if x.size is not None else 0)
                x = [dp.size for dp in dps_sorted if dp.size is not None]
            elif plot_by == "epsilon":
                dps_sorted = sorted(dps, key=lambda x: x.epsilon if x.epsilon is not None else 0.0)
                x = [dp.epsilon for dp in dps_sorted if dp.epsilon is not None]
            else:
                print(f"Unknown plot_by attribute: {plot_by}")
                return

            y = [dp.time for dp in dps_sorted if getattr(dp, plot_by) is not None]

            if not x or not y:
                print(f"No valid data for function '{function}' based on '{plot_by}'. Skipping.")
                continue

            plt.plot(x, y, marker='o', linestyle='-', label=function)

        plt.title(f'Execution Time vs. {plot_by.capitalize()} by Function')
        if plot_by == "threads":
            plt.xlabel('Number of Threads')
        elif plot_by == "size":
            plt.xlabel('Size')
        elif plot_by == "epsilon":
            plt.xlabel('Epsilon')

    plt.ylabel('Execution Time (s)')
    plt.grid(True)
    plt.legend(title="Function")
    plt.tight_layout()
    plt.savefig(output_graph)
    plt.show()
    print(f"Graph saved to {output_graph}")


def plot_table(data: list[DataPoint], output_table: str) -> None:
    if not data:
        print("No data available to generate table.")
        return

    table = "| Function | Komenda | Czas |\n"
    table += "|----------|-----------------|------------|\n"

    for dp in data:
        table += f"| {dp.function} | {dp.command} | {dp.time} s |\n"

    with open(output_table, 'w') as table_file:
        table_file.write(table)
        print(f"Markdown table written to {output_table}")


def main():
    args = parse_arguments()
    log_dir = args.log_dir

    if not os.path.isdir(log_dir):
        print(f"Error: '{log_dir}' is not a valid directory.")
        return

    function_pattern = re.compile(r"Function\s*=\s*(.*)")
    command_pattern = re.compile(r"Command\s*=\s*(.*)")
    time_pattern = re.compile(r"time\s*=\s*([0-9]+\.[0-9]+\s*s)")

    data: list[DataPoint] = []

    log_files = sorted([f for f in os.listdir(log_dir) if re.search(r'run.*\.log$', f)],
                       key=lambda log_filename: int(re.search(r'run(\d+)', log_filename).group(1)))

    for filename in log_files:
        filepath = os.path.join(log_dir, filename)
        with open(filepath, 'r') as file:

            content = file.read()
            function_match = function_pattern.search(content)
            command_match = command_pattern.search(content)
            time_match = time_pattern.search(content)
            run_number_match = re.search(r'run(\d+)', filename)

            if function_match and command_match and time_match and run_number_match:
                function = function_match.group(1).strip()
                command = command_match.group(1).strip()
                time_str = time_match.group(1).strip()

                # Convert time to float (seconds)
                try:
                    time_seconds = float(time_str.split()[0])
                except ValueError:
                    print(f"Warning: Could not parse time in file {filename}")
                    continue

                if run_number_match:
                    run_number = int(run_number_match.group(1))
                else:
                    print(f"Warning: Could not extract run number from filename {filename}")
                    continue

                data.append(DataPoint(function, command, time_seconds, run_number))

    if args.table:
        plot_table(data, args.log_dir + '/' + args.output_table)

    if args.graph:
        plot_graph(data, args.log_dir + '/' + args.output_graph, args.plot_by)


if __name__ == "__main__":
    main()
