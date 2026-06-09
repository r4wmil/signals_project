function unit_tests
    ut_trel(3, [5 7]);
    ut_trel(4, [13 17]);
    ut_trel(3, [5 7 5]);
    ut_enc_hard(3, [5 7], [1 0 1 1 0]);
    ut_enc_hard(3, [5 7 5], [1 0 1 1 0]);
    ut_enc_hard(4, [13 17], randi([0 1], 1, 20));
    ut_enc_hard(3, [5 7], 0);
    ut_enc_hard(3, [5 7], 1);
end

function ut_enc_hard(constr_len, gen, data)
    fprintf('\n=== Test: constr_len=%d, gen=%s, data=%s ===\n', ...
        constr_len, mat2str(gen), mat2str(data));
    
    trel = mconv_trel(constr_len, gen);
    enc_our = mconv_enc_hard(data, trel);
    
    % Verify against MATLAB's convenc if available
    try
        trel_matlab = poly2trellis(constr_len, gen);
        enc_matlab = convenc(data, trel_matlab);
        
        if isequal(enc_our, enc_matlab)
            fprintf('✓ PASSED\n');
        else
            fprintf('✗ FAILED\n');
            fprintf('  Our output:    '); disp(enc_our);
            fprintf('  MATLAB output: '); disp(enc_matlab);
        end
    catch
        fprintf('! Could not verify with MATLAB convenc\n');
        fprintf('  Our output: '); disp(enc_our);
    end
end

function ut_trel(constr_len, gen)
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