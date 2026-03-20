% Input values
N = 20000;
noise_mse = 0.1 : 0.05 : 0.8;
trel = poly2trellis(3, [5 7]);
traceback = 5 * 3;

% Simulating
data = randi([0, 1], N, 1);
err_unc = zeros(size(noise_mse));
err_cod = zeros(size(noise_mse));
for i = 1:length(noise_mse)
    [err_unc(i), err_cod(i)] = sim_conv2vit(data, trel, traceback, noise_mse(i));
end

% Plotting
figure;
hold on;
semilogy(noise_mse, err_unc, 'b-o', 'LineWidth', 2);
semilogy(noise_mse, err_cod, 'r-s', 'LineWidth', 2);
hold off;
grid on;
xlabel('MSE (Noise Power)');
ylabel('Bit Error Rate');
legend('Uncoded', 'Convolutional Coded');

% Functions

function [err_unc, err_cod] = sim_conv2vit(data, trel, tb, mse)
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
    data_conv_dec = vitdec(data_conv_noise, trel, tb, 'trunc', 'hard');
    err_cod = mean(data ~= data_conv_dec);
end