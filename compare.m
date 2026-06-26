addpath('out');

% Input values
N = 3000;
EbNo = 0:0.5:5; % Energy/bit to noise power
trialsPerEbNo = 50;
trel = poly2trellis(7, [171 133]);
c = 7;
G = [121 91];
traceback = 15;

% Simulating
eu_a  = zeros(size(EbNo)); % Errors uncoded
ech_a = zeros(size(EbNo)); % Errors coded hard (vitdec)
eco_a = zeros(size(EbNo)); % Errors coded hard (mex)
ecs_a = zeros(size(EbNo)); % Errors coded soft (vitdec)
ecos_a = zeros(size(EbNo)); % Errors coded soft (mex)

for i = 1:length(EbNo)
	data = randi([0, 1], N, 1);
	for t = 1:trialsPerEbNo
		[eu, ech, eco, ecs, ecos] = sim_conv57(data, trel, traceback, EbNo(i));
		eu_a(i)   = eu_a(i) + eu;
		ech_a(i)  = ech_a(i) + ech;
		eco_a(i)  = eco_a(i) + eco;
		ecs_a(i)  = ecs_a(i) + ecs;
		ecos_a(i) = ecos_a(i) + ecos;
	end
	eu_a(i)   = eu_a(i) / trialsPerEbNo;
	ech_a(i)  = ech_a(i) / trialsPerEbNo;
	eco_a(i)  = eco_a(i) / trialsPerEbNo;
	ecs_a(i)  = ecs_a(i) / trialsPerEbNo;
	ecos_a(i) = ecos_a(i) / trialsPerEbNo;
end

% Plotting
figure;

hold on;
semilogy(EbNo, eu_a, 'b-o', 'LineWidth', 2);
semilogy(EbNo, ech_a, 'r-s', 'LineWidth', 2);
semilogy(EbNo, eco_a, 'r--x', 'LineWidth', 1);
semilogy(EbNo, ecs_a, 'g-d', 'LineWidth', 2);
semilogy(EbNo, ecos_a, 'g--+', 'LineWidth', 1);
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

function [eu, ech, eco, ecs, ecos] = sim_conv57(data, trel, tb, EbNo)
    global c G
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
	
	% Uncoded
	eu = mean(data ~= data_unc_noise);
	
	% Coded (hard, matlab)
	data_conv_dec = vitdec(hard_conv_noise, trel, tb, 'trunc', 'hard');
	ech = mean(data ~= data_conv_dec);
	
	% Coded (hard, our MEX)
	dec_mex_hard = conv_mex('hard', c, G, 2, logical(hard_conv_noise));
	eco = mean(data ~= dec_mex_hard.');
	
	% Coded (soft, matlab)
	data_conv_soft = vitdec(-mod_conv_noise, trel, tb, 'trunc', 'unquant');
	ecs = mean(data ~= data_conv_soft);
	
	% Coded (soft, our MEX)
	dec_mex_soft = conv_mex('soft', c, G, 2, single(-mod_conv_noise));
	ecos = mean(data ~= dec_mex_soft.');
end
