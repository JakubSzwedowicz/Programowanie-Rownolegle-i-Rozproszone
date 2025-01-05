import os
import re
import argparse

parser = argparse.ArgumentParser(description="Generate a table from log files.")
parser.add_argument("log_dir", type=str, help="Directory containing the log files")
args = parser.parse_args()

log_dir = args.log_dir

command_pattern = re.compile(r"Command\s+=\s+(.*)")
time_pattern = re.compile(r"time\s+=\s+(\d+\.\d+\s+s)")

# Iterate over the log files
log_files = sorted([f for f in os.listdir(log_dir)], key=lambda log_filename: int(re.search(r'run(\d+)', log_filename).group(1)))

table =  "|                            Komenda                        |    Czas    |\n"
table += "|-----------------------------------------------------------|------------|\n"
for filename in log_files:
    filepath = os.path.join(log_dir, filename)
    with open(filepath, 'r') as file:
        content = file.read()
        command_match = command_pattern.search(content)
        time_match = time_pattern.search(content)
        if command_match and time_match:
            command = command_match.group(1)
            time = time_match.group(1)
            table += f"| {command} | {time} |\n"

print(table)