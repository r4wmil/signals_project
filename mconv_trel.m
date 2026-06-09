function mtrel = mconv_trel(c, G)
    k = size(G, 1); % input bits
    n = size(G, 2); % output bits

    % oct int -> int
    for i = 1:k
        for j = 1:n
            G(i,j) = sscanf(num2str(G(i,j)), '%o');
        end
    end

    mtrel = struct();
    mtrel.numInpSym = 2 ^ k;
    mtrel.numOutSym = 2 ^ n;
    mtrel.numStates = 2 ^ ((c - 1) * k);
    mtrel.nextStates = zeros(mtrel.numStates, 2 ^ k);
    mtrel.outputs = zeros(mtrel.numStates, 2 ^ k);
    
    for state = 0:mtrel.numStates-1
        for inputSym = 0:mtrel.numInpSym-1
            % get k input bits
            inputs = zeros(1, k);
            temp = inputSym;
            for i = k:-1:1
                inputs(i) = bitget(temp, 1);
                temp = bitshift(temp, -1);
            end
            
            % nextStates
            nextState = bitshift(state, -k);
            for i = 1:k
                nextState = nextState + bitshift(inputs(i), (c-2)*k + (k-i));
            end
            mtrel.nextStates(state+1, inputSym+1) = nextState;
            
            % [ new_inp_arr | old_states ]
            reg = state;
            for i = 1:k
                reg = reg + bitshift(inputs(i), (c-1)*k - (i-1)*k);
            end
            
            % outputs
            out = 0;
            for j = 1:n
                parity = 0;
                for i = 1:k
                    gen = G(i,j);
                    for b = 0:c-1
                        if bitand(gen, bitshift(1, b))
                            reg_bit = bitand(bitshift(reg, -(b + (k-i)*c)), 1);
                            parity = xor(parity, reg_bit);
                        end
                    end
                end
                out = bitshift(out, 1) + parity;
            end
            mtrel.outputs(state+1, inputSym+1) = out;
        end
    end
end