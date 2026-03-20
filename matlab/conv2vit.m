% Input values
N = 1000;
noise_mse = 0.3;
trel = poly2trellis(3, [5 7]);
traceback = 5 * 3;

% Simulating
data = randi([0, 1], N, 1);
[erru, errc] = sim_conv2vit(data, trel, traceback, 0.3)

function [err_unc, err_cod] = sim_conv2vit(data, trel, traceback, mse)
    % Uncoded
    data_noise = data + mse * randn(length(data), 1);
    data_noise = round(data_noise);
    data_noise = max(0, min(1, data_noise));
    err_unc = mean(data ~= data_noise);
    
    % Coded
    data_conv = convenc(data, trel);
    data_conv_noise = data_conv + mse * randn(length(data_conv), 1);
    data_conv_noise = round(data_conv_noise);
    data_conv_noise = max(0, min(1, data_conv_noise));
    data_conv_dec = vitdec(data_conv_noise, trel, traceback, 'trunc', 'hard');
    err_cod = mean(data ~= data_conv_dec);
end