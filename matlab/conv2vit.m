addpath('conv57');
run('conv57/compile.m');

% Input values
N = 30000;
EbNo = 0:1:10; % Energy/bit to noise power
trials = 10;
trel = poly2trellis(3, [5 7]);
traceback = 5 * 3;

% Simulating
data = randi([0, 1], N, 1);
err_unc = zeros(size(EbNo));
err_cod = zeros(size(EbNo));
err_cod_our = zeros(size(EbNo));
for i = 1:length(EbNo)
    for t = 1:trials
        eu = sim_noise(data, EbNo(i));
        ec = sim_conv2vit(data, trel, traceback, EbNo(i));
        eco = sim_conv57(data, trel, EbNo(i));
        err_unc(i) = err_unc(i) + eu;
        err_cod(i) = err_cod(i) + ec;
        err_cod_our(i) = err_cod_our(i) + eco;
    end
    err_unc(i) = err_unc(i) / trials;
    err_cod(i) = err_cod(i) / trials;
    err_cod_our(i) = err_cod_our(i) / trials;
end

% Plotting
figure;
hold on;
semilogy(EbNo, err_unc, 'b-o', 'LineWidth', 2);
semilogy(EbNo, err_cod, 'r-s', 'LineWidth', 2);
semilogy(EbNo, err_cod_our, 'g-s', 'LineWidth', 2);
set(gca, 'YScale', 'log');
hold off;
grid on;
xlabel('Eb/No');
ylabel('Bit Error Rate');
legend('Uncoded', 'Convolutional (vitdec)' , 'Convolutional (our)');

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

function err_unc = sim_noise(data, EbNo)
    % Uncoded
    modulated = 2 * data - 1;
    data_noise = awgn(modulated, get_snr(0, EbNo));
    data_noise = round(data_noise);
    data_noise = max(-1, min(1, data_noise));
    data_noise = data_noise >= 0;
    err_unc = mean(data ~= data_noise);
end

function err_cod = sim_conv2vit(data, trel, tb, EbNo)
    % Coded
    data_conv = convenc(data, trel);
    modulated = 2 * data_conv - 1;
    data_conv_noise = awgn(modulated, get_snr(trel, EbNo));
    data_conv_noise = round(data_conv_noise);
    data_conv_noise = max(-1, min(1, data_conv_noise));
    data_conv_noise = data_conv_noise >= 0;
    data_conv_dec = vitdec(data_conv_noise, trel, tb, 'trunc', 'hard');
    err_cod = mean(data ~= data_conv_dec);
end

function err_cod = sim_conv57(data, trel, EbNo)
    % Coded (our)
    data_padded = [zeros(3,1); data; zeros(3,1)];
    data_conv = convenc(data_padded, trel);
    modulated = 2 * data_conv - 1;
    data_conv_noise = awgn(modulated, get_snr(trel, EbNo));
    %data_conv_noise = modulated;
    data_conv_noise = round(data_conv_noise);
    data_conv_noise = max(-1, min(1, data_conv_noise));
    data_conv_noise = data_conv_noise >= 0;
    data_conv_dec = conv57dechard(data_conv_noise);
    err_cod = mean(data_padded' ~= data_conv_dec(3:end));
end