#include<bits/stdc++.h>

using namespace std;

using ll = long long;
using ld = long double;
#define pb push_back
// #define endl "\n"

const ll SEED = 27102001;
const ll INF = 1e9 + 7;
const int bound = 1001;
ll start_time;

//random SEED
mt19937 rng(SEED);

//h:m:s to second
int time_to_sec(string time){
	int h = (time[0] - '0') * 10 + (time[1] - '0');
	int m = (time[3] - '0') * 10 + (time[4] - '0');
	int s = (time[6] - '0') * 10 + (time[7] - '0');
	return h*3600 + m*60 + s;
}

//second to h:m:s
string sec_to_time(int sec){
	string ans = "";
	int x;
	x = sec / 3600;
	sec -= 3600 * x;
	//hours
	ans += char(x / 10 + '0');
	ans += char(x % 10 + '0');
	ans += ':';
	x = sec / 60;
	sec -= 60 * x;
	//minutes
	ans += char(x / 10 + '0');
	ans += char(x % 10 + '0');
	ans += ':';
	x = sec;
	//second
	ans += char(x / 10 + '0');
	ans += char(x % 10 + '0');
	return ans;
}


// Distance_matrix;
int nV;
double distance_matrix[bound][bound];


//class Package
int N;
int Packages_Sender_ID[bound], Packages_Receiver_ID[bound];
ld Packages_mass[bound], Packages_vol[bound];
// Sender Process
int Packages_Sender_s[bound], Packages_Sender_e[bound], Packages_load[bound];
// Receiver Process
int Packages_Receiver_s[bound], Packages_Receiver_e[bound], Packages_unload[bound];
bool Serves[bound];

//Process is Sender or Receiver base on boolean is_Sen
struct Process {
	int pID;
	int ID;
	int timeDone = 0;
	bool is_Sen;
};


// class Truck
int K;
int Truck_indexes[bound];
int Trucks_hub[bound];
int Trucks_s[bound], Trucks_e[bound];
ld Trucks_mass[bound], Trucks_vol[bound], Trucks_vel[bound];
// sol_Routes of Truck
vector<Process> sol_Routes[bound];

// Solution  struct to decode
// Step is each ation of the Truck do in that Node
struct Step {
	int pID;
	string timeIn;
};	
// Node represent all the action that the Truck do in that Node ID
struct Node {
	int ID;
	vector<Step> processes;
	string timeIn;
	string timeOut;
};
// end Solution struct

// all Truck methods:
int travel_Time(int start, int end, int Truck_ID) {
	return ceil(( 3600 * double(distance_matrix[start][end]) / Trucks_vel[Truck_ID] ));
}

void Truck_calc_runtime(int Truck_ID) {
	int current_Time = Trucks_s[Truck_ID];

	for(int i = 0; i < sol_Routes[Truck_ID].size(); i++) {
		int Package_ID = sol_Routes[Truck_ID][i].pID;

		if (i == 0){
			current_Time += travel_Time(Trucks_hub[Truck_ID], sol_Routes[Truck_ID][i].ID, Truck_ID);
		}
		else{
			current_Time += travel_Time(sol_Routes[Truck_ID][i-1].ID, sol_Routes[Truck_ID][i].ID, Truck_ID);
		}

		//Sender
		if(sol_Routes[Truck_ID][i].is_Sen) {
			if (current_Time < Packages_Sender_s[Package_ID]){
				current_Time = Packages_Sender_s[Package_ID];
			}
			current_Time += Packages_load[Package_ID];

		} 
		//Receiver
		else {
			if (current_Time < Packages_Receiver_s[Package_ID]){
				current_Time = Packages_Receiver_s[Package_ID];
			}
			current_Time += Packages_unload[Package_ID];
		}
		sol_Routes[Truck_ID][i].timeDone = current_Time;
	}
}

int Truck_Insert(int Package_ID, int Truck_ID, bool is_Insert) {

	int best_Sender_index, best_Receiver_index;

	ld current_mass = 0, current_vol = 0;

	int best_time = INF;

	for(int i = 0; i <= sol_Routes[Truck_ID].size(); i++) {
		int current_Time;

		// if we add sender to index, do we sastisfied time constraint
		if (i==0) {
			current_Time = Trucks_s[Truck_ID] + travel_Time(Trucks_hub[Truck_ID], Packages_Sender_ID[Package_ID], Truck_ID);

		}
		else {
			current_Time = sol_Routes[Truck_ID][i-1].timeDone + travel_Time(sol_Routes[Truck_ID][i-1].ID, Packages_Sender_ID[Package_ID], Truck_ID);
		} 

		if (current_Time < Packages_Sender_s[Package_ID]){
			current_Time = Packages_Sender_s[Package_ID];
		}

		// if current_time > timewindow of Package
		if(current_Time > Packages_Sender_e[Package_ID]){
			break;
		}

		current_Time += Packages_load[Package_ID];

		// temporary mass and vol after insert sender to index i
		ld temp_mass = current_mass + Packages_mass[Package_ID];
		ld temp_vol = current_vol + Packages_vol[Package_ID];
		
		// if sastisfied mass and vol constraint
		if(temp_mass <= Trucks_mass[Truck_ID] && temp_vol <= Trucks_vol[Truck_ID]) {
			
			int prev_ID = Packages_Sender_ID[Package_ID];

			// insert receiver to index j
			for(int j = i; j <= sol_Routes[Truck_ID].size(); j++) {
				
				// calc runtime, temp_mass, temp_vol
				if(j > i) {
					int current_pID = sol_Routes[Truck_ID][j - 1].pID;

					if(sol_Routes[Truck_ID][j - 1].is_Sen) {
						current_Time += travel_Time(prev_ID, Packages_Sender_ID[current_pID], Truck_ID);
						if(current_Time > Packages_Sender_e[current_pID]){
							break;
						}

						if (current_Time < Packages_Sender_s[current_pID]){
							current_Time = Packages_Sender_s[current_pID];
						}
						current_Time += Packages_load[current_pID];

						temp_mass += Packages_mass[current_pID];
						temp_vol += Packages_vol[current_pID];
						if(temp_mass > Trucks_mass[Truck_ID] || temp_vol > Trucks_vol[Truck_ID]){
							break;
						}

						prev_ID = Packages_Sender_ID[current_pID];
					} 
					else {
						current_Time += travel_Time(prev_ID, Packages_Receiver_ID[current_pID], Truck_ID);
						if(current_Time > Packages_Receiver_e[current_pID]){
							break;
						}
						if (current_Time < Packages_Receiver_s[current_pID]){
							current_Time = Packages_Receiver_s[current_pID];
						}
						current_Time += Packages_unload[current_pID];

						temp_mass -= Packages_mass[current_pID];
						temp_vol -= Packages_vol[current_pID];

						prev_ID = Packages_Receiver_ID[current_pID];
					}

				}


				// if add Receiver to Route
				int temp_Time = current_Time + travel_Time(prev_ID, Packages_Receiver_ID[Package_ID], Truck_ID);
				if(temp_Time > Packages_Receiver_e[Package_ID]){
					break;
				}

				if (temp_Time < Packages_Receiver_s[Package_ID]){
					temp_Time = Packages_Receiver_s[Package_ID];
				}

				temp_Time += Packages_unload[Package_ID];
				
				int temp_prev_ID = Packages_Receiver_ID[Package_ID];

				//check all constraint till second last Route
				bool check_constraints = true;

				// run to the lass Process in the Route
				for(int k = j; k < sol_Routes[Truck_ID].size(); k++) {
					int current_pID = sol_Routes[Truck_ID][k].pID;
					if(sol_Routes[Truck_ID][k].is_Sen) {
						temp_Time += travel_Time(temp_prev_ID, Packages_Sender_ID[current_pID], Truck_ID);
						if(temp_Time > Packages_Sender_e[current_pID]) {
							check_constraints = false;
							break;
						}

						if (temp_Time < Packages_Sender_s[current_pID]){
							temp_Time = Packages_Sender_s[current_pID];
						}

						temp_Time += Packages_load[current_pID];

						temp_prev_ID = Packages_Sender_ID[current_pID];
					} 
					else {
						temp_Time += travel_Time(temp_prev_ID, Packages_Receiver_ID[current_pID], Truck_ID);
						if(temp_Time > Packages_Receiver_e[current_pID]) {
							check_constraints = false;
							break;
						}

						if (temp_Time < Packages_Receiver_s[current_pID]){
							temp_Time = Packages_Receiver_s[current_pID];
						}

						temp_Time += Packages_unload[current_pID];

						temp_prev_ID = Packages_Receiver_ID[current_pID];
					}

				}
				
				//actual runtime when return to hub
				int runtime = temp_Time + travel_Time(temp_prev_ID, Trucks_hub[Truck_ID], Truck_ID);
				// if sastisfied last constraint
				if( check_constraints && runtime <= Trucks_e[Truck_ID]) {
					//update best position to insert
					if(runtime < best_time){
						best_time = runtime;
						best_Sender_index = i;
						best_Receiver_index = j + 1;
					}
				}
			}
		}

		if(i == sol_Routes[Truck_ID].size()) {
			break;
		}
		
		// calc actual current mass, vol
		if(sol_Routes[Truck_ID][i].is_Sen) {
			current_vol += Packages_vol[sol_Routes[Truck_ID][i].pID];
			current_mass += Packages_mass[sol_Routes[Truck_ID][i].pID];
		} 
		else {
			current_vol -= Packages_vol[sol_Routes[Truck_ID][i].pID];
			current_mass -= Packages_mass[sol_Routes[Truck_ID][i].pID];
		}
	}	


	if(best_time != INF && is_Insert) {
		sol_Routes[Truck_ID].insert(sol_Routes[Truck_ID].begin() + best_Sender_index, {Package_ID, Packages_Sender_ID[Package_ID], 0, true});
		sol_Routes[Truck_ID].insert(sol_Routes[Truck_ID].begin() + best_Receiver_index, {Package_ID, Packages_Receiver_ID[Package_ID], 0, false});
		//Truck_calc_runtime
		Truck_calc_runtime(Truck_ID);
	}

	return best_time;
}

vector<Node> Truck_Route_to_Sol(int Truck_ID){

	vector<Node> solution;

	Node first_temp;
	first_temp.ID = Trucks_hub[Truck_ID];
	first_temp.timeIn = sec_to_time(Trucks_s[Truck_ID]);
	first_temp.timeOut = sec_to_time(Trucks_s[Truck_ID]);

	solution.pb(first_temp);

	if (sol_Routes[Truck_ID].size() == 0){   
		return solution;
	}


	int n = sol_Routes[Truck_ID].size();

	int solution_index = 0;

	for(int i = 0; i < n; i++){
		Step step;
		step.pID = sol_Routes[Truck_ID][i].pID;
		if (sol_Routes[Truck_ID][i].is_Sen){
			step.timeIn = sec_to_time(sol_Routes[Truck_ID][i].timeDone - Packages_load[sol_Routes[Truck_ID][i].pID]);
		}
		else{
			step.timeIn = sec_to_time(sol_Routes[Truck_ID][i].timeDone - Packages_unload[sol_Routes[Truck_ID][i].pID]);
		}


		if (sol_Routes[Truck_ID][i].ID == solution[solution_index].ID){

			solution[solution_index].processes.pb(step);
			solution[solution_index].timeOut = sec_to_time(sol_Routes[Truck_ID][i].timeDone);
		}
		else{
			solution_index += 1;

			Node temp;    
			temp.ID = sol_Routes[Truck_ID][i].ID;

			temp.processes.pb(step);

			if (sol_Routes[Truck_ID][i].is_Sen){
				temp.timeIn = sec_to_time(sol_Routes[Truck_ID][i].timeDone - Packages_load[sol_Routes[Truck_ID][i].pID]);
			}
			else{
				temp.timeIn = sec_to_time(sol_Routes[Truck_ID][i].timeDone - Packages_unload[sol_Routes[Truck_ID][i].pID]);
			}


			temp.timeOut = sec_to_time(sol_Routes[Truck_ID][i].timeDone);

			solution.pb(temp);

		}
	}

	Node last_temp;
	last_temp.ID = Trucks_hub[Truck_ID];
	last_temp.timeIn = sec_to_time(sol_Routes[Truck_ID][sol_Routes[Truck_ID].size() - 1].timeDone + travel_Time(solution[solution.size()-1].ID, Trucks_hub[Truck_ID], Truck_ID) );
	last_temp.timeOut = last_temp.timeIn;

	solution.pb(last_temp);

	return solution;

}

// end Truck methods


// class Solver

// all Solver methods
void import_data(){
	cin >> nV;
	for(int i = 1; i <= nV; i++) {
		for(int j = 1; j <= nV; j++) {
			cin >> distance_matrix[i][j];
		}
	}

	cin >> K;	
	for(int i = 1; i <= K; i++) {		
		Truck_indexes[i] = i;

		cin >> Trucks_hub[i];
		
		string temp_Time; 

		cin >> temp_Time;
		Trucks_s[i] = time_to_sec(temp_Time);
		
		cin >> temp_Time;
		Trucks_e[i] = time_to_sec(temp_Time);

		cin >> Trucks_mass[i];
		cin >> Trucks_vol[i];
		cin >> Trucks_vel[i];
	}	

	cin >> N;
	for(int i = 1; i <= N; i++) {
		cin >> Packages_Sender_ID[i] >> Packages_Receiver_ID[i];

		cin >> Packages_mass[i];
		cin >> Packages_vol[i];

		cin >> Packages_load[i];
		cin >> Packages_unload[i];

		string temp_Time; 
		cin >> temp_Time;
		Packages_Sender_s[i] = time_to_sec(temp_Time);
		cin >> temp_Time;
		Packages_Sender_e[i] = time_to_sec(temp_Time);
		
		cin >> temp_Time;
		Packages_Receiver_s[i] = time_to_sec(temp_Time);
		cin >> temp_Time;
		Packages_Receiver_e[i] = time_to_sec(temp_Time);
	}
	
}

// run local search with n-opt
void local_n_opt(int num_opt, int &num_serve, int &Del_percentage){

	vector<Process> temp_Routes[K+1];

	shuffle(Truck_indexes + 1, Truck_indexes + 1 + K, rng);

	int i = 0;

	for(int i = 1; i <= K-num_opt; i++) {
		bool temp_Serves[N+1];

		for (int j = 1; j <= N; j++){
			temp_Serves[j] = Serves[j];
		}

		int temp_num_serve = num_serve;
		for(int j = i; j <= i + num_opt; j++) {
			int Truck_ID = Truck_indexes[j];

			temp_Routes[Truck_ID] = sol_Routes[Truck_ID];

			sol_Routes[Truck_ID].clear();

			for(int i = 0; i < temp_Routes[Truck_ID].size(); i++) {
				if(temp_Routes[Truck_ID][i].is_Sen) {
					if(rng() % 100 < Del_percentage) {
						Serves[temp_Routes[Truck_ID][i].pID] = 0;
						temp_num_serve --;
					} 
					else {
						sol_Routes[Truck_ID].pb(temp_Routes[Truck_ID][i]);
					}
				} 
				else {
					if(Serves[temp_Routes[Truck_ID][i].pID]) {
						sol_Routes[Truck_ID].pb(temp_Routes[Truck_ID][i]);
					}
				}
			}
			
			Truck_calc_runtime(Truck_ID);
		}

		int cantInsert[N+1];

		for(int j = i; j <= i + num_opt; j++) {
			for(int Package_ID = 1; Package_ID <= N; Package_ID++){
				if(Serves[Package_ID]){
					cantInsert[Package_ID] = true;
					continue;
				}
				cantInsert[Package_ID] = false;
			}

			int Truck_ID = Truck_indexes[j];
			
			while(true) {
				int best_Package_ID = 0, best_time = INF;
				for(int Package_ID = 1; Package_ID <= N; Package_ID++) {
					if(cantInsert[Package_ID]) {
						continue;
					}
					int temp_time = Truck_Insert(Package_ID, Truck_ID, false);
					if(temp_time == INF) {
						cantInsert[Package_ID] = true;
					}
					else if(temp_time < best_time) {
						best_time = temp_time;
						best_Package_ID = Package_ID;
					}
				}

				if (best_Package_ID == 0){
					break;
				}

				Serves[best_Package_ID] = true;
				cantInsert[best_Package_ID] = true;
				Truck_Insert(best_Package_ID, Truck_ID, true);
				temp_num_serve ++;
			}
		}

		//update best solution
		if(num_serve <= temp_num_serve) {
			num_serve = temp_num_serve;
		}
		
		//if current solution worst than old solution, reset current solution to old solution
		else {

			for (int j = 1; j <= N; j++){
				Serves[j] = temp_Serves[j];
			}

			for(int j = i; j <= i + num_opt; j++) {
				int Truck_ID = Truck_indexes[j];
				sol_Routes[Truck_ID] = temp_Routes[Truck_ID];
			}


		}
	}
	
}

// main solve function
int solve(int opt_time, int num_opt, int Del_percentage) { 	
	//random SEED
	mt19937 rng(SEED);

	//initial solution
	int num_serve = 0;
	for(int Package_ID = 1; Package_ID <= N; Package_ID++) {
		for(int Truck_ID = 1; Truck_ID <= K; Truck_ID++) {
			if(Truck_Insert(Package_ID, Truck_ID, 1) != INF) {
				Serves[Package_ID] = true;
				num_serve++;
				break;
			}
		}
	}

	if (nV == 100){

		while(clock() - start_time <= 120 * CLOCKS_PER_SEC && num_serve < N) {
			local_n_opt(num_opt -1, num_serve, Del_percentage);
		}

		while(clock() - start_time <= 240 * CLOCKS_PER_SEC && num_serve < N) {
			local_n_opt(num_opt, num_serve, Del_percentage);
		}

		while(clock() - start_time <= opt_time * CLOCKS_PER_SEC && num_serve < N) {
			local_n_opt(num_opt+1, num_serve, Del_percentage);
		}
	}

	else if (nV==200 || nV == 300) {
		while(clock() - start_time <= 60 * CLOCKS_PER_SEC && num_serve < N) {
			local_n_opt(num_opt, num_serve, Del_percentage);
		}

		while(clock() - start_time <= 180 * CLOCKS_PER_SEC && num_serve < N) {
			local_n_opt(num_opt-1, num_serve, Del_percentage);
		}

		while(clock() - start_time <= opt_time * CLOCKS_PER_SEC && num_serve < N) {
			local_n_opt(num_opt-2, num_serve, Del_percentage);
		}
	}

	else {
		while(clock() - start_time <= 120 * CLOCKS_PER_SEC && num_serve < N) {
			local_n_opt(num_opt, num_serve, Del_percentage);
		}

		while(clock() - start_time <= 240 * CLOCKS_PER_SEC && num_serve < N) {
			local_n_opt(num_opt-1, num_serve, Del_percentage);
		}

		while(clock() - start_time <= opt_time * CLOCKS_PER_SEC && num_serve < N) {
			local_n_opt(num_opt-2, num_serve, Del_percentage);
		}
	}


	return num_serve;

}

//Route to sol
void print_sol(){
	for (int Truck_ID = 1; Truck_ID <= K; Truck_ID++){
		vector<Node> solution;

		solution = Truck_Route_to_Sol(Truck_ID);

		// fprintf(f, "%ld\n", solution.size());
		cout << solution.size() << endl;

		for (auto &node: solution){
			// fprintf(f, "%d %lu %s %s\n", node.ID, node.processes.size(), node.timeIn.c_str(), node.timeOut.c_str());
			cout << node.ID << " " << node.processes.size() << " " << node.timeIn << " " << node.timeOut << endl;
			for (auto &process: node.processes){
				// fprintf(f, "%d %s\n", process.pID, process.timeIn.c_str());
				cout << process.pID << " " << process.timeIn << endl;
			}
		}
	}

}


int main(){
	ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);


	int timeOpt = 299, n_Opt = 3, Del_Percentage = 30;

	import_data();

	int num_serve = solve(timeOpt, n_Opt, Del_Percentage);

	print_sol();

	cout << "\nnumserve: " << num_serve << "\n";
	cout << "runtime: " << double(clock() - start_time) / double(CLOCKS_PER_SEC) << endl;
}
