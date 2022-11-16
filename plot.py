from matplotlib.pyplot import plot, show, axis
from csv import reader
from sys import argv


if __name__ == '__main__':
    with open(argv[1], 'r') as f:
        lines = reader(f, delimiter=';')
        next(lines)
        xs = []
        ys = []
        for line in lines:
            xs.append(float(line[0]))
            ys.append(float(line[1]))
    #for i in range(2, 100):
    #    print(xs[i], ys[i])
    plot(xs[2:], ys[2:])
    show()
