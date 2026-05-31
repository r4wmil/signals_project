addpath("out");

function ut_trel(constr_len, gen, feedback)
	trel_matlab = poly2trellis(constr_len, gen, feedback);
	trel_our = conv_trel(constr_len, gen, feedback);
end

ut_trel(3, [5 7], 7);
