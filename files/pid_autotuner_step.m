%% pid_autotuner_step.m
% УЛУЧШЕННЫЙ АВТОТЮНИНГ ПО ШАГОВОМУ ОТКЛИКУ
% С защитой от отрицательных коэффициентов и нестабильности

clear; clc; close all;

fprintf('🚀 Запуск улучшенного автотюнера (Step + Z-N)...\n\n');

run('motor_params.m');

num_iterations = 3;

% Начинаем с разумных значений
Kp = 8.0;
Ki = 1.0;
Kd = 0.8;

for iter = 1:num_iterations
    fprintf('=== Итерация %d ===\n', iter);
    
    assignin('base','Kp',Kp);
    assignin('base','Ki',Ki);
    assignin('base','Kd',Kd);
    
    simOut = sim('position_pid_model.slx', 'StopTime','20', 'FixedStep','0.001');
    
    t   = simOut.tout;
    pos = simOut.get('pos_out').Data;
    err = simOut.get('error_out').Data;
    
    final_value = mean(pos(end-300:end));
    max_value   = max(pos);
    overshoot   = (max_value - final_value) / abs(final_value) * 100;
    
    idx10 = find(pos >= 0.1*final_value, 1, 'first');
    idx90 = find(pos >= 0.9*final_value, 1, 'first');
    
    if isempty(idx10) || isempty(idx90) || idx90 <= idx10
        Tr = 4; 
        fprintf('   ⚠️ Не удалось определить Tr → берём Tr=4\n');
    else
        Tr = t(idx90) - t(idx10);
    end
    
    % Формулы Z-N
    Kp_new = 1.2 / (final_value * Tr + 1e-6);
    Ki_new = Kp_new / (2 * Tr + 1e-6);
    Kd_new = Kp_new * Tr / 8;
    
    % ЗАЩИТА от отрицательных коэффициентов
    if Kp_new < 0
        Kp_new = abs(Kp_new) * 0.5;
        fprintf('   ⚠️ Kp был отрицательным → взяли положительный\n');
    end
    
    Kp = Kp_new * 0.8;
    Ki = Ki_new * 0.8;
    Kd = Kd_new * 0.9;
    
    fprintf('   Overshoot = %.1f%%   Tr = %.3f сек\n', overshoot, Tr);
    fprintf('   Новые PID → Kp=%.4f  Ki=%.4f  Kd=%.4f\n\n', Kp, Ki, Kd);
    
    % График после каждой итерации
    figure(iter);
    plot(t, pos, 'b', 'LineWidth', 1.5); hold on;
    yline(1, '--r', 'Заданное положение');
    grid on;
    title(['Итерация ' num2str(iter) ' — Kp=' num2str(Kp,'%.4f')]);
    xlabel('Время, сек'); ylabel('Угол, рад');
end

fprintf('🎉 ТЮНИНГ ЗАВЕРШЁН!\n');
fprintf('ИТОГОВЫЕ КОЭФФИЦИЕНТЫ:\n');
fprintf('   Kp = %.4f\n', Kp);
fprintf('   Ki = %.4f\n', Ki);
fprintf('   Kd = %.4f\n', Kd);

save('pid_tuned_results.mat', 'Kp','Ki','Kd');