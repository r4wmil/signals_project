function enc = check57(data)
    trel = poly2trellis(3, [5 7]);
    enc = convenc(data, trel);
end

check57([1,1]);

function enc = check2d(data)
    trel = poly2trellis([3 3], [5 7; 1 4]);
    enc = convenc(data, trel);
end

check2d([1 1 1 1])

trel = poly2trellis(3, [5 7]);
disp(trel);
trel.nextStates
trel.outputs