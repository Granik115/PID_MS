%% motor_params.m
% Обновлённые параметры (чтобы модель не взрывалась)

Rs   = 0.45;
Ld   = 1.2e-3;
Lq   = 1.2e-3;
Ke   = 0.072;
pp   = 4;

J    = 0.05;        % ← было 2.5e-5, теперь нормально
B    = 0.01;        % ← было 1.8e-6, теперь нормально
Tc   = 0.008;

U_n  = 24;
I_n  = 2.5;
M_n  = 0.35;
n_n  = 3000;

alpha_Rs = 0.0039;

h_relay   = 20;
sim_time  = 25;
Ts        = 0.001;

Kp_init = 8;
Ki_init = 1.5;
Kd_init = 0.8;

enable_temp_correction = true;
T_current = 25;

fprintf('✅ motor_params.m загружен (обновлённые стабильные значения).\n');