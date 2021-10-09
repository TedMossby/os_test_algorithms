typedef int process_id;

typedef struct{
    int at;
    int bt;
    int wt;
    int rt;
    float hrrn_t;
    int state;
    int priority;
}pcb;

typedef struct{
    process_id id;
    pcb *t_pcb;
}process;

typedef struct tmp{
    process ps;
    struct tmp *next;
}process_group;




