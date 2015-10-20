
// DEFAULTS 
int setDefaults(int ud);

// OSCILLATOR CONTROL  
int setFrequency(int ud, const char *var);
int setAmplitude(int ud, const char *var);

// MEASUREMENT CONTROL
int setMode(int ud, const char *var);
int calOpen(int ud); 
int calShort(int ud);

// BIAS CONTROL 
int setBias(int ud, const char *var);
int setBiasON(int ud);
int setBiasOFF(int ud);

// SAVE DATA 
char** measure(int ud, double* swp, int len, char** freq, int lenf, int buffersize);
int savedata(int ud, char* filename, char* hdata, char* oscdata, char** DATA, int len, int lenf);
