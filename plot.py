import matplotlib.pyplot as plt
import pathlib


RES_DIR = './res'


def read(filename):
    filename = f'meas/{filename}'
    nums = []
    with open(filename, 'r') as file:
        for line in file:
            nums.append(float(line))
    return nums


def plot(filename):
    nums = read(filename)
    nums = list(map(lambda n: nums[0]/n, nums))
    # xs = [2**i for i in range(0, len(nums))]
    xs = list(range(1, len(nums)+1))

    ax = plt.subplot()
    ax.plot(xs, nums, '-o', label=filename)
    ax.xaxis.set_major_locator(plt.MultipleLocator(1))
    plt.xlabel('Number of threads')
    plt.ylabel('Acceleration')
    plt.title(filename)
    plt.savefig(f'{RES_DIR}/{filename}.png')
    plt.clf()


if __name__ == '__main__':
    pathlib.Path(RES_DIR).mkdir(parents=True, exist_ok=True)
    for filename in ['openmp', 'mpi']:
        plot(filename)
