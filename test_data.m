mkdir('test_data');

function gen_trel(name, constr_len, gen)
    % TODO: generate file with data
    trel = poly2trellis(constr_len, gen);
    trel.numInputSymbols
    trel.numOutputSymbols
    trel.numStates
    trel.nextStates
    trel.outputs
end

gen_trel('5-7', 3, [5 7]);