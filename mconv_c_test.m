%% 1. Configuration Parameters
c = 3;                       % Constraint length
G = [5 7];                   % Generator polynomials (Octal: 111, 101 -> 7, 5)
n = length(G);               % Number of output streams

%% 2. Generate Test Input Data
data = logical([1, 0, 1, 1, 0, 0]); 

%% 3. Run Operations
% Encode
encoded = conv_mex('enc', c, G, n, data);
fprintf('Encoded Bits:      %s\n', num2str(double(encoded)));

% Hard Decision Decode
decoded_hard = conv_mex('hard', c, G, n, encoded);
fprintf('Hard Decoded Data: %s\n', num2str(double(decoded_hard)));

% Soft Decision Decode (Map bits to BPSK: 0 -> +1.0, 1 -> -1.0 and add noise)
bpsk_symbols = single(1.0 - 2.0 * double(encoded)); 
noisy_symbols = bpsk_symbols + single(0.25 * randn(size(bpsk_symbols)));

decoded_soft = conv_mex('soft', c, G, n, noisy_symbols);
fprintf('Soft Decoded Data: %s\n', num2str(double(decoded_soft)));
