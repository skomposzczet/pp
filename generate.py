import random
import pathlib
import sys

DATA_DIR = './data'
MIN = -2_000_000_000
MAX = 2_000_000_000


def generate_file(filename, record_count):
    with open(filename, "w") as f:
        for _ in range(record_count):
            num = random.randint(MIN, MAX)
            f.write(f'{num}\n')


def generate(no_files, per_file):
    pathlib.Path(DATA_DIR).mkdir(parents=True, exist_ok=True)
    for i in range(no_files):
        generate_file(f'{DATA_DIR}/datafile{i}', per_file)


if __name__ == '__main__':
    no_files = 1
    per_file = 10_000_000
    if len(sys.argv) > 1:
        per_file = int(sys.argv[1])
    if len(sys.argv) > 2:
        no_files = int(sys.argv[2])

    generate(no_files, per_file)
