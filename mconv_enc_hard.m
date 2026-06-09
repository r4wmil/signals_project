function enc = mconv_enc_hard(data, trel)
   
    k = round(log2(trel.numInpSym));
    n = round(log2(trel.numOutSym));

    if mod(length(data), k) ~= 0
        error('Data length must be a multiple of k');
    end

    state = 0;
    enc = zeros(1, (length(data)/k) * n);

    out_pos = 1;

    for p = 1:k:length(data)

        % k bits -> input symbol
        inputSym = 0;
        for i = 1:k
            inputSym = bitshift(inputSym, 1) + data(p+i-1);
        end

        % encoder output symbol
        outSym = trel.outputs(state+1, inputSym+1);

        % symbol -> n output bits (MSB first)
        for i = n:-1:1
            enc(out_pos+i-1) = bitand(outSym, 1);
            outSym = bitshift(outSym, -1);
        end

        out_pos = out_pos + n;

        % next state
        state = trel.nextStates(state+1, inputSym+1);
    end
end