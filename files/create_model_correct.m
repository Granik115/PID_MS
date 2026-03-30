%% create_model_correct.m
% ИСПРАВЛЕННАЯ МОДЕЛЬ С ПРАВИЛЬНЫМ СОЛВЕРОМ И ОТРИЦАТЕЛЬНОЙ ОБРАТНОЙ СВЯЗЬЮ

clear; clc;

model_name = 'position_pid_model';

if bdIsLoaded(model_name), close_system(model_name,0); end
if exist([model_name '.slx'],'file'), delete([model_name '.slx']); end

new_system(model_name);
open_system(model_name);

% Правильные настройки солвера для MATLAB R2018b и новее
set_param(model_name, ...
    'SolverType', 'Fixed-step', ...
    'Solver', 'ode4', ...           % Runge-Kutta
    'FixedStep', '0.001', ...
    'StopTime', '20');

fprintf('✅ Создаём модель с правильным солвером ode4...\n');

% Блоки
add_block('simulink/Sources/Step', [model_name '/Setpoint'], 'Position', [100 100 130 130]);
set_param([model_name '/Setpoint'], 'After', '1');

add_block('simulink/Math Operations/Sum', [model_name '/Error'], 'Position', [200 180 230 220]);
set_param([model_name '/Error'], 'Inputs', '+-');

add_block('simulink/Continuous/PID Controller', [model_name '/PID'], 'Position', [300 170 350 230]);
set_param([model_name '/PID'], 'P','Kp','I','Ki','D','Kd', ...
          'UpperSaturationLimit','100','LowerSaturationLimit','-100');

add_block('simulink/Continuous/Transfer Fcn', [model_name '/Plant'], 'Position', [420 170 490 230]);
set_param([model_name '/Plant'], 'Numerator','[1]','Denominator','[J B 0]');

add_block('simulink/Discrete/Unit Delay', [model_name '/Delay'], 'Position', [550 220 580 260]);

add_block('simulink/Sinks/To Workspace', [model_name '/pos_out'], 'Position', [620 100 670 130], 'VariableName','pos_out','SaveFormat','Timeseries');
add_block('simulink/Sinks/To Workspace', [model_name '/error_out'], 'Position', [620 200 670 230], 'VariableName','error_out','SaveFormat','Timeseries');
add_block('simulink/Sinks/To Workspace', [model_name '/u_out'], 'Position', [620 300 670 330], 'VariableName','u_out','SaveFormat','Timeseries');

add_block('simulink/Sinks/Scope', [model_name '/Scope'], 'Position', [720 150 770 280]);

% Соединения (отрицательная ОС)
add_line(model_name, 'Setpoint/1', 'Error/1');
add_line(model_name, 'PID/1', 'Plant/1');
add_line(model_name, 'Plant/1', 'pos_out/1');
add_line(model_name, 'Plant/1', 'Scope/1');
add_line(model_name, 'Plant/1', 'Delay/1');
add_line(model_name, 'Delay/1', 'Error/2');
add_line(model_name, 'Error/1', 'PID/1');
add_line(model_name, 'PID/1', 'u_out/1');
add_line(model_name, 'Error/1', 'error_out/1');

save_system(model_name);
close_system(model_name, 0);

fprintf('✅ МОДЕЛЬ УСПЕШНО СОЗДАНА\n');
fprintf('Теперь запускай pid_autotuner_step\n');