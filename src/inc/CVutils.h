
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
<<<<<<< HEAD
char** measure(int ud, double* swp, int len, char** freq, int lenf, int buffersize);
int savedata(int ud, char* filename, char* hdata, char* oscdata, char** DATA, int len, int lenf);
=======
char** measure(int ud, double* swp, int len, int buffersize);
int savedata(int ud, char* filename, char* hdata, char* oscdata, char** DATA, int len);
>>>>>>> 53b06224f894b3c0fe780e7e190cf754f9cb5fd3
