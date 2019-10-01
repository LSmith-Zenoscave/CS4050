#!/usr/bin/env python
import numpy as np
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
from matplotlib import gridspec
import sys 
import math
from itertools import product
from functools import reduce
from operator import and_

filename = sys.argv[1] if len(sys.argv) > 1 else input("filename: ")
outname = filename.rsplit(".", 1)[0]
data = pd.read_csv(filename)
keys = ["pivot", "partition", "callback"]

def do_plot(ax, label, dataset):
    xs, ys = zip(*dataset.values)
    scaled_ys = [y*1e6 for y in ys]
    ax.scatter(xs, scaled_ys)
    ax.set_ylim((0, 120000))
    ax.set_xlabel(label)

def isvalue(kv):
    return data[kv[0]] == kv[1]


filters = list(product(*(set(data[key]) for key in keys)))
graph_sets = {
    "-".join(filter_set): data[reduce(and_, map(isvalue, zip(keys, filter_set)))].filter(
        items=("size", "time")
    )
    for filter_set in filters
}

N = len(graph_sets)
cols = 4
rows = int(math.ceil(N / cols))

gs = gridspec.GridSpec(rows, cols)
fig = plt.figure()

for n, (label, dataset) in enumerate(graph_sets.items()):
    ax = fig.add_subplot(gs[n])
    do_plot(ax, label, dataset)

plt.title("Quicksorts times (micorseconds) from 10,000 to 1,000,000")
plt.show()
plt.close()

vals = [("\n".join(l.split()[0] for l in label.split("-")), 1e6*dataset[dataset["size"] == max(dataset["size"])].time.values[0]) for label, dataset in graph_sets.items()]
vals.sort(key=lambda v: v[1])
labels, y_vals = zip(*vals)
y_pos = list(np.arange(len(y_vals)))

plt.bar(y_pos, y_vals, align="center")
plt.xticks(y_pos, labels, rotation=-45, ha="left")

plt.ylabel("Time in microseconds")
plt.title("Quicksort Timing Comparisons")

plt.show()
