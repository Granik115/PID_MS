%% pid_autotuner_optimization.m
% УЛУЧШЕННЫЙ АВТОТЮНИНГ С ЖЁСТКИМИ ОГРАНИЧЕНИЯМИ

clear; clc; close all;

fprintf('🚀 УЛУЧШЕННЫЙ АВТОТЮНИНГ (ITAE + жёсткие штрафы)...\n\n');

run('motor_params.m');

x0 = [8, 1.5, 0.8];   % стартовые значения

options = optimset('Display','iter', 'MaxIter', 50, 'TolFun', 1e-4);

fprintf('Запускаем оптимизацию...\n\n');

[x_opt, fval] = fminsearch(@(x) objective_function(x), x0, options);

Kp = max(x_opt(1), 0.1);
Ki = max(x_opt(2), 0.01);
Kd = max(x_opt(3), 0);

fprintf('\n🎉 ОПТИМИЗАЦИЯ ЗАВЕРШЕНА!\n');
fprintf('Лучшие коэффициенты:\n');
fprintf('   Kp = %.4f\n', Kp);
fprintf('   Ki = %.4f\n', Ki);
fprintf('   Kd = %.4f\n', Kd);
fprintf('ITAE = %.4f\n', fval);

save('pid_tuned_results.mat', 'Kp','Ki','Kd');

% ================================================
function ITAE = objective_function(x)
    Kp = x(1);
    Ki = x(2);
    Kd = x(3);

    % ЖЁСТКИЕ ШТРАФЫ
    penalty = 0;
    if Ki < 0, penalty = penalty + 1e6 * abs(Ki); end
    if Kd < 0, penalty = penalty + 1e6 * abs(Kd); end
    if Kp < 0, penalty = penalty + 1e6 * abs(Kp); end
    if Kp > 100, penalty = penalty + 1e5 * (Kp-100); end
    if Ki > 30,  penalty = penalty + 1e5 * (Ki-30); end

    assignin('base','Kp',Kp);
    assignin('base','Ki',Ki);
    assignin('base','Kd',Kd);

    try
        simOut = sim('position_pid_model.slx', 'StopTime','25', 'FixedStep','0.001');
        t   = simOut.tout;
        pos = simOut.get('pos_out').Data;
        
        error = 1 - pos;
        ITAE = trapz(t, t .* abs(error)) + penalty;
    catch
        ITAE = 1e10;
    end
end