#include "simulator.h"
#include "pi.h"
#include "circular_buffer.h"

#include <string>
#include <sstream>
#include <vector>
#include <tuple>
using namespace std;

int DAC_RANGE = 4095;
int analogReadRes = {12};

float Vcc = {3.3};
float Rdivider = {10000};
float ledP = 1; //led power in Watt
short int LED_PIN = 15;
int ADC0_PIN = A0;

int printL_left = 0, printD_left = 0, buf_ptr = 0;
bool calibrate = true, first_loop = true, ref_change = true;
bool streamL = false, streamD = false;
float static_gain, external_lum, Rlux;
float prev_dc = 0, duty_cycle, fb_PWM, ff_PWM; //u, system input (from 0 to 1)
float Yadc, Yadc_sum = 0, Ylux, fb_Rlux, Ylux_prev, Ylux_prev_prev;
float ti, vi, xf, vt;
float total_energy = 0, total_vis_sum = 0, total_flicker_sum = 0; 
String inFull = "", tokens[10];
unsigned long t_start = 0, t_startSim = 0, t_stream = 1, n_samples = 0;

const float m = {-1.05};
const float b = log10(200000)-m; //200 kOhm is the median value for LDR when lux is 10

const int buffer_size = 6000;

struct my_data{
  float duty_cycle; float illuminance;
  my_data(float dc = 0.0f, float lux = 0.0f):
    duty_cycle {dc}, illuminance {lux} {};
    inline void print_l() const {Serial.print(illuminance);}
    inline void print_d() const {Serial.print(duty_cycle);}
};

////////////////////////////////////////////////////////////////////////////////////
bool occ_rate = true;
bool ff_control = true, fb_control = true, anti_windup = true; //saturate integral term
float time_step = 0.01; //time between simulator samples in seconds (default 10 ms)
float OccLux = 20, noOccLux = 5; //reference in lux for occupancy true/false
float kp = 0.01; 
float ki = 0.1;
short int ref_duty_cycle = 100; //duty cycle when ff and fb control are off (from 0 to 100 in %)
////////////////////////////////////////////////////////////////////////////////////

float mili_time_step =  time_step * pow(10, 3);
float micro_time_step =  time_step * pow(10, 6);

pi cont(kp, ki, time_step);
sim simu{m, b, Vcc, Rdivider};
circular_buffer<my_data, buffer_size> cbuf;

bool isInt(const string & str){
   if(str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+'))) return false;

   char *ptr;
   strtol(str.c_str(), &ptr, 10);

   return (*ptr == 0);
}

bool isFloat(const string & str){
    if (str.empty()) return false;

    char* ptr;
    strtof(str.c_str(), &ptr);
    
    return (*ptr == 0);
}

float calibrate_gain(){
  analogWrite(LED_PIN, 0);
  delay(1); 
  
  analogWrite(LED_PIN, 0.1 * DAC_RANGE);
  delay(1000); 
  
  float lux1 = adc_to_LUX(analogRead(ADC0_PIN));

  analogWrite(LED_PIN, 0.95 * DAC_RANGE);
  delay(1000); 
  
  float lux2 = adc_to_LUX(analogRead(ADC0_PIN));

  float gain = (lux2 - lux1)/(0.95 - 0.1);

  analogWrite(LED_PIN, 0);
  delay(1);
  return gain;
}


float adc_to_LUX(int adc){
  return volt_to_LUX(adc_to_volt(adc));
}

float volt_to_LUX(float Vadc){
  return LDR_to_LUX(Rdivider * (Vcc/Vadc - 1));
}

inline float LDR_to_LUX(float LDR){
  return pow(10, (log10(LDR)-b)/m);
}

inline float adc_to_volt(int adc){
  return adc * Vcc / pow(2, analogReadRes);
}

inline float LUX_to_volt(float LUX){
  return Vcc * Rdivider /(pow(10, m*log10(LUX)+b) + Rdivider);
}

void answerCommand(const char* token0, const char* token1, const char* token2){
  //set duty cycle
  if(string(token0) == "d"){
        if(isInt(token1)){
          if(atoi(token1) <= 3 && atoi(token1) >= 1){
            if(isInt(token2)) {
                if(atoi(token2) <= 100 && atoi(token2) >= 0){
                  int L = atoi(token1);
                  ref_duty_cycle = atoi(token2); 
                  Serial.println("ack"); 
                  return;
                }
                else{
                  Serial.println("err: duty cycle between 0 and 100 (%)");
                  return;
                }
              }
              else{
                Serial.println("err: inserted duty cycle not an integer");
                return;
              }
            }
            else{
              Serial.println("err: choose a luminaire between 1 and 3");
              return;
            }
          }
          else{
            Serial.println("err: inserted luminaire not an integer"); 
            return;      
          }
  }
  //get commands         
  else if(string(token0) == "g"){
        if(isInt(token2)){
            if(atoi(token2) <= 3 && atoi(token2) >= 1){
            }
            else{
              Serial.println("err: choose a luminaire between 1 and 3");
              return;
            }
        }
        else{
          Serial.println("err: inserted luminaire not an integer");
          return;
        }
          
        if(string(token1) == "d"){
          //get duty_cycle
          
          int L = atoi(token2);
          Serial.print("d "); Serial.print(L); Serial.print(" "); Serial.print(duty_cycle); Serial.println("%");
          return;
        }
        else if(string(token1) == "r"){
          //get reference illuminance level
          
          int L = atoi(token2);
          Serial.print("r "); Serial.print(L); Serial.print(" "); Serial.print(OccLux);Serial.println(" lx"); 
          return;
        }
        else if(string(token1) == "n"){
          //get no occupancy reference illuminance level
          
          int L = atoi(token2);
          Serial.print("n "); Serial.print(L); Serial.print(" "); Serial.print(noOccLux);Serial.println(" lx"); 
          return;
        }
        else if(string(token1) == "l"){
          //get illuminance level
          
          int L = atoi(token2);
          Serial.print("l "); Serial.print(L); Serial.print(" "); Serial.print(Ylux); Serial.println(" lx");
          return;
        }
        else if(string(token1) == "o"){
          //get occupancy rate
          
          int L = atoi(token2);
          Serial.print("o "); Serial.print(L); Serial.print(" "); Serial.println(occ_rate);
          return;
        }
        else if(string(token1) == "a"){
          //get anti_windup state

          int L = atoi(token2);
          Serial.print("a "); Serial.print(L); Serial.print(" "); Serial.println(anti_windup);
          return;
        }
        else if(string(token1) == "w"){
          //get feedforward control state

          int L = atoi(token2);
          Serial.print("w "); Serial.print(L); Serial.print(" "); Serial.println(ff_control);
          return;
        }
        else if(string(token1) == "b"){
          //get feedback control state
          
          int L = atoi(token2);
          Serial.print("b "); Serial.print(L); Serial.print(" "); Serial.println(fb_control);
          return;
        }
        else if(string(token1) == "x"){
          //get external illuminance
          
          int L = atoi(token2);
          external_lum = Ylux - static_gain * duty_cycle / 100;
          Serial.print("x "); Serial.print(L); Serial.print(" "); Serial.print(external_lum);Serial.println(" lx");
          return;
        }
        else if(string(token1) == "p"){
          //get instantaneous power consumption
          
          int L = atoi(token2);

          float power = duty_cycle / 100 * ledP;
          Serial.print("p "); Serial.print(L); Serial.print(" "); Serial.print(power); Serial.println(" W");
          return;
        }
        else if(string(token1) == "t"){
          //get instantaneous power consumption
          
          int L = atoi(token2);
          Serial.print("t "); Serial.print(L); Serial.print(" "); Serial.print((micros() - t_start)/pow(10,6)); Serial.println(" s");
          return;
        }
        else if(string(token1) == "e"){
          //get energy consumption since reset
          
          int L = atoi(token2);
          Serial.print("e "); Serial.print(L); Serial.print(" "); Serial.print(total_energy); Serial.println(" J");
          return;
        }
        else if(string(token1) == "v"){
          //get visibility error since reset
          
          int L = atoi(token2);
          Serial.print("v "); Serial.print(L); Serial.print(" "); Serial.print(total_vis_sum/n_samples); Serial.println(" lx");
          return;
        }
        else if(string(token1) == "f"){
          //get flicker error since reset
          
          int L = atoi(token2);
          Serial.print("f "); Serial.print(L); Serial.print(" "); Serial.print(total_flicker_sum/n_samples); Serial.println(" lx/s");
          return;
        }
        else{
          Serial.println("err: g options are d, r, l, o, a, w, b, x, p, t, e, v, f");
          return;
        }
  }
  //set illuminance reference
  else if(string(token0) == "r"){
        if(isInt(token1)) {
          if(atoi(token1) <= 3 && atoi(token1) >= 1){
            if(isFloat(token2)){
              if(stof(token2) >= 0){
                int L = atoi(token1);
                OccLux = stof(token2);
                Serial.println("ack");
                if(occ_rate) ref_change = true; //reference lux changed, but only if occupancy was already true
                return;
              }
              else{
                Serial.println("err: lux must be greater or equal to 0");
                return;
              }
            }
            else{
              Serial.println("err: inserted lux not a float");
              return;
            }
          }
          else{
            Serial.println("err: choose a luminaire between 1 and 3");  
            return; 
          }
        }
        else{
          Serial.println("err: inserted luminaire not an integer");
          return;
        }
  }
  //set no occupancy illuminance reference
  else if(string(token0) == "n"){
        if(isInt(token1)) {
          if(atoi(token1) <= 3 && atoi(token1) >= 1){
            if(isFloat(token2)){
              if(stof(token2) >= 0){
                int L = atoi(token1);
                noOccLux = stof(token2);
                Serial.println("ack");
                if(!occ_rate) ref_change = true; //reference lux changed, but only if occupancy was already false
                return;
              }
              else{
                Serial.println("err: lux must be greater or equal to 0");
                return;
              }
            }
            else{
              Serial.println("err: inserted lux not a float");
              return;
            }
          }
          else{
            Serial.println("err: choose a luminaire between 1 and 3");  
            return; 
          }
        }
        else{
          Serial.println("err: inserted luminaire not an integer");
          return;
        }
  }
  //set occupancy rate
  else if(string(token0) == "o"){
        if(isInt(token1)) {
          if(atoi(token1) <= 3 && atoi(token1) >= 1){
            if(isInt(token2)){
              if(atoi(token2) == 0 || atoi(token2) == 1){                
                int L = atoi(token1);
                occ_rate = atoi(token2);
                ref_change = true;
                Serial.println("ack");
                return;
              }
              else{
                Serial.println("err: occupancy rate is 1 or 0");
                return;
              }
            }
            else{
              Serial.println("err: inserted lux not a float");
              return;
            }
          }
          else{
            Serial.println("err: choose a luminaire between 1 and 3");  
            return; 
          }
        }
        else{
          Serial.println("err: inserted luminaire not an integer");
          return;
        }
  }
  //set anti-windup
  else if(string(token0) == "a"){
        if(isInt(token1)) {
          if(atoi(token1) <= 3 && atoi(token1) >= 1){
            if(isInt(token2)){
              if(atoi(token2) == 0 || atoi(token2) == 1){
                int L = atoi(token1);
                anti_windup = atoi(token2);
                Serial.println("ack");
                return;
              }
              else{
                Serial.println("err: anti-windup is 1 or 0");
                return;
              }
            }
            else{
              Serial.println("err: inserted lux not a float");
              return;
            }
          }
          else{
            Serial.println("err: choose a luminaire between 1 and 3");   
            return;
          }
        }
        else{
          Serial.println("err: inserted luminaire not an integer");
          return;
        }
  }
  //set feed forward control
  else if(string(token0) == "w"){
        if(isInt(token1)) {
          if(atoi(token1) <= 3 && atoi(token1) >= 1){
            if(isInt(token2)){
              if(atoi(token2) == 0 || atoi(token2) == 1){           
                int L = atoi(token1);
                ff_control = atoi(token2);
                Serial.println("ack");
                return;
              }
              else{
                Serial.println("err: feedforward control is 1 or 0");
                return;
              }
            }
            else{
              Serial.println("err: inserted lux not a float");
              return;
            }
          }
          else{
            Serial.println("err: choose a luminaire between 1 and 3");   
            return;
          }
        }
        else{
          Serial.println("err: inserted luminaire not an integer");
          return;
        }
  }
  //set feedback control
  else if(string(token0) == "b"){
        if(isInt(token1)) {
          if(atoi(token1) <= 3 && atoi(token1) >= 1){
            if(isInt(token2)){
              if(atoi(token2) == 0 || atoi(token2) == 1){               
                int L = atoi(token1);
                fb_control = atoi(token2);
                Serial.println("ack");
                return;
              }
              else{
                Serial.println("err: feedback is 1 or 0");
                return;
              }
            }
            else{
              Serial.println("err: inserted lux not a float");
              return;
            }
          }
          else{
            Serial.println("err: choose a luminaire between 1 and 3");   
            return;
          }
        }
        else{
          Serial.println("err: inserted luminaire not an integer");
          return;
        }
  }
  //start/stop stream
  else if(string(token0) == "s"){
        if(isInt(token2)) {
          if(atoi(token2) <= 3 && atoi(token2) >= 1){
            int L = atoi(token2); 
            //start/stop illuminance stream
            if(string(token1) == "l"){               
              if(streamL){
                streamL = false;
                Serial.println("ack");
                return;
              }

              streamD = false;
              streamL = true;
              t_stream = 0;
              Serial.println("ack");
              return;
            }
            //start/stop duty_cycle stream
            else if(string(token1) == "d"){              
              if(streamD){
                streamD = false;
                Serial.println("ack");
                return;
              }
              
              streamL = false;
              streamD = true;
              t_stream = 0;
              Serial.println("ack");
              return;
            }
            else{
              Serial.println("err: s options: l or d");   
              return;
            }
          }
          else{
            Serial.println("err: choose a luminaire between 1 and 3");   
            return;
          }
        }
        else{
          Serial.println("err: inserted luminaire not an integer");
          return;
        }
  }
  //show buffer
  else if(string(token0) == "B"){
        if(isInt(token2)) {
          if(atoi(token2) <= 3 && atoi(token2) >= 1){
            int L = atoi(token2); 
            //show illuminance buffer
            if(string(token1) == "l"){  
              streamL = false; streamD = false;
              Serial.print("B "); Serial.print("l "); Serial.print(L); Serial.print(" ");                                   
              printL_left = buffer_size;     
              buf_ptr = cbuf.get_tail();       
              return;
            }
            else if(string(token1) == "d"){
              //show duty cycle buffer
              streamL = false; streamD = false;
              Serial.print("B "); Serial.print("d "); Serial.print(L); Serial.print(" ");                                   
              printD_left = buffer_size;
              buf_ptr = cbuf.get_tail();      
              return;
            }
            else{
              Serial.println("err: B options: l or d");   
              return;
            }
          }
          else{
            Serial.println("err: choose a luminaire between 1 and 3");   
            return;
          }
        }
        else{
          Serial.println("err: inserted luminaire not an integer");
          return;
        }
  }
  else if(string(token0) == "R"){
    calibrate = true;
    streamL = false; 
    streamD = false;
    Serial.println("ack");    
    return;
  }
  else{
    Serial.println("err: options are g, d, r, n, o, a, w, b, s, B, R");
    return;
  }
}


volatile unsigned long int t_sample{0};
volatile bool sample_time_up{false};
struct repeating_timer timer;

bool sample_time_handler(struct repeating_timer *t){
  if(!sample_time_up){
    t_sample = micros();
    sample_time_up = true;
  }
  else Serial.println("Sample time exceeded the step time!!!!!");
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  Serial.begin(115200);

  while(Serial); //wait to close Serial
  while(!Serial); //wait to open Serial
  
  delay(1000);
  
  analogReadResolution(analogReadRes); //default is 12
  analogWriteFreq(60000); //60KHz, about max
  analogWriteRange(DAC_RANGE); //100% duty cycle
}

void loop(){
  
  if(calibrate){
    calibrate = false;
    
    //restart timer when calibrating
    if(!first_loop) cancel_repeating_timer(&timer);
    else first_loop = false;
    
    Serial.print("Calibrating...");
    
    static_gain = calibrate_gain();
    
    //total energy consumption, total visibility error, total flicker error and number of samples since last reset
    total_energy = 0; total_vis_sum = 0; total_flicker_sum = 0; n_samples = 0;

    //reset buffer to 0
    for(int j = 0; j < buffer_size; j++){
      if(cbuf.is_full()) cbuf.take();
      cbuf.put(my_data{0.0f, 0.0f}); 
    }

    Serial.println("done.");

    //start sample_timer
    t_start = micros();
    add_repeating_timer_us(-micro_time_step, sample_time_handler, NULL, &timer);
  }

  //take sample if sample_time is up
  if(sample_time_up){
    sample_time_up = false;
    
    int i = 0;
    Yadc_sum = 0;
    //time to take measures for a given PWM
    while(micros() - t_sample < 0.1 * micro_time_step){ //digital filter
        Yadc_sum += analogRead(ADC0_PIN);
        i++;
    }
    Yadc = Yadc_sum/i;
    Ylux = adc_to_LUX(Yadc);
    
    n_samples++;
  
    if(occ_rate) Rlux = OccLux; 
    else Rlux = noOccLux; //if no occupancy, set reference to no occupancy level
    
    //turn on/off feedforward control
    if(ff_control) ff_PWM = Rlux/static_gain; 
    else ff_PWM = 0;

    //turn on/off feedback control
    if(fb_control){ 
      if(ref_change){ //if user issued a ref lux change command, need to recalculate the simulator
        ref_change = false;
  
        float simLux = Rlux;
        if(Rlux/static_gain >= 1) simLux = static_gain;

        //calculate tau and readies the class
        simu.dim_change(static_gain, adc_to_volt(Yadc), simLux, 0);  //simu.dim_change(static_gain, vi, xf, ti);

        fb_Rlux = 0;
        t_startSim = micros();
      }

      //having the tau, just needs to calculate voltage
      fb_Rlux = volt_to_LUX(simu.get_vt((micros() - t_startSim)/pow(10,6))); //new lux reference for the FB based on the simulator
      fb_PWM = cont.calc_pwm(fb_Rlux, Ylux, anti_windup, ff_PWM);
    }
    else fb_PWM = 0; 
  
    prev_dc = duty_cycle;
    
    //sum the components of feedforward and feedback
    duty_cycle = (ff_PWM + fb_PWM)*100;
    if(!ff_control && !fb_control) duty_cycle = ref_duty_cycle;
    
    if(duty_cycle > 100) duty_cycle = 100;
    else if(duty_cycle < 0) duty_cycle = 0;
    analogWrite(LED_PIN, duty_cycle * DAC_RANGE / 100);


    if(!t_stream) t_stream = micros();
    if(streamL){
      Serial.print("s l "); Serial.print("1");  Serial.print(" "); Serial.print(Ylux); Serial.print(" lx "); Serial.print((micros() - t_stream)/pow(10,3),0); Serial.println(" ms");
    }
    if(streamD){
      Serial.print("s d "); Serial.print("1");  Serial.print(" "); Serial.print(duty_cycle); Serial.print(" % "); Serial.print((micros() - t_stream)/pow(10,3),0); Serial.println(" ms");
    }
  
    //fill circular buffer
    if(cbuf.is_full()) cbuf.take();
    cbuf.put(my_data{prev_dc, Ylux});

    //performance metrics
    total_energy += ledP*prev_dc*time_step; 
    total_vis_sum += max(0, Rlux - Ylux); 
    
    float flicker = 0;
    if(n_samples == 1) Ylux_prev = Ylux;
    if(n_samples == 2){
      Ylux_prev_prev = Ylux_prev;
      Ylux_prev = Ylux;
    }
    if(n_samples >= 3){
      if((Ylux - Ylux_prev)*(Ylux_prev - Ylux_prev_prev) < 0){
        flicker = (abs(Ylux - Ylux_prev) + abs(Ylux_prev - Ylux_prev_prev))/(2*time_step);
      }
      Ylux_prev_prev = Ylux_prev;
      Ylux_prev = Ylux;      
    }
    total_flicker_sum += flicker;
  }
  
  
  //check for user commands, but only if buffer is empty
  short int count = 0;
  if(!printL_left && !printD_left){
    while(Serial.available() > 0){
        char inChar = Serial.read();
        count++;
        
        if(count == 10){
          Serial.println("Input too long!");
          //no need to flush (too slow) because next time the previous command is going to be invalid
          inFull = "";
          break;
        }
        
        //reached end of string
        if (inChar == '\n'){
          Serial.print("> "); Serial.println(inFull);

          //cut string in tokens
          short int i = 0;
          while (inFull.length() > 0){
            short int j = inFull.indexOf(' '); //find delimiter (space)
            if (j == -1){ //no space found
              tokens[i++] = inFull; //copy the rest of the string
              break;
            }
            else{
              tokens[i++] = inFull.substring(0, j); //cut string until delimiter
              inFull = inFull.substring(j+1); //save the rest of the string
            }
          }
            if(i == 3) answerCommand(tokens[0].c_str(), tokens[1].c_str(), tokens[2].c_str());
            else if(i == 1) answerCommand(tokens[0].c_str(), "", "");
            else Serial.println("err: incorrect number of arguments!");

            inFull = ""; //clear buffer
            break;
        }
        //not reached \n, keep adding the chars
        inFull += inChar; 
    }
  }

  //print Buffer
  if(printL_left){     
    for(int j = 0; j < 50 && printL_left > 0; j++, buf_ptr++, printL_left--){
      if(buf_ptr == buffer_size) buf_ptr = 0;
      cbuf[buf_ptr].print_l();
      if(printL_left != 1) Serial.print(",");
    }
    if(printL_left == 0) Serial.println();
  }
  else if(printD_left){     
    for(int j = 0; j < 50 && printD_left > 0; j++, buf_ptr++, printD_left--){
      if(buf_ptr == buffer_size) buf_ptr = 0;
      cbuf[buf_ptr].print_d();
      if(printD_left != 1) Serial.print(",");
    }
    if(printD_left == 0) Serial.println();
  }

}
