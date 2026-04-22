mex conv57enc.c
mex conv57decnoerr.c
mex conv57dechard.c
inp = logical([0, 0, 0, 1, 1, 0, 1, 0, 0, 0])
enc = conv57enc(inp);
trel = poly2trellis(3, [5 7]);
d1 = vitdec((enc + 1) / 2, trel, 3, 'trunc', 'hard')'
d2 = conv57dechard(enc)