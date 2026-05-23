addpath('conv57');

% Input values
N = 10000;
EbNo = 0:1:8; % Energy/bit to noise power
trials = 30;
trel = poly2trellis(3, [5 7]);
traceback = 5 * 3;

% Simulating
err_unc = zeros(size(EbNo));
err_cod = zeros(size(EbNo));
err_cod_our = zeros(size(EbNo));
err_cod_soft = zeros(size(EbNo));
err_cod_our_soft = zeros(size(EbNo));
for i = 1:length(EbNo)
	data = randi([0, 1], N, 1);
	for t = 1:trials
		[eu, ec, eco, ecs, ecos] = sim_conv57(data, trel, traceback, EbNo(i));
		err_unc(i) = err_unc(i) + eu;
		err_cod(i) = err_cod(i) + ec;
		err_cod_our(i) = err_cod_our(i) + eco;
		err_cod_soft(i) = err_cod_soft(i) + ecs;
		err_cod_our_soft(i) = err_cod_our_soft(i) + ecos;
	end
	err_unc(i) = err_unc(i) / trials;
	err_cod(i) = err_cod(i) / trials;
	err_cod_our(i) = err_cod_our(i) / trials;
	err_cod_soft(i) = err_cod_soft(i) / trials;
	err_cod_our_soft(i) = err_cod_our_soft(i) / trials;
end

% Plotting
figure;
hold on;
semilogy(EbNo, err_unc, 'b-o', 'LineWidth', 2);
semilogy(EbNo, err_cod, 'r-s', 'LineWidth', 2);
semilogy(EbNo, err_cod_our, 'g-s', 'LineWidth', 2);
semilogy(EbNo, err_cod_soft, 'r-s', 'LineWidth', 1);
semilogy(EbNo, err_cod_our_soft, 'b-s', 'LineWidth', 1);
set(gca, 'YScale', 'log');
hold off;
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

function [err_unc, err_cod, err_cod_our, err_cod_soft, ecos] = sim_conv57(data, trel, tb, EbNo)
	% Generating random data
	% 1. Uncoded
	modulated = 2 * data - 1;
	data_noise = awgn(modulated, get_snr(0, EbNo));
	data_noise = data_noise >= 0;
	% 2. Coded (3, 5-7)
	data_conv = convenc(data, trel);
	modulated_conv = 2 * data_conv - 1;
	soft_values = awgn(modulated_conv, get_snr(trel, EbNo));
	data_conv_noise = soft_values >= 0;
	soft_quant = round((soft_values + 1) / 2 * 7);
	soft_quant = max(0, min(7, soft_quant));
	% Uncoded
	err_unc = mean(data ~= data_noise);
	% Coded
	data_conv_dec = vitdec(data_conv_noise, trel, tb, 'trunc', 'hard');
	err_cod = mean(data ~= data_conv_dec);
	% Coded (our)
	data_conv_dec = conv57dechard(data_conv_noise);
	err_cod_our = mean(data' ~= data_conv_dec(3:end));
	% Coded (soft)
	data_conv_dec = vitdec(soft_quant, trel, tb, 'trunc', 'soft', 3);
	err_cod_soft = mean(data ~= data_conv_dec);
	assignin('base', 'data_check', data);
	assignin('base', 'data_conv_dec', data_conv_dec);
	% Coded (our soft)
	data_conv_dec = conv57decsoft(soft_quant);
	ecos = mean(data' ~= data_conv_dec(3:end));
end

% diff check (TODO: separate file)
[eu, ec, eco, ecs] = sim_conv57(data, trel, traceback, 5);
diff_pos = (data_check ~= data_conv_dec);
[row, col] = find(diff_pos);
row
