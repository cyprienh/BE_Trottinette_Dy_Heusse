%ftbf
%ftbf_z

clear all
close all

% Paramètres de G(s)
Kh=24;
t1=2e-3;

% Paramètres de F(s)
Kc=0.1042; % V/A
K =1.457;
t2=7.43e-5;
t3=4.84e-6;

% Tension E
E=0;

% Paramètre du correcteur P.I.
%ti=(K*Kc*Kh)/(2*pi*400)
ti=0.0014
%   On veut abs(Kglobal/(ti*p)) = 1 -> système simplifié
%   p = j*w = j*2*pi*f

% Définition de G, F et C
s=tf('s');
G=Kh/(1+t1*s);
F=Kc*K/((1+t2*s)*(1+t3*s));
C=(1+t1*s)/(ti*s);

[nG,dG] = tfdata(G);
[nF,dF] = tfdata(F);
[nC,dC] = tfdata(C);
nG=nG{1};
dG=dG{1};
nF=nF{1};
dF=dF{1};
nC=nC{1};
dC=dC{1};

% Tracé du diagramme de Bode du système en Boucle Ouverte
%bode(C*G*F)

% Simulation du système en boucle fermée
Kp = 0.6;
out = sim('ftbf',[0 0.2]);
%figure
%plot(out.i)
%figure
%plot(out.capteur)
%figure
%plot(out.epsilon)

% Transformée bilinéaire
Te=100e-6 % 20us
a1=Te/(2*ti)+t1/ti;
a0=Te/(2*ti)-t1/ti;
I=1/ti;

z = tf('z',Te);
Cz=(a1*z+a0)/(z-1)
[nCz,dCz]=tfdata(Cz);
nCz=nCz{1};
dCz=dCz{1};

out = sim('ftbf_z',[0 0.05]);
figure
plot(out.i)
figure
plot(out.epsilon)
figure
plot(out.capteur)