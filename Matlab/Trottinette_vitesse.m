%ftbf
%ftbf_z

clear all
close all

Kp=3.3/20;
K=58.33;
w0=20*pi;
t=tand(60-180)/w0;
ti=sqrt(((K*Kp)^2)*(1+t^2*w0^2)/(w0^4));

% Definition de G, F et C
s=tf('s');
F=K*Kp/s;
C=(1+t*s)/(ti*s);

[nF,dF] = tfdata(F);
[nC,dC] = tfdata(C);
nF=nF{1};
dF=dF{1};
nC=nC{1};
dC=dC{1};

% Trace du diagramme de Bode du systeme en Boucle Ouverte

%bode(C_pi*F)
bode(C*F)

% Cette commande permet de trouver un contrôleur P.I. fonctionnel avec les specifications
% demandée mais ce n'est pas ce que nous avons finalement utilisé.
[C_pi,info] = pidtune(F,'PI',20*pi)
