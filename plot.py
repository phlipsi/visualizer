from matplotlib.pyplot import plot, show, axis
from csv import reader
from sys import argv
from argparse import ArgumentParser


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument('filename')
    parser.add_argument('--measures')
    parser.add_argument('--parameter', default='measure', action='store')
    args = parser.parse_args()

    start = None
    end = None
    if args.measures:
        start, end = args.measures.split('-')
        if start:
            start = float(start)
        if end:
            end = float(end)

    with open(args.filename, 'r') as f:
        lines = reader(f, delimiter=';')
        columns = next(lines)
        column = columns.index(args.parameter)
        if not column:
            exit(1)
        xs = []
        ys = []
        for line in lines:
            measure = float(line[0])
            if (not start or measure >= start) and (not end or measure < end):
                xs.append(measure)
                ys.append(float(line[column]))

    plot(xs[2:], ys[2:])
    show()
