// MODE SELECTION 
int _setSweepMode(int ud);
int _setSamplingMode(int ud);

// SWEEP MODE VARS 
int setSweepSMU(int ud, const char *vars[]);
int setSweepVAR(int ud, const char *vars[]);

// SAMPLING MODE VARS 
int setSamplingSMU(int ud, const char *vars[]);
int setSamplingVAR(int ud, const char *vars[]);

// DISABLE ALL 
int disableSMU(int ud, const char *var);

// USER FUNCTION 
int setUserFunction(int ud, const char* data[]);

// LIST CONTROL 
int addToList(int ud, char* variable);
int remFromList(int ud, char* variable);
int remFromListAll(int ud);

// SAVE DATA 
int savedata(int ud, char* filename, node_t *listVARS, int buffersize);


