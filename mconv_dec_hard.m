function dec = mconv_dec_hard(data, trel)
    
    k = round(log2(trel.numInpSym));
    n = round(log2(trel.numOutSym));
    numStates = trel.numStates;

    if mod(length(data), n) ~= 0
        error('Data length must be a multiple of n');
    end

    numSteps = length(data) / n;
    
    % Initialize path metrics: State 0 starts at 0, all others at infinity
    pathMetrics = Inf(numStates, 1);
    pathMetrics(1) = 0;
    
    % Store the history of inputs that led to each state at each step
    % Dimensions: [numStates, numSteps * k]
    tbHistory = zeros(numStates, numSteps * k);

    in_pos = 1;

    for step = 1:numSteps
        % 1. Extract the current received n-bit block
        recBits = data(in_pos : in_pos+n-1);
        in_pos = in_pos + n;

        % Pre-allocate metrics for the next step
        nextPathMetrics = Inf(numStates, 1);
        nextTbHistory = zeros(numStates, step * k);

        % 2. Loop through all possible current states
        for currState = 0:numStates-1
            if isinf(pathMetrics(currState+1))
                continue; % Skip unreachable states
            end

            % 3. Loop through all possible input symbols from this state
            for inputSym = 0:trel.numInpSym-1
                
                % Get the expected output symbol and next state
                expectedOutSym = trel.outputs(currState+1, inputSym+1);
                nextState = trel.nextStates(currState+1, inputSym+1);

                % Convert expected output symbol to bits (MSB first, matching encoder)
                expectedBits = zeros(1, n);
                tempSym = expectedOutSym;
                for i = n:-1:1
                    expectedBits(i) = bitand(tempSym, 1);
                    tempSym = bitshift(tempSym, -1);
                end

                % Calculate Hamming distance (branch metric)
                branchMetric = sum(recBits ~= expectedBits);
                candidateMetric = pathMetrics(currState+1) + branchMetric;

                % 4. Select the path with the minimum metric (Add-Compare-Select)
                if candidateMetric < nextPathMetrics(nextState+1)
                    nextPathMetrics(nextState+1) = candidateMetric;
                    
                    % Convert input symbol to its k-bit representation
                    inputBits = zeros(1, k);
                    tempIn = inputSym;
                    for i = k:-1:1
                        inputBits(i) = bitand(tempIn, 1);
                        tempIn = bitshift(tempIn, -1);
                    end
                    
                    % Update traceback history for the next state
                    if step == 1
                        nextTbHistory(nextState+1, :) = inputBits;
                    else
                        nextTbHistory(nextState+1, :) = [tbHistory(currState+1, 1:(step-1)*k), inputBits];
                    end
                end
            end
        end
        
        % Update structures for the next time step
        pathMetrics = nextPathMetrics;
        tbHistory = nextTbHistory;
    end

    % 5. Traceback: Find the state with the lowest metric at the final step
    [~, bestStateIdx] = min(pathMetrics);
    
    % The decoded data is the history sequence of that best final state
    dec = tbHistory(bestStateIdx, :);
end
