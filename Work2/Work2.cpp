#include <iostream>
#include <queue>
#include <fstream>

using namespace std;

/***计算机科学与技术1901 陈润尧
本代码以数值越大优先权越高为规则，每个进程经过处理机调度，每运行一个单位时间，优先级按输入数量级减少
对于等待队列中的元素，优先权越高，任务越紧急，排名越靠前。
对于相同优先权的，到达等待队列的时间越早，优先权越高***/

//排序Mode


struct Progress
{
    string Name;        //进程名
    int ArriveTime;     //到达时间
    int ServiceTime;    //服务时间
    int InitPriority;   //初始优先权
    string Status;      //状态

    int ArriveQueue;    //到达等待队列的时间
    int RemainTime;     //剩余服务时间
    int FinishTime;     //完成时间

    int AroundTime;     //周转时间
    double AveAroundTime;  //平均周转时间

}pro[1010];

template <typename T>
class cmp
{
public:
    bool operator()(T a, T b)
    {
        if (a.InitPriority == b.InitPriority) return  a.ArriveQueue > b.ArriveQueue;
        else return a.InitPriority < b.InitPriority;
    }
};

//等待队列
priority_queue< struct Progress, vector<struct Progress>, cmp<struct Progress> > wait_que;

///初始化进程
int InitProgress();
///排序函数
void SortProgresses(int n);
///调度函数
void RunProgress(int n, int weight);
//计算周转时间和带权周转时间
void CalcAroundTime(int n);
//显示全部信息
void ShowDetail(int n);
//更新函数
void UpdatePro(int n, struct Progress P);

int main()
{
    int function;
    int n, weight = 0;
    cout << "请选择执行方式：（1）文件读入 （2）手动输入" << endl;
    cin >> function;
    if (function == 1)
    {
        int i = 0;
        ifstream data("t.txt", ios::in);
        /*data.open("t.txt");*/
        if (!data.is_open())
        {
            cout << "文件打开失败！" << endl;
            return 0;
        }

        data >> n;
        while (!data.eof())
        {
            data >> pro[i].Name >> pro[i].ArriveTime >> pro[i].ServiceTime >> pro[i].InitPriority;
            pro[i].RemainTime = pro[i].ServiceTime;
            i++;
            if (i == n) data >> weight;
        }
        cout << "读取数据如下：" << endl;
        cout << "进程数" << n << endl;
        cout << "输入每次运行，程序权重的减少数" << weight << endl;
        ShowDetail(n);
    }
    else
    {
        n = InitProgress();
        cout << "请输入每次运行，程序权重的减少数" << endl;
        cin >> weight;
    }
    RunProgress(n, weight);
    ShowDetail(n);
    return 0;
}

int InitProgress()
{
    int n;

    cout << "请输入进程数:" << endl;
    cin >> n;

    cout << "请输入进程的名字、进程的到达时间、服务时间、初始优先权" << endl;
    for (int i = 0; i < n; i++) {
        cin >> pro[i].Name >> pro[i].ArriveTime >> pro[i].ServiceTime >> pro[i].InitPriority;
        pro[i].RemainTime = pro[i].ServiceTime;
    }

    return n;
}

void SortProgresses(int n)
{

    int m = n;
    struct Progress t;

    for (int i = 0; i < n; i++) {
        m--;
        for (int j = 0; j < m; j++) {
            if (pro[j].InitPriority > pro[j + 1].InitPriority) {
                t = pro[j];
                pro[j] = pro[j + 1];
                pro[j + 1] = t;
            }
            else if (pro[j].InitPriority == pro[j + 1].InitPriority) {
                if (pro[j].ArriveTime > pro[j + 1].ArriveTime) {
                    t = pro[j];
                    pro[j] = pro[j + 1];
                    pro[j + 1] = t;
                }
            }
        }
    }
}

void RunProgress(int n, int weight)
{
    int cnt = 0;
    for (int time = 0;; time++)
    {
        //每过一个时间单位，都扫描一遍，将time时刻到达的进程放入等待队列
        for (int i = 0; i < n; i++)
        {
            if (pro[i].ArriveTime <= time)
            {
                if (pro[i].Status == "")
                {
                    pro[i].ArriveQueue = time;
                    pro[i].Status = "W";    //到达等待队列的进程进入等待状态
                    wait_que.push(pro[i]);
                }
            }
        }

        //如果等待队列不为空，将队首取出，运行一个时间单位，并更改其优先权
        if (!wait_que.empty())
        {
            struct Progress P = wait_que.top();
            wait_que.pop();

            P.RemainTime--;

            //当前进程执行完毕
            if (P.RemainTime == 0)
            {
                cnt++;                      //执行完的进程数加一
                P.FinishTime = time + 1;    //记录执行结束时间
                P.Status = "F";             //状态设为结束
                UpdatePro(n, P);
                continue;
            }
            else
            {
                P.InitPriority -= weight;   //当前进程的权重减少
                P.ArriveQueue = time + 1;   //再次进入等待队列的时间
                P.Status = "W";             //状态改为等待
                wait_que.push(P);           //再次进入等待队列，并自动排序
            }
        }

        //如果等待队列为空，并且都已执行完毕，说明任务结束了
        if (wait_que.empty() && cnt == n)
        {
            CalcAroundTime(n);              //计算周转时间
            break;
        }

        //struct Progress P = wait_que.top();
        //cout << P.Name;
    }
}

void CalcAroundTime(int n)
{
    for (int i = 0; i < n; i++)
    {
        pro[i].AroundTime = pro[i].FinishTime - pro[i].ArriveTime;              //计算周转时间
        pro[i].AveAroundTime = (1.0) * pro[i].AroundTime / pro[i].ServiceTime;  //平均周转时间
    }
}

void ShowDetail(int n)
{
    cout << "进程运行结果" << endl;
    cout << "进程名\t" << "到达时间\t" <<"服务时间\t" << "结束时间\t" << "周转时间\t" << "平均周转时间\t" << endl;
    for (int i = 0; i < n; i++)
    {
        cout << pro[i].Name << "\t\t" << pro[i].ArriveTime << "\t\t" << pro[i].ServiceTime<<"\t\t" << pro[i].FinishTime << "\t\t" << pro[i].AroundTime << "\t\t" << pro[i].AveAroundTime << "\t\t" << endl;
    }
}

void UpdatePro(int n, struct Progress P)
{
    for (int i = 0; i < n; i++)
    {
        if (pro[i].Name == P.Name)
        {
            pro[i].FinishTime = P.FinishTime;
            pro[i].Status = P.Status;
        }
    }
}