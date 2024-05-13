#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <climits> 
#include <queue>
#include <iomanip>

using namespace std;

struct Process {
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int start_time;
    int finish_time;
    int waiting_time;
    int turnaround_time;
};

void RoundRobin_algorithm(vector<Process>& processes, int quantum, int num_processes) {
    int current_time = 0;
    int total_waiting_time = 0, total_turnaround_time = 0, total_burst_time = 0;
    queue<int> ready_queue;
    vector<pair<int, int>> gantt_chart;

    for (auto& process : processes) {
        process.remaining_time = process.burst_time;
    }

    cout << "Process\tStart Time\tFinish Time\tWaiting Time\tTurnaround Time\n";
    while (true) {
        bool done = true;
        bool idle = true;
        for (int i = 0; i < num_processes ; ++i) {
            if (processes[i].remaining_time > 0) {
                done = false;
                if (processes[i].arrival_time <= current_time) {
                    idle = false;
                    int execution_time = min(quantum, processes[i].remaining_time);
                    processes[i].remaining_time -= execution_time;
                    current_time += execution_time;

                    if (processes[i].start_time == -1) {
                        processes[i].start_time = current_time - execution_time;
                    }

                    gantt_chart.push_back({processes[i].id, execution_time});

                    if (processes[i].remaining_time == 0) {
                        processes[i].finish_time = current_time;
                        processes[i].turnaround_time = processes[i].finish_time - processes[i].arrival_time;
                        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                        total_waiting_time += processes[i].waiting_time;
                        total_turnaround_time += processes[i].turnaround_time;
                        total_burst_time += processes[i].burst_time;
                    } else {
                        ready_queue.push(i);
                    }
                }
            }
        }

        if (done) {
            break;
        }

        if (idle) {
            gantt_chart.push_back({0, 1}); 
            current_time++;
        }

        if (!ready_queue.empty()) {
            int idx = ready_queue.front();
            ready_queue.pop();
            ready_queue.push(idx);
        } else {
            current_time++;
        }
    }

    for (auto& process : processes) {
        cout << process.id << "\t" << process.start_time << "\t\t"  << process.finish_time 
        << "\t\t" << process.waiting_time <<"\t\t" << process.turnaround_time<< endl;
    }

    double avg_waiting_time = (double)total_waiting_time / num_processes;
    double avg_turnaround_time = (double)total_turnaround_time / num_processes;
    double cpu_utilization = (double)total_burst_time / current_time;

    cout << "Average Waiting Time: " << avg_waiting_time << endl;
    cout << "Average Turnaround Time: " << avg_turnaround_time << endl;
    cout << "CPU Utilization: " << cpu_utilization * 100 << "%" << endl;

    cout << "Gantt Chart:" << endl;
    for (auto& entry : gantt_chart) {
        if (entry.first == 0) {
            cout << "| Idle ";
        } else {
            cout << "| P" << entry.first << " ";
        }
    }
    cout << "|" << endl;
    cout << endl << "more details : " << endl;
    int temp_time = 0;
    for (auto& entry : gantt_chart) {
        if (entry.first == 0) {
            cout << "| Idle ";
        } else {
            cout << "| P" << entry.first << " ";
        }
        cout << " " << setw(2) << temp_time << "-" << setw(2) << temp_time + entry.second << " ";
        temp_time += entry.second;
    }
    cout << "|" << endl;
}



bool compareArrivalTime(Process p1, Process p2) {
    return p1.arrival_time < p2.arrival_time;
}

bool compareRemainingTime(Process p1, Process p2) {
    return p1.remaining_time < p2.remaining_time;
}

void SRT_algorithm(vector<Process>& processes , int context_switch_time , int num_processes) {
    sort(processes.begin(), processes.end(), compareArrivalTime);

    int remaining_time = 0;
    int current_time = 0;
    int completed = 0;
    int total_waiting_time = 0 , total_turnaround_time = 0 , total_burst_time = 0;

    vector<pair<int, int>> gantt_chart;

    for (auto& process : processes) {
        remaining_time += process.burst_time;
    }

    while (completed < num_processes) {
        int idx = -1;
        int min_remaining = INT_MAX; 

        for (int i = 0; i < num_processes ; ++i) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time < min_remaining && processes[i].remaining_time > 0) {
                idx = i;
                min_remaining = processes[i].remaining_time;
            }
        }

        if (idx == -1) {
            current_time++;
            gantt_chart.push_back(make_pair(current_time, -1)); 
        } else {
            if (gantt_chart.empty() || gantt_chart.back().second != processes[idx].id) {
                gantt_chart.push_back(make_pair(current_time, processes[idx].id));
                processes[idx].start_time = current_time;
            }

            processes[idx].remaining_time--;
            current_time++;

            if (processes[idx].remaining_time == 0) {
                completed++;
                processes[idx].finish_time = current_time;
                processes[idx].turnaround_time = processes[idx].finish_time - processes[idx].arrival_time;
                processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
                total_burst_time += processes[idx].burst_time;
                total_waiting_time += processes[idx].waiting_time;
                total_turnaround_time += processes[idx].turnaround_time;

                gantt_chart.push_back(make_pair(current_time, processes[idx].id));
            }
        }
    }

    double avg_waiting_time = double(total_waiting_time) / num_processes;
    double avg_turnaround_time = double(total_turnaround_time) / num_processes;
    double cpu_utilization = (double)total_burst_time / current_time;

    cout << "Process\tStart Time\tFinish Time\tWaiting Time\tTurnaround Time" << endl;
    for (auto& process : processes) {
        cout << process.id << "\t" << process.start_time << "\t\t" << process.finish_time << "\t\t" << process.waiting_time << "\t\t" << process.turnaround_time << endl;
    }
    cout << "Average Waiting Time: " << avg_waiting_time << endl;
    cout << "Average Turnaround Time: " << avg_turnaround_time << endl;
    cout << "CPU Utilization: " << cpu_utilization*100 << "%" << endl;

    cout << "Gantt Chart:" << endl;
    for (size_t i = 0; i < gantt_chart.size(); ++i) {
        if (i == 0 || gantt_chart[i].second != gantt_chart[i - 1].second) {
            if (gantt_chart[i].second == -1) {
                cout << "| Idle ";
            } else {
                cout << "| P" << gantt_chart[i].second << " " ;
            }
        }
    }
    cout << "|" << endl;
    cout << endl << "more details : " << endl;

    int temp_time = 0;
    for (size_t i = 0; i < gantt_chart.size(); ++i) {
        if (i == 0 || gantt_chart[i].second != gantt_chart[i - 1].second) {
            if (gantt_chart[i].second == -1) {
                cout << "| Idle " ;
                cout << setw(2) << temp_time << "-";
                
            } else {
                cout << "| P" << gantt_chart[i].second << " ";
                cout << setw(2) << gantt_chart[i].first << "-";
                
            }
            temp_time = gantt_chart[i+1].first;
        }
        if (i + 1 == gantt_chart.size() || gantt_chart[i].second != gantt_chart[i + 1].second) {
            if(i + 1 == gantt_chart.size()){
                cout << setw(2) << gantt_chart[i].first << " ";
            }
            else{
            cout << setw(2) << gantt_chart[i+1].first << " ";
            }

        }
    }
    cout << "|" << endl;
    cout << endl;

}

void FCFS_algorithm(vector<Process>& processes, int num_processes) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    int total_waiting_time = 0, total_turnaround_time = 0, total_burst_time = 0;
    int current_time = 0;
    cout << "PID\tStart Time\tFinish Time\tWaiting Time\tTurnaround Time\n";

    for (int i = 0; i < num_processes; i++) {
        if (current_time < processes[i].arrival_time) {
            total_waiting_time += processes[i].arrival_time - current_time;
            current_time = processes[i].arrival_time;
        }
        processes[i].start_time = current_time;
        processes[i].finish_time = processes[i].start_time + processes[i].burst_time;
        processes[i].waiting_time = processes[i].start_time - processes[i].arrival_time;
        processes[i].turnaround_time = processes[i].finish_time - processes[i].arrival_time; 
        total_burst_time += processes[i].burst_time;
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;

        cout << processes[i].id << "\t" << processes[i].start_time << "\t\t" << processes[i].finish_time 
            << "\t\t" << processes[i].waiting_time << "\t\t" << processes[i].turnaround_time << endl;
        current_time = processes[i].finish_time; 
    }

    float avg_waiting_time = (float)total_waiting_time / num_processes;
    float avg_turnaround_time = (float)total_turnaround_time / num_processes; 
    double cpu_utilization = (float)total_burst_time / current_time;

    cout << "Average waiting time: " << avg_waiting_time << endl;
    cout << "Average turnaround time: " << avg_turnaround_time << endl;
    cout << "CPU Utilization: " << cpu_utilization * 100 << "%" << endl;

    cout << "Gantt Chart:" << endl;
    int previous_finish_time = 0;
    for (int i = 0; i < num_processes; i++) {
        if (previous_finish_time < processes[i].arrival_time) {
            cout << "| Idle ";
        }
        cout << "| P" << processes[i].id << " ";
        previous_finish_time = processes[i].finish_time;
    }
    cout << "|" <<endl;
    cout << endl << " more details : " <<endl ;
    previous_finish_time = 0;
    for (int i = 0; i < num_processes; i++) {
        if (previous_finish_time < processes[i].arrival_time) {
            cout << "| Idle " << previous_finish_time << "-" << processes[i].start_time;
        }
        cout << "| P" << processes[i].id << " " ;
        cout << processes[i].start_time << "-" << processes[i].finish_time;
        previous_finish_time = processes[i].finish_time;
    }
    cout << "|" << endl;
}




int main() {
    vector<Process> processes;
    string filename = "input.txt";
    // string filename = "input2.txt";

    ifstream file(filename);

    int id, arrival_time, burst_time;
    int num_processes,context_switch_time,quantum;
    file >> context_switch_time >> quantum;
    while (file >> id >> arrival_time >> burst_time) {
        processes.push_back({id, arrival_time, burst_time, burst_time, -1, -1, 0, 0});
    }
    num_processes = processes.size();

    file.close();
    vector<Process>& fcfs = processes;
    vector<Process>& srt = processes;
    vector<Process>& rr = processes;

    cout << "First come first serve algorithm" << endl;
    FCFS_algorithm(fcfs,num_processes);
    cout << endl <<"---------------------------------------------" << endl;
    cout << "Shortest remaining time algorithm with context switch = "<< context_switch_time << " milliseconds" << endl;
    SRT_algorithm(srt,context_switch_time,num_processes);
    cout << endl <<"---------------------------------------------" << endl;
    cout<<"Round Ronin algorithm with quantum = "<< quantum << endl;
    RoundRobin_algorithm(rr,quantum,num_processes);
 

    return 0;
}
