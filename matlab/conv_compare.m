% comparing conv57 & vitdec

addpath('conv57');

trel = poly2trellis(3, [5 7]);

data = [1, 1, 0, 0];

data_conv = convenc(data, trel);
data_conv_noise = data_conv > 0;
data_conv_noise(7) = 0;
data_conv_noise(8) = 0;

data_conv_dec = vitdec(data_conv_noise, trel, 1, 'trunc', 'hard');

data_conv_dec_our = conv57dechard(data_conv_noise);

data
data_conv
data_conv_noise
data_conv_dec
data_conv_dec_our(3:end)
