
N = 1000;
noise_mse = 0.3;
trel = poly2trellis(3, [5 7]);
traceback = 5 * 3;

data = randi([0, 1], N, 1);
data_noise = data + noise_mse * randn(length(data), 1);
data_noise = max(0, min(1, round(data_noise)))

data_conv = convenc(data, trel);
data_conv_noise = data_conv + noise_mse * randn(length(data_conv), 1);
data_conv_noise = max(0, min(1, round(data_conv_noise)))
data_conv_dec = vitdec(data_conv_noise, trel, traceback, 'trunc', 'hard');

err = mean(data ~= data_noise)
err_conv = mean(data ~= data_conv_dec)
