int setSMU(int ud, const char *vars[]);
int setVAR(int ud, const char *vars[]);
int disableSMU(int ud, const char *var);
int addToList(int ud, char* variable);
int remFromList(int ud, char* variable);
int remFromListAll(int ud);
int savedata(int ud, char* filename, node_t *listVARS, int buffersize);
