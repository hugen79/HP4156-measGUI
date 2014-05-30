int _setSweepMode(int ud);
int _setSamplingMode(int ud);

int setSMU(int ud, const char *vars[]);
int disableSMU(int ud, const char *var);
int setSweepVAR(int ud, const char *vars[]);
int setUserFunction(int ud, const char* data[]);

int addToList(int ud, char* variable);
int remFromList(int ud, char* variable);
int remFromListAll(int ud);

int savedata(int ud, char* filename, node_t *listVARS, int buffersize);


