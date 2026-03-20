
N = 1000;
noise_mse = 1.0;

x = randi([0, 1], 1, N);
y = x + noise_mse * randn(1, N);

z = max(0, min(1, round(y)));

err = mean(x ~= z)