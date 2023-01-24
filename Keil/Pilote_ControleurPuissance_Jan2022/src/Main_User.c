
/*
	!!!! NB : ALIMENTER LA CARTE AVANT DE CONNECTER L'USB !!!

VERSION 16/12/2021 :
- ToolboxNRJ V4
- Driver version 2021b (synchronisation de la mise à jour Rcy -CCR- avec la rampe)
- Validé Décembre 2021

*/


/*
STRUCTURE DES FICHIERS

COUCHE APPLI = Main_User.c : 
programme principal à modifier. Par défaut hacheur sur entrée +/-10V, sortie 1 PWM
Attention, sur la trottinette réelle, l'entrée se fait sur 3V3.
Attention, l'entrée se fait avec la poignée d'accélération qui va de 0.6V à 2.7V !

COUCHE SERVICE = Toolbox_NRJ_V4.c
Middleware qui configure tous les périphériques nécessaires, avec API "friendly"

COUCHE DRIVER =
clock.c : contient la fonction Clock_Configure() qui prépare le STM32. Lancée automatiquement à l'init IO
lib : bibliothèque qui gère les périphériques du STM : Drivers_STM32F103_107_Jan_2015_b
*/

#include "ToolBox_NRJ_v4.h"
#include <math.h>

//=================================================================================================================
// 					USER DEFINE
//=================================================================================================================
#define Te 1e-4
// Choix de la fréquence PWM (en kHz)
#define FPWM_Khz 20.0

//==========END USER DEFINE========================================================================================

// ========= Variable globales indispensables et déclarations fct d'IT ============================================

void IT_Principale(void);
//=================================================================================================================

/*=================================================================================================================
 					FONCTION MAIN : 
					NB : On veillera à allumer les diodes au niveau des E/S utilisée par le progamme. 
					
					EXEMPLE: Ce progamme permet de générer une PWM (Voie 1) à 20kHz dont le rapport cyclique se règle
					par le potentiomètre de "l'entrée Analogique +/-10V"
					Placer le cavalier sur la position "Pot."
					La mise à jour du rapport cyclique se fait à la fréquence 1kHz.

//=================================================================================================================*/
float Te_us;

float ti = 0.002899536956069;
float t1;

// Variables pour auto-identification de tau
#define s 100			// nombre total de samples après transition
#define t 100		 	// nombre de Te avant transition
char tau_trouve = 0;
float val_tau;
int m = 0;
int n = 0;
int i;
int j;
float samples[s];

// Variables pour la commande de couple
int Courant_1,Cons_In,Pot_In;
float I1_val,Vin_val,epsilon,out,out_sat;
float uk_prev = 0.0;
float yk_prev = 0.0;
int r_cyc;

float a0,a1;

int main (void) {
  // !OBLIGATOIRE! //	
  Conf_Generale_IO_Carte();	
    

    
  // ------------- Discret, choix de Te -------------------	
  Te_us=Te*1000000.0; // conversion en µs pour utilisation dans la fonction d'init d'interruption

  //______________ Ecrire ici toutes les CONFIGURATIONS des périphériques ________________________________	
  // Paramétrage ADC pour entrée analogique
  Conf_ADC();
  // Configuration de la PWM avec une porteuse Triangle, voie 1 & 2 activée, inversion voie 2
  Triangle (FPWM_Khz);
  Active_Voie_PWM(1);	
  Active_Voie_PWM(2);	
  Inv_Voie(2);

  Start_PWM;
  R_Cyc_1(2048);  // positionnement à 50% par défaut de la PWM
  R_Cyc_2(2048);

  // Activation LED
  LED_Courant_On;
  LED_PWM_On;
  LED_PWM_Aux_Off;
  LED_Entree_10V_On;
  LED_Entree_3V3_Off;
  LED_Codeur_Off;

  // Conf IT
  Conf_IT_Principale_Systick(IT_Principale, Te_us);

	while(1);
}

// Auto-identification du tau du moteur
void Trouver_tau(float I1_val) {
	if (m < t) {
    // Moteur à 40% pendant t*Te (100*Te)
		r_cyc = (int)4096*0.4;
		m++;
	} else if(n < s) {
    // Moteur à 60% et enregistrement des valeurs
		r_cyc = (int)4096*0.6;
		samples[n] = I1_val;
		n++;
	} else {
    // Identification de la valeuren régime permanent et de 63% -> tau
		for(i = 2; i<s; i++) {
			if(fabs(samples[i]-samples[i-1]) < 0.01) {
				val_tau = samples[1] + 0.63 * (samples[i]-samples[1]);
				for(j=1; j<i; j++) {
					if(samples[j] > val_tau) {
						tau_trouve = 1;
						t1 = j * 1e-4;
						a0=Te/(2*ti)-t1/ti;
						a1=Te/(2*ti)+t1/ti;
						break;
					}
				}
				break;						
			}
		}
	}
}

//=================================================================================================================
// 					FONCTION D'INTERRUPTION PRINCIPALE SYSTICK
//=================================================================================================================
void IT_Principale(void) {
	Courant_1=I1();				// Lecture de la valeur du courant I1 -> retour
	Pot_In=Entree_3V3();	// Lecture de la valeur de la tension du potentiomètre -> consigne
	
	// Valeurs en V
	I1_val = (float)Courant_1*3.3/4096.0;
	Vin_val = (float)Pot_In*3.3/4096.0;
	
	if(tau_trouve) {
		epsilon=Vin_val-I1_val;
		out=a1*epsilon+a0*uk_prev+yk_prev;
		
    // Saturation
		if(out < -0.5) {
			out_sat = -0.5;
		} else if(out > 0.5) {
			out_sat = 0.5;
		} else {
			out_sat = out;
		}
		
		uk_prev = epsilon;
		yk_prev = out_sat;  // Utiliser out_sat permet l'anti-aliasing
		
		out_sat += 0.5;
		
		r_cyc = (int)(4096.0*out_sat);

	} else {
    Trouver_tau(I1_val);
	}
	
	R_Cyc_1(r_cyc);
	R_Cyc_2(r_cyc);
}
