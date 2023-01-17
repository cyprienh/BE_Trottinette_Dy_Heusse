%ftbf
%ftbf_z

clear all
close all

Kp=3.3/20;
K=58.33;
w0=20*pi;
t=tand(60-180)/w0;
ti=sqrt(((K*Kp)^2)*(1+t^2*w0^2)/(w0^4));

% Définition de G, F et C
s=tf('s');
F=K*Kp/s;
C=(1+t*s)/(ti*s);

[nF,dF] = tfdata(F);
[nC,dC] = tfdata(C);
nF=nF{1};
dF=dF{1};
nC=nC{1};
dC=dC{1};

% Tracé du diagramme de Bode du système en Boucle Ouverte

[C_pi,info] = pidtune(F,'PI',20*pi)

%bode(C_pi*F)
bode(C*F)

% Simulation du système en boucle fermée
%figure
%plot(out.i)
%figure
%plot(out.capteur)
%figure
%plot(out.epsilon)

% Transformée bilinéaire
% Te=100e-6 % 20us
% a1=Te/(2*ti)+t1/ti;
% a0=Te/(2*ti)-t1/ti;
% I=1/ti;
% 
% z = tf('z',Te);
% Cz=(a1*z+a0)/(z-1)
% [nCz,dCz]=tfdata(Cz);
% nCz=nCz{1};
% dCz=dCz{1};
% 
% out = sim('ftbf_z',[0 0.05]);
% figure
% plot(out.i)
% figure
% plot(out.epsilon)
% figure
% plot(out.capteur)