#include "type.h"
#include <iostream>
#include <iomanip>
using namespace std;



process ps[100];//外存存储进程的块
int num_of_ps=0;//外存上对待的进程
int index_of_ps=1;//index_of_ps是下个要放入ready队列的进程在ps数组中的序号
process_group* ready=new process_group();//存放ready状态的进程的链表
process *running=NULL;//获得cpu运行时间进程
process_group* running_group;//用process_group结构表示的获得cpu运行时间进程
process_group* finish=new process_group();//已经完成的进程的组成的链表
int t=0;//当前的运行时间
int remaining_process;//没有运行完成的进程的数目

int turn_around_time[100] = {0};//进程的周转时间
float  weighted_turn_around_time[100] = {0};//进程的带权周转时间
int finish_time[100] = {0};//进程的完成时间
float average_wt;//所有进程的平均等待时间
int sum_average_wt=0;//用于所有进程等待时间相加
float average_turn_around_time;//所有进程的平均周转时间
int sum_average_turn_around_time;//用于所有进程等待时间相加
int state_HRRN[100];//进程的带权周转时间
enum OS_scheduling_algorithms{FCFS=1,SJF,HRRN,RR,Preemptive_Priority_Algorithm}; //各种调度算法的枚举
int choice_algorithm;//表示你选择的调度算法的序号 对应上面的调度算法枚举
int quantum=1;//RR算法中进程分配的时间片
int in_quantum_gap;//在RR调度算法中使用

//初始化外存中等待的进程
void init(){
    ready->next = NULL;
    finish->next = NULL;
    cout << "Number of process in outside: ";
    cin >> num_of_ps;

    for(int i=1;i<=num_of_ps;++i){
        ps[i].id = i;
        ps[i].t_pcb = new pcb();
        ps[i].t_pcb->at = 10-i;
        ps[i].t_pcb->bt = i;
        ps[i].t_pcb->wt = 0;
        ps[i].t_pcb->rt = i;
        ps[i].t_pcb->hrrn_t = 1;
        ps[i].t_pcb->state = 1;
        ps[i].t_pcb->priority = 10-i;
    }
}

//初始化外存中等待的进程
void init_2(){
    ready->next = NULL;
    finish->next = NULL;
    cout << "Number of process in outside: ";
    cin >> num_of_ps;

    for(int i=1;i<=num_of_ps;++i){
        cout<<i<<" process is";
        cout<<endl;
        //cout<<"id: ";
        ps[i].id=i;
        //cout<<endl;
        ps[i].t_pcb = new pcb();
        cout<<"arrive time: ";
        cin>>ps[i].t_pcb->at;
        //cout<<endl;
        cout<<"burst time: ";
        cin>>ps[i].t_pcb->bt;
        //cout<<endl;
        ps[i].t_pcb->wt=0;     
        ps[i].t_pcb->rt=ps[i].t_pcb->bt;
        ps[i].t_pcb->hrrn_t=1;
        ps[i].t_pcb->state=1;
        cout<<"priority: ";
        cin>>ps[i].t_pcb->priority;
        cout<<endl<<endl;
    }
}

//把外存中的进程按到达ready队列的时间升序排列
void sort_ps(){
    int j;
    process tmp;
    for(int i=0;i<num_of_ps-1;++i){
        for(j=1;j<num_of_ps-i;++j){
            if(ps[j].t_pcb->at>ps[j+1].t_pcb->at){
                tmp = ps[j+1];
                ps[j+1] = ps[j];
                ps[j] = tmp;
            }
        }
    }
}

//显示外存中等待进程的pcb信息和进程id
void display_ps_inf(){
    cout<<left;
    cout<<setw(9)<<"id   ";
    cout<<setw(9)<<"at   ";
    cout<<setw(9)<<"bt   ";
    cout<<setw(9)<<"wt   ";
    cout<<setw(9)<<"rt   ";
    cout<<setw(9)<<"hrrt_t";
    cout<<setw(9)<<"state";
    cout<<setw(9)<<"priority";
    cout<<endl;
    for(int i=1;i<=num_of_ps;++i){
        cout<<setw(9)<<ps[i].id<<setw(9)<<ps[i].t_pcb->at \
        <<setw(9)<<ps[i].t_pcb->bt<<setw(9)<<ps[i].t_pcb->wt \
        <<setw(9)<<ps[i].t_pcb->rt<<setw(9)<<ps[i].t_pcb->hrrn_t<<setw(9)<<ps[i].t_pcb->state<<setw(9)<<ps[i].t_pcb->priority;
        cout<<endl;
    }
}

//显示一个process_group链表中的进程信息
void display_process_group(process_group *head){
    process_group* tmp;
    tmp = head;
    cout<<left;
    cout<<setw(9)<<"id   ";
    cout<<setw(9)<<"at   ";
    cout<<setw(9)<<"bt   ";
    cout<<setw(9)<<"wt   ";
    cout<<setw(9)<<"rt   ";
    cout<<setw(9)<<"hrrt_t";
    cout<<setw(9)<<"state";
    cout<<setw(9)<<"priority";
    cout<<setw(9)<<"ft";
    cout<<setw(9)<<"t_a_t";
    //cout<<setw(9)<<"w_t_a_t"; <<setw(9)<<weighted_turn_around_time[tmp->next->ps.id]
    cout<<endl;
    while(tmp->next != NULL){
        cout<<setw(9)<<tmp->next->ps.id<<setw(9)<<tmp->next->ps.t_pcb->at \
        <<setw(9)<<tmp->next->ps.t_pcb->bt<<setw(9)<<tmp->next->ps.t_pcb->wt \
        <<setw(9)<<tmp->next->ps.t_pcb->rt<<setw(9)<<tmp->next->ps.t_pcb->hrrn_t<<setw(9)<<tmp->next->ps.t_pcb->state<<setw(9)<<tmp->next->ps.t_pcb->priority \
        <<setw(9)<<finish_time[tmp->next->ps.id]<<setw(9)<<turn_around_time[tmp->next->ps.id];
        cout<<endl;
        tmp = tmp->next;
    }
    cout<<endl;
    cout << "Average wait time is " << average_wt;
    cout<<endl;
    cout<< "Average trun around time is " << average_turn_around_time;
    cout<<endl;
}

//把一个进程放入process_group head链表的队尾
void put_process_into_tail(process_group *head,process t_ps){
    process_group *tmp=head;
    while(tmp->next != NULL)
        tmp = tmp->next;
    process_group *p = new process_group();
    p->ps = t_ps;
    p->next = NULL;
    tmp->next = p;
}

//把一个process_group 放入 head链表的队尾
void put_process_group_into_tail(process_group *head,process_group *t_psg){
    process_group *tmp=head;
    while (tmp->next != NULL) {
        tmp = tmp->next;
        //cout << 1 << endl;
    }
    tmp->next = t_psg;
    //t_psg->next = NULL;
}

//从外存中取进程放入ready队列
void put_into_ready(){
    while(index_of_ps<=num_of_ps && t>=ps[index_of_ps].t_pcb->at){
        if(choice_algorithm != Preemptive_Priority_Algorithm){
            put_process_into_tail(ready,ps[index_of_ps]);            
        }
        else{
            //使用Preemptive_Priority_Algorithm()算法时的情况
            if(running != NULL && ps[index_of_ps].t_pcb->priority > running->t_pcb->priority){
                running_group->next = ready->next;
                ready->next = running_group;
                process_group* p = new process_group();
                p->ps = ps[index_of_ps];
                p->next = NULL;
                running_group = p;
                running = &(running_group->ps);
            }
            else if (running == NULL) {
                process_group* p = new process_group();
                p->ps = ps[index_of_ps];
                p->next = NULL;
                running_group = p;
                running = &(running_group->ps);
            }
            else {
                put_process_into_tail(ready, ps[index_of_ps]);
            }
        }
        ++index_of_ps;
    }
}

//从ready链表中拿出第一个进程1
process_group* get_process_from_ready(){
    process_group* tmp = ready->next;
    if(tmp==NULL) return NULL;
    ready->next = tmp->next;
    tmp->next = NULL;
    return tmp;
}

//把ready链表中的第一个进程拿出进入运行状态
void put_into_running(){
    running_group = get_process_from_ready();
    if(running_group==NULL) {
        running = NULL;
        return;
    }
    //running = new process();
    running = &(running_group->ps);
}

//把运行中的进程重新放回reay链表
void putback_into_ready(){

    put_process_group_into_tail(ready,running_group);
    running = NULL;

}

//运行完成的进程放入finish链表
void put_into_finish(process_group *psg){
    put_process_group_into_tail(finish,psg);
}

//dispatcher类里面包含了各种调度算法
class dispatcher{
    process_group ready_sort_group[100];
    int num_of_sort_group;
    public: 
        void sort_SJF();
        void sort_HRRN();
        void sort_Preemptive_Priority_Algorithm();
    public:
        void FCFS();
        void SJF();
        void HRRN();
        void RR();
        void Preemptive_Priority_Algorithm();
};

void dispatcher::FCFS(){}
void dispatcher::sort_SJF(){
    process_group tmp;
    int k;
    int j;
    num_of_sort_group=0;
    process_group tmp_2=*ready;

    while(tmp_2.next != NULL){
        ready_sort_group[num_of_sort_group++] = *(tmp_2.next); 
        tmp_2 = *(tmp_2.next);
    }

    if(num_of_sort_group == 0 || num_of_sort_group==1) return;

    for(int i=0;i<num_of_sort_group-1;++i){
        //tmp = ready_sort_group[i];
        k = i;
        for(j=i+1;j<num_of_sort_group;++j){
            if(ready_sort_group[k].ps.t_pcb->bt > ready_sort_group[j].ps.t_pcb->bt){
                k=j;
            }
        }
        if(k != i){
            tmp = ready_sort_group[i];
            ready_sort_group[i] = ready_sort_group[k];
            ready_sort_group[k] = tmp;
        }
    }

    process_group* tmp_3 = ready->next;
    for (int i = 0; i < num_of_sort_group; ++i) {
        tmp_3->ps = ready_sort_group[i].ps;
        tmp_3 = tmp_3->next;
    }
}
void dispatcher::SJF(){
    sort_SJF();
}
void dispatcher::sort_HRRN(){
    process_group tmp;
    int k;
    int j;
    num_of_sort_group=0;
    process_group tmp_2=*ready;

    while(tmp_2.next != NULL){
        ready_sort_group[num_of_sort_group++] = *(tmp_2.next); 
        tmp_2 = *(tmp_2.next);
    }

    if(num_of_sort_group == 0 || num_of_sort_group==1) return;

    for(int i=0;i<num_of_sort_group-1;++i){
        //tmp = ready_sort_group[i];
        k = i;
        for(j=i+1;j<num_of_sort_group;++j){
            if(ready_sort_group[k].ps.t_pcb->hrrn_t < ready_sort_group[j].ps.t_pcb->hrrn_t){
                k=j;
            }
        }
        if(k != i){
            tmp = ready_sort_group[i];
            ready_sort_group[i] = ready_sort_group[k];
            ready_sort_group[k] = tmp;
        }
    }

    process_group* tmp_3 = ready->next;
    for (int i = 0; i < num_of_sort_group; ++i) {
        tmp_3->ps = ready_sort_group[i].ps;
        tmp_3 = tmp_3->next;
    }
}
void dispatcher::HRRN(){
    sort_HRRN();
}
void dispatcher::RR(){

    putback_into_ready();

}
void dispatcher::sort_Preemptive_Priority_Algorithm(){
    process_group tmp;
    int k;
    int j;
    num_of_sort_group=0;
    process_group tmp_2=*ready;

    while(tmp_2.next != NULL){
        ready_sort_group[num_of_sort_group++] = *(tmp_2.next); 
        tmp_2 = *(tmp_2.next);
    }

    if(num_of_sort_group == 0 || num_of_sort_group==1) return;

    for(int i=0;i<num_of_sort_group-1;++i){
        //tmp = ready_sort_group[i];
        k = i;
        for(j=i+1;j<num_of_sort_group;++j){
            if(ready_sort_group[k].ps.t_pcb->priority < ready_sort_group[j].ps.t_pcb->priority){
                k=j;
            }
        }
        if(k != i){
            tmp = ready_sort_group[i];
            ready_sort_group[i] = ready_sort_group[k];
            ready_sort_group[k] = tmp;
        }
    }

    process_group* tmp_3 = ready->next;
    for (int i = 0; i < num_of_sort_group; ++i) {
        tmp_3->ps = ready_sort_group[i].ps;
        tmp_3 = tmp_3->next;
    }
}
void dispatcher::Preemptive_Priority_Algorithm(){

    sort_Preemptive_Priority_Algorithm();

}

//manage_ps_inf类负责在运行时间内更新进程的信息 如等待时间 剩余运行时间等等
class manage_ps_inf{
    public:
        void inf_fresh();
        void cal_end_inf();
};

void manage_ps_inf::inf_fresh(){
    process_group *tmp;
    tmp = ready;
    while(tmp->next != NULL){
        //cout <<"id: " << tmp->next->ps.id << endl;
        tmp->next->ps.t_pcb->wt += 1;
        tmp->next->ps.t_pcb->hrrn_t = (float)(tmp->next->ps.t_pcb->wt+tmp->next->ps.t_pcb->bt)/tmp->next->ps.t_pcb->bt;
        //tmp->next->ps.t_pcb->rt -= 1;
        tmp = tmp->next;
    }
    if(running != NULL){
        running->t_pcb->rt -= 1;
        running->t_pcb->hrrn_t = (float)(running->t_pcb->wt+running->t_pcb->bt)/running->t_pcb->bt;
    }
        
}
void manage_ps_inf::cal_end_inf(){
    finish_time[running->id] = t;
    turn_around_time[running->id] = t-running->t_pcb->at;
    sum_average_turn_around_time += turn_around_time[running->id];
    sum_average_wt += running->t_pcb->wt;
    weighted_turn_around_time[running->id] = (float)turn_around_time[running->id]/running->t_pcb->bt;
}

int main()
{
    //选择你需要的调度算法
    cout<<"1 represent FCFS algorithm";
    cout<<endl;
    cout<<"2 represent SJF algorithm";
    cout<<endl;
    cout<<"3 represent HRRN algorithm";
    cout<<endl;
    cout<<"4 represent RR algorithm";
    cout<<endl;
    cout<<"5 represent Preemptive_Priority_Algorithm algorithm";
    cout<<endl;
    cout<<"Choice your os scheduling algorithm: ";

    while(1){
        cin>>choice_algorithm;
        if(choice_algorithm == 1 || choice_algorithm == 2 || choice_algorithm == 3 || choice_algorithm == 4 || choice_algorithm == 5)
            break;
        cout<<endl;
        cout<<"value of choice_algorithm not in 1 or 2 or 3 !!!";
        cout<<endl;
        cout<<"input again your value: ";
    }

    if(choice_algorithm == 4){
        //cout<<endl;
        cout<<"Your quantum is: ";
        cin>>quantum;
    }
    
    //init_2();
    init();//初始化外存ps数组上的进程
    sort_ps();//按进入ready队列的时间既at 排序外存上的进程
    display_ps_inf();//显示外存ps上的进程信息
    cout<<endl<<endl;

    remaining_process = num_of_ps;//剩余要执行的进程数目
    manage_ps_inf mpi;//管理进程信息的类
    dispatcher dpc;//含有各种调度算法的类

    while(remaining_process > 0){ //还有剩余要执行的进程
        //cout << "rp: " << remaining_process << endl;
        if(index_of_ps <= num_of_ps){
            put_into_ready();//从外存中拿进程入ready链表
        }
        if(running == NULL){ //没有进程运行时从ready链表中按照你选择的调度算法拿进程入running中运行
            switch (choice_algorithm){
                case FCFS:
                case RR:
                    break;
                case SJF:
                    dpc.SJF();
                    break;
                case HRRN:
                    dpc.HRRN();
                    break;
                case Preemptive_Priority_Algorithm:
                    dpc.Preemptive_Priority_Algorithm();
                    break;
            }

            put_into_running();//放入running中
        }
        t += 1;//时间过去了一个单位
        //cout << "t: " << t << endl;
        mpi.inf_fresh();//时间过去了一个单位更新进程信息
        if(running != NULL && running->t_pcb->rt <= 0){ //进程运行完成放入finish队列
            put_into_finish(running_group);
            mpi.cal_end_inf();//计算完成时间等时间量
            //delete running;
            running = NULL;//此时没有进程运行了
            
            //cout << "had run" << endl;
            --remaining_process;//需要完成的进程数目减少一个
        }
        
        if(choice_algorithm == RR){//算法时RR时的判断进程的时间片quantum用完了没
            in_quantum_gap = t%quantum;
            if(running != NULL && in_quantum_gap==0){
                //cout<<"id: "<<running->id;
                //cout<<endl;
                dpc.RR();
            }
        }
        
    }
    
    average_wt = (float)sum_average_wt/num_of_ps;//计算平均等待时间
    average_turn_around_time = (float)sum_average_turn_around_time/num_of_ps;//计算平均周转时间

    cout << endl << endl;
    display_process_group(finish);
    return 0;
}




