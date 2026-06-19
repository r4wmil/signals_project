addpath('out');

% Input values
N = 200;
EbNo = 0:0.5:5; % Energy/bit to noise power
trialsPerEbNo = 30;
trel = poly2trellis(7, [171 133]);
trelo = mconv_trel(7, [171 133]);
traceback = 5 * 3;

% Simulating
eu_a = zeros(size(EbNo)); % Errors uncoded
ec_a = zeros(size(EbNo)); % Errors coded
eco_a = zeros(size(EbNo)); % Errors coded, our
for i = 1:length(EbNo)
	data = randi([0, 1], N, 1);
	for t = 1:trialsPerEbNo
		[eu, ec, eco] = sim_conv57(data, trel, trelo, traceback, EbNo(i));
		eu_a(i) = eu_a(i) + eu;
		ec_a(i) = ec_a(i) + ec;
        eco_a(i) = eco_a(i) + eco;
	end
	eu_a(i) = eu_a(i) / trialsPerEbNo;
	ec_a(i) = ec_a(i) / trialsPerEbNo;
    eco_a(i) = eco_a(i) / trialsPerEbNo;
end

% Plotting
figure;

hold on;
semilogy(EbNo, eu_a, 'b-o', 'LineWidth', 2);
semilogy(EbNo, ec_a, 'r-s', 'LineWidth', 2);
semilogy(EbNo, eco_a, 'r-s', 'LineWidth', 1);
set(gca, 'YScale', 'log');
grid on;

xlabel('Eb/No');
ylabel('Bit Error Rate');
legend('Uncoded', 'Hard (vitdec)' , 'Hard (our)', 'Soft (vitdec)', 'Soft (our)');

% Signal/Noise
function snr = get_snr(trel, EbNo)
	if isobject(trel)
		k = log2(trel.numInputSymbols);
		n = log2(trel.numOutputSymbols);
		conv_rate = k / n;
		K = 1; % BPSK
		snr = EbNo + 10 * log10(conv_rate * K);
	else
		snr = EbNo;
	end
end

function [eu, ec, eco] = sim_conv57(data, trel, trelo, tb, EbNo)
	% Generating random data
	% 1. Uncoded
	mod = 2 * data - 1;
	mod_unc_noise = awgn(mod, get_snr(0, EbNo));
	data_unc_noise = mod_unc_noise >= 0;
	% 2. Coded (3, 5-7)
	data_conv = convenc(data, trel);
	mod_conv = 2 * data_conv - 1;
	mod_conv_noise = awgn(mod_conv, get_snr(trel, EbNo));
	hard_conv_noise = mod_conv_noise >= 0;
	%soft_quant = round((mod_conv_noise + 1) / 2 * 7);
	%soft_quant = max(0, min(7, soft_quant));
	% Uncoded
	eu = mean(data ~= data_unc_noise);
	% Coded (hard, matlab)
	data_conv_dec = vitdec(hard_conv_noise, trel, tb, 'trunc', 'hard');
	ec = mean(data ~= data_conv_dec);
    % Coded (hard, our)
	data_conv_dec = mconv_dec_hard(hard_conv_noise, trelo);
	eco = mean(data ~= data_conv_dec');
end
