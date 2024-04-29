% пример как сделать функцию
t = linspace(0, 1, 1000); % от 0 до 1

% X arrow
annotation('arrow', [0.9, 0.95], [0.1, 0.1], 'unit', 'normalized');
% Y arrow
annotation('arrow', [0.1, 0.1], [0.9, 0.95], 'unit', 'normalized');

xlabel('t')
ylabel('y')
title('Plot of power(t, 2.2) and power(t, 1/2.2) and linear')

box off % disable frame around plot

hold on; % retains the current plot when adding new plots

plot(t, power(t, 2.2));
plot(t, power(t, 1/2.2));
plot(t, t);

% Adding text labels to the plots
text(0.6, 0.3, {"power(t, 2.2)"});
text(0.3, 0.6, {"power(t, 1/2.2)"});
text(0.3, 0.3, {"y(t)"});

hold off; % new plots overwrite old now
