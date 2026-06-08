function test_mconv_trel
    run_test(3, [5 7]);
    run_test(4, [13 17]);
    run_test(3, [5 7 5]);
end

function run_test(constr_len, gen)
    fprintf('\n=== Test: constr_len=%d, gen=%s ===\n', constr_len, mat2str(gen));
    trel_matlab = poly2trellis(constr_len, gen);
    trel_our = mconv_trel(constr_len, gen);
    compare_trellis(trel_matlab, trel_our);
end

function compare_trellis(trel_matlab, trel_our)
    fields = {'numInputSymbols', 'numOutputSymbols', 'numStates', 'nextStates', 'outputs'};
    our_fields = {'numInpSym', 'numOutSym', 'numStates', 'nextStates', 'outputs'};
    passed = true;
    
    for i = 1:length(fields)
        if ~isequal(trel_matlab.(fields{i}), trel_our.(our_fields{i}))
            fprintf('✗ %s mismatch\n', fields{i});
            fprintf('  MATLAB: '); disp(trel_matlab.(fields{i}));
            fprintf('  Our:    '); disp(trel_our.(our_fields{i}));
            passed = false;
        end
    end
    
    if passed
        fprintf('✓ PASSED\n');
    end
end