A
jsim_n dff5-t.js > /tmp/dff5.dat
cutt dff5
% ext dff5
mv /tmp/dff5.ext ~/sim/dff/dff5.ext
xmgrace -autoscale xy -nxy dff5.ext &
