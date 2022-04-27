#include "cli.h"
#include <stdio.h>

#include <Arduino.h>

#include "command_functions.h"

void cli() 
{
    String str;

    int argc;
    char argv[3][16];


    if(Serial.available() > 0)
    {
        str = Serial.readStringUntil('\n');

        argc = sscanf(str.c_str(), "%s %s %s", argv[0], argv[1], argv[2]);

        if(!strcmp(argv[0], "d") && argc == 3) {
            //Set duty cycle at luminaire i: d <i> <val> 
            cmd_d(argv[1], argv[2]);
        }
        else if(!strcmp(argv[0], "r") && argc == 3) {
            //Set illuminance reference at luminaire i: r <i> <val>
            cmd_r(argv[1], argv[2]);
        }
        else if(!strcmp(argv[0], "o") && argc == 3) {
            //Set current occupancy state at desk <i>: o <i> <val>
            cmd_o(argv[1], argv[2]);
        }
        else if(!strcmp(argv[0], "a") && argc == 3) {
            //Set anti-windup state at desk <i>: a <i> <val>
            cmd_a(argv[1], argv[2]);
        }
        else if(!strcmp(argv[0], "w") && argc == 3) {
            //Set feedforward control state at desk <i>: w <i> <val>
            cmd_w(argv[1], argv[2]);
        }
        else if(!strcmp(argv[0], "b") && argc == 3) {
            //Set feedback control state at desk <i>: b <i> <val>
            cmd_b(argv[1], argv[2]);
        }
        else if(!strcmp(argv[0], "s") && !strcmp(argv[1], "l") && argc == 3) {
            //Start/stop stream of real-time variable l of desk <i>: s l <i>
            cmd_sl(argv[2]);
        }
        else if(!strcmp(argv[0], "s") && !strcmp(argv[1], "d") && argc == 3) {
            //Start/stop stream of real-time variable d of desk <i>: s d <i>
            cmd_sd(argv[2]);
        }
        else if(!strcmp(argv[0], "B") && !strcmp(argv[1], "l") && argc == 3) {
            //Get last minute buffer of variable l of desk <i>: B l <i>
            cmd_Bl(argv[2]);
        }
        else if(!strcmp(argv[0], "B") && !strcmp(argv[1], "d") && argc == 3) {
            //Get last minute buffer of variable d of desk <i>: B d <i>
            cmd_Bd(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "d") && argc == 3) {
            //Get current duty cycle at luminaire i: g d <i>
            cmd_gd(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "r") && argc == 3) {
            //Get current illuminance reference at luminaire i: g r <i>
            cmd_gr(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "l") && argc == 3) {
            //Get measured illuminance at luminaire i: g l <i>
            cmd_gl(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "o") && argc == 3) {
            //Get current occupancy state at luminaire i: g o <i>
            cmd_go(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "a") && argc == 3) {
            //Get anti-windup state at desk <i>: g a <i>
            cmd_ga(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "w") && argc == 3) {
            //Get feedforward control state at desk <i>: g w <i>
            cmd_gw(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "b") && argc == 3) {
            //Get feedback control state at desk <i>: g b <i>
            cmd_gb(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "x") && argc == 3) {
            //Get current external illuminance at desk <i>: g x <i>
            cmd_gx(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "p") && argc == 3) {
            //Get instantaneous power consumption at desk <i>: g p <i>
            cmd_gp(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "t") && argc == 3) {
            //Get elapsed time since last restart: g t <i>
            cmd_gt(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "e") && argc == 3) {
            //Get accumulated energy consumption at desk <i>: g e <i>
            cmd_ge(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "v") && argc == 3) {
            //Get accumulated visibility error at desk <i>: g v <i>
            cmd_gv(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "f") && argc == 3) {
            //Get accumulated flicker at desk <i>: g f <i>
            cmd_gf(argv[2]);
        }
        
        //** COMANDOS DA PARTE II **//
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "O") && argc == 3) {
            //Get lower bound on illuminance for Occupied state at desk <i>
            cmd_gO(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "U") && argc == 3) {
            //Get lower bound on illuminance for Unoccupied state at desk <i>
            cmd_gU(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "L") && argc == 3) {
            //Get current illuminance lower bound at desk <i>
            cmd_gL(argv[2]);
        }
        else if(!strcmp(argv[0], "g") && !strcmp(argv[1], "c") && argc == 3) {
            //Get current energy cost at desk <i>
            cmd_gc(argv[2]);
        }
        else if(!strcmp(argv[0], "O") && argc == 3) {
            //Set lower bound on illuminance for Occupied state at desk <i>
            cmd_O(argv[1], argv[2]);
        }
        else if(!strcmp(argv[0], "U") && argc == 3) {
            //Set lower bound on illuminance for Unoccupied state at desk <i>
            cmd_U(argv[1], argv[2]);
        }
        else if(!strcmp(argv[0], "c") && argc == 3) {
            //Set current energy cost at desk <i>
            cmd_c(argv[1], argv[2]);
        }
        else if(!strcmp(argv[0], "R") && argc == 1) {
            //Restarts system
            cmd_R();
        }
        else{
            Serial.println("err");
        }

    }
}

