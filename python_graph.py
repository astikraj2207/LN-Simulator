f = open("stat.txt")
data = f.readlines()

data = [list(map(int, x.split())) for x in data]


from graphviz import Digraph

dot = Digraph(comment="output graph")

for dat in data:
    dot.edge(f"{dat[0]}", f"{dat[1]}")

dot.render("check.gv")