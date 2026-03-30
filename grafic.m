
assignin('base','Kp',Kp);
assignin('base','Ki',Ki);
assignin('base','Kd',Kd);

simOut = sim('position_pid_model.slx', 'StopTime','30');
t = simOut.tout;
pos = simOut.get('pos_out').Data;

figure;
plot(t, pos, 'b', 'LineWidth', 2);
yline(1, '--r', 'Заданное = 1 рад');
grid on;
title('Проверка PID');