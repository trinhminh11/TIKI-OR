#include<bits/stdc++.h>

using namespace std;

using ll = long long;
using ull = unsigned long long;

#define endl "\n"
#define pb push_back
#define pf push_front

//random seed
mt19937 rng(11042004);

//infinity
const ll INF = 1e9;

//bound
const int bound = 1001;

const int num_candidate = 5;

//distance_matrix
int distance_matrix[bound][bound];
//number of node, number of trucks, number of packages
int nV, K, N;

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

//struct to read solution
struct Step2{
    int pID;
    string timeIn;
};

//struct to read solution
struct Step{
    int ID;
    vector<Step2> nodes;
    string timeIn;
    string timeOut;
};

//node is Sender or Receiver base on boolean is_Sen
class Node{
public:
    Node(){};
    int pID, ID, load, s, e;
    double mass, vol;
    bool is_Sen;
    int timeDone;

    void init(int pID, int ID, double mass, double vol, int load, int s, int e, bool is_Sen){
        this->pID = pID;
        this->ID = ID;
        this->mass = mass;
        this->vol = vol;
        this->load = load;
        this->s = s;
        this->e = e;
        this->is_Sen = is_Sen;
        this->timeDone = s + load;
    }

    void call(){
        if(is_Sen){
            cout << "pID: " << this->pID << ", sID: " << this->ID << ", mass: " << this->mass << ", vol: " << this->vol << ", load: " << sec_to_time(this->load) << ", " << sec_to_time(this->s) << "-" << sec_to_time(this->e) << endl;
        }
        else{
            cout << "pID: " << this->pID << ", rID: " << this->ID << ", mass: " << this->mass << ", vol: " << this->vol << ", unload: " << sec_to_time(this->load) << ", " << sec_to_time(this->s) << "-" << sec_to_time(this->e) << endl;
        }
    }
    
};

//class Package
class Package{
public:
    Package(int ID, int sID, int rID, double mass, double vol, int load, int unload, int ep, int lp, int ed, int ld){
        this->init(ID, sID, rID,  mass, vol, load, unload, ep, lp, ed, ld);
    };
    int pID, sID, rID, load, unload, ep, lp, ed, ld, mid;
    double mass, vol;
    Node sender, receiver;
    
    void init(int ID, int sID, int rID, double mass, double vol, int load, int unload, int ep, int lp, int ed, int ld){
        this->pID = ID;
        this->sID = sID;
        this->rID = rID;
        this->mass = mass;
        this->vol = vol;
        this->load = load;
        this->unload = unload;
        this->ep = ep;
        this->lp = lp;
        this->ed = ed;
        this->ld = ld;

        this->mid = (ep+lp+ed+ld)/4;
        
        // Sender of package
        this->sender.init(ID, sID, mass, vol, load, ep, lp, true);
        // Receiver of package
        this->receiver.init(ID, rID, mass, vol, unload, ed, ld, false);
    }

    void call(){
        cout << "pID: " << this->pID << ", sID: " << this->sID << ", rID: " << this->rID << ", mass: " << this->mass << ", vol: " << this->vol << ", load: " << sec_to_time(this->load) << ", unload: " << sec_to_time(this->unload) << ", " << sec_to_time(this->ep) << "-" << sec_to_time(this->lp) << ", " << sec_to_time(this->ed) << "-" << sec_to_time(this->ld) << endl;
    }        
};

//class Truck
class Truck{
public:
    int ID, hub, st, et;
    double mass, vol, vel;
    //solution route and temporary Route
    vector<Node> S_R_Route, temp_Route, candidate[num_candidate];
    int candidate_serve[num_candidate];

    Truck(int ID, int hub, int st, int et, double mass, double vol, double vel){
        this->ID = ID;
        this->hub = hub;
        this->st = st;
        this->et = et;
        this->mass = mass;
        this->vol = vol;
        this-> vel = vel;

        Package dummy(0, hub, hub, 0, 0, 0, 0, st, st, st, st);
        dummy.sender.timeDone = st;
        dummy.receiver.timeDone = st;
        this->S_R_Route.pb(dummy.sender);
        this->S_R_Route.pb(dummy.receiver);

        this->temp_Route = this->S_R_Route;
        
        for(int i = 0; i<num_candidate; i++){
            this->candidate[i] = this->S_R_Route;
            this->candidate_serve[i] = 0;
        }

    }

    void call(){
        cout << "Truck: " << this->ID << ", hub: " << this->hub << ", start: " << this->st << ", end: " << this->et << ", mass: " << this->mass << ", vol: " << this->vol << ", vel: " << this->vel << endl; 
    }

    //check mass and vol constraints
    bool check_mass_vol(vector<Node> &Route){
        double current_mass = 0;
		double current_vol = 0;
        for(int i = 0; i < Route.size(); i++){
            if (Route[i].is_Sen){
                current_mass += Route[i].mass;
                current_vol += Route[i].vol;
                if (current_mass > this->mass || current_vol > this->vol){
                    return false;
                }
            }
            else{
                current_mass -= Route[i].mass;
                current_vol -= Route[i].vol;
            }
        }

        return true;
    }

    //calc runtime in Route, return -1 if not sastified time constraints
    int calc_runtime(vector<Node> &Route){

        int current_time = this->st;

        if (Route.size() == 2){
            return current_time;
        }
        
        for (int i = 1; i < Route.size() - 1; i++){
            int start = Route[i].ID;
            int end = Route[i+1].ID;
            int temp_time = current_time + int(ceil(3600*double(distance_matrix[start-1][end-1])/this->vel));

            if (temp_time <= Route[i+1].s){
                current_time = Route[i+1].s + Route[i+1].load;
                Route[i+1].timeDone = current_time;
            }
            else if(temp_time > Route[i+1].e){
                return -1;
            }
            else{
                current_time += int(ceil(3600*double(distance_matrix[start-1][end-1])/this->vel)) + Route[i+1].load;
                Route[i+1].timeDone = current_time;
            }
        }

        return current_time;
    }

    //check all constraints
    bool check_constraints(vector<Node> &Route){
        bool check_time = true;
        int current_time = this->calc_runtime(Route);
        bool c_mass_vol = this->check_mass_vol(Route);

        if (current_time == -1 ||  c_mass_vol == false){
            return false;
        }

        current_time += int(ceil( 3600*distance_matrix[Route[Route.size()-1].ID-1][this->hub-1]/this->vel ));

        if (current_time > this->et){
            return false;
        }

        return true;

    }

    //insert a Package to Route
    int Insert(Package &package, bool is_temp = false, bool is_Insert = true){
        vector<Node> used;

        if (is_temp){
			used = this->temp_Route;
        }
		else{
			used = this->S_R_Route;
        } 
		

		vector<Node> temp_Route = used;

		vector<Node> best_Route = used;

		int best_time = INF;

		int n = temp_Route.size();
		int m = n + 1;

		for (int i = 2; i < n+1; i++){
            if (package.sender.e < temp_Route[i-1].timeDone){
                break;
            }

            temp_Route.insert(temp_Route.begin() + i, 1, package.sender);

            if(this->check_constraints(temp_Route)){

                for (int j = i+1; j < m+1; j++){
                    // cout << sec_to_time(temp_Route[j].timeDone) << endl;
                    if (package.receiver.e < temp_Route[j-1].timeDone){
                        // cout << sec_to_time(package.receiver.e) << " " << sec_to_time(temp_Route[j-1].timeDone) <<" " << m << endl;
                        break;
                    }

                    temp_Route.insert(temp_Route.begin() + j, 1, package.receiver);
                    if (this->check_constraints(temp_Route)){
                        int temp_time = this->calc_runtime(temp_Route) + int(ceil( 3600*distance_matrix[temp_Route[temp_Route.size()-1].ID-1][this->hub-1]/this->vel ));
                        if (temp_time < best_time){
                            best_Route = temp_Route;
                            best_time = temp_time;
                        }
                    }

                    temp_Route = used;
                    temp_Route.insert(temp_Route.begin() + i, 1, package.sender);

                }
            }

            temp_Route = used;
        }

        if (best_time != INF && is_Insert){
            if (is_temp){
                this->temp_Route = best_Route;
            }
            else{
                this->S_R_Route = best_Route;
            }

        }

        return best_time;
    }

    //from Route to solution
    vector<Step> Route_to_Sol(){

        vector<Step> solution;

        Step first_temp;
        first_temp.ID = this->hub;
        first_temp.timeIn = sec_to_time(this->st);
        first_temp.timeOut = sec_to_time(this->st);

        solution.pb(first_temp);

        if (this->S_R_Route.size() == 2){
            
        }

        else{
            int n = this->S_R_Route.size();
            this->calc_runtime(this->S_R_Route);
            
            int solution_index = 0;

            for(int i = 2; i < n; i++){

                Step2 step2;
                step2.pID = this->S_R_Route[i].pID;
                step2.timeIn = sec_to_time( this->S_R_Route[i].timeDone - this->S_R_Route[i].load );


                if (this->S_R_Route[i].ID == solution[solution_index].ID){

                    solution[solution_index].nodes.pb(step2);
                    solution[solution_index].timeOut = sec_to_time(this->S_R_Route[i].timeDone);
                }
                else{
                    solution_index += 1;
                
                    Step temp;    
                    temp.ID = this->S_R_Route[i].ID;
                    temp.nodes.pb(step2);
                    temp.timeIn = sec_to_time(this->S_R_Route[i].timeDone - this->S_R_Route[i].load);
                    temp.timeOut = sec_to_time(this->S_R_Route[i].timeDone);

                    solution.pb(temp);

                }
            }
            Step last_temp;
            last_temp.ID = this->hub;
            last_temp.timeIn = sec_to_time(this->S_R_Route[n-1].timeDone + int(ceil( double(3600*distance_matrix[this->S_R_Route[n-1].ID-1][this->hub-1]) /this->vel)) );
            last_temp.timeOut = last_temp.timeIn;
            
            solution.pb(last_temp);
        }

			

		return solution;

    }

};


int count_serve(int serve[]){
    int ans = 0;
    for(int i = 0; i < N;i++){
        if (serve[i]==1){
            ans ++;
        }
    }
    return ans;
}

vector<Truck> trucks;
vector<Package> packages;


//Import Data
void import_data(){

    //distance matrix
    cin >> nV;
	for(int i = 0; i<nV;i++){
        for (int j = 0; j < nV; j++){
            cin >> distance_matrix[i][j];
        }
    }

    //Trucks
    cin >> K;

    for(int i = 0; i<K; i++){
        int hub;
		string temp_st, temp_et;
        double mass, vol, vel; 
        int st, et;

        cin >> hub >> temp_st >> temp_et >> mass >> vol >> vel;
		
		st = time_to_sec(temp_st);
        et = time_to_sec(temp_et);

        Truck truck(i+1, hub, st, et, mass, vol, vel);

        trucks.pb(truck);
		
    }

    //Packages
    cin >> N;

    for(int i = 0; i<N; i++){
        int sID, rID, load, unload;
        double mass, vol;
        string temp_ep, temp_lp, temp_ed, temp_ld;
        int ep, lp, ed, ld;

        cin >> sID >> rID >> mass >> vol >> load >> unload >> temp_ep >> temp_lp >> temp_ed >> temp_ld;

        ep = time_to_sec(temp_ep);
        lp = time_to_sec(temp_lp);
        ed = time_to_sec(temp_ed);
        ld = time_to_sec(temp_ld);

        Package package(i+1, sID, rID, mass, vol, load, unload, ep, lp ,ed, ld);

        packages.pb(package);
    }
}

int find_Package_base_on_ID(int ID){
    for (int i = 0; i<N;i++){
        if (packages[i].pID == ID){
            return i;
        }
    }
    cout <<"false";
    exit(0);
}

//Solver class
class Solver{
public:
    int serve[bound];
    int used_truck;
    int total_runtime = 0;
    int num_serve = 0;
    vector<int> candidate_serve[num_candidate];
    int num_candidate_serve = 0;
    //solutions vector
    vector<vector<Step>> solutions;			
    string solution = "";		
    //cost package			
    Solver(){init();};

    void init(){
        this->used_truck = K;
        for(int i = 0; i<K; i++){
            vector<Step> sol;
            this->solutions.pb(sol); 
        }

        for(int i = 0; i<N;i++){
            this->serve[i] = 0;
        }

    }

    //read solution
    void read_sol(){
        for (auto sol : this->solutions){
            cout << sol.size() << endl;
            
            for (int i = 0; i<sol.size()-1; i++){
                cout << sol[i].ID << " " << sol[i].nodes.size() << " " << sol[i].timeIn << " " << sol[i].timeOut << endl;

                for (int j = 0; j < sol[i].nodes.size(); j++){
                    cout << sol[i].nodes[j].pID << " " << sol[i].nodes[j].timeIn << endl;
                }
            }

            cout << sol[sol.size()-1].ID << " " << sol[sol.size()-1].nodes.size() << " " << sol[sol.size()-1].timeIn << " " << sol[sol.size()-1].timeOut << endl;

            this->total_runtime += time_to_sec(sol[sol.size()-1].timeIn) - time_to_sec(sol[0].timeIn);
        }

    }

    void local_search(int time){
        int cost[N];
        int Package_IDs[N];
        for (int i = 0; i<N;i++){
            Package_IDs[i] = i+1; 
        }

        while (clock() < time*CLOCKS_PER_SEC){

            for (auto &truck: trucks){
                truck.temp_Route = truck.S_R_Route;
            }

            int current_serve[N];

            for(int i =0; i<N;i++){
                current_serve[i] = this->serve[i];
            }

            int current_num_serve = this->num_serve;


            int numDel = rng() % min(N/7, 50) + 1;

            for(int z=0; z<numDel;z++){
                //reset cost to 0
                for(int i =0;i<N;i++){
                    cost[i] = 0;
                }

                //calc cost function
                for(int i = 0; i < K; i++) {

                    for(int j = 2; j < trucks[i].temp_Route.size(); j++) {
                        int package = trucks[i].temp_Route[j].pID-1;

                        if (trucks[i].temp_Route[j-1].pID-1 != package){
                            cost[package] += trucks[i].temp_Route[j].timeDone - trucks[i].temp_Route[j-1].timeDone;
                        }

                        if (i+1 == trucks[i].temp_Route.size()){
                            cost[package] += int( ceil( 3600 * double(distance_matrix[trucks[i].temp_Route[j].ID-1][trucks[i].hub-1]) / trucks[i].vel ) );
                        }
                        else{
                            cost[package] += trucks[i].temp_Route[j+1].timeDone - trucks[i].temp_Route[j].timeDone;
                        }

                    }
                }



                //Sort base on cost
                sort(Package_IDs, Package_IDs + N, [&](int x, int y) {
                    return cost[x-1] > cost[y-1];
                });

                //choose package to delete
				int min_index = rng() % 200 + 100;
				int rPackage_ID = Package_IDs[rng() % min( (N/4) , min_index)];

                for (auto &truck: trucks){
                    bool found = false;
                    for (auto node: truck.temp_Route){
                        if (node.pID == rPackage_ID){
                            Package package = packages[find_Package_base_on_ID(rPackage_ID)];
                            found = true;
                            break;
                        }
                    }
                    if (found){
                        vector<Node> temp;

                        for(int i = 0; i < truck.temp_Route.size(); i++) {
                            if(truck.temp_Route[i].pID != rPackage_ID) {
                                temp.pb(truck.temp_Route[i]);
                            }
                        }
                        truck.temp_Route = temp;
                        current_num_serve -= 1;
                        current_serve[rPackage_ID-1] = 0;
                        break;
                    }
                }

            }

            shuffle(packages.begin(), packages.end(), rng);

            for(auto &package: packages){
                if (current_serve[package.pID-1] == 0){
                    sort(trucks.begin(), trucks.end(), [&](Truck x, Truck y) {
                        return x.temp_Route.size() > y.temp_Route.size();
                    });
                    for(auto &truck: trucks){
                        if(truck.Insert(package, true, true) != INF){
                            current_serve[package.pID-1] = 1;
                            current_num_serve += 1;
                            break;
                        }
                        else{
                            package.sender.timeDone = 0;
                            package.receiver.timeDone = 0;
                        }
                    }
                }
            }

            if(current_num_serve > this->num_serve){
                for (auto &truck: trucks){
                    truck.S_R_Route = truck.temp_Route;
                }

                for (int i = 0; i<N;i++){
                    this->serve[i] = current_serve[i];
                }
                this->num_serve = current_num_serve;
                // cout << this->num_serve << endl;


            }

        }
    }

    void local_search_n_opt(int n, int time){


        while (clock() < time*CLOCKS_PER_SEC){


            shuffle(trucks.begin(), trucks.end(), rng);

            

            for(int i = 0; i <= K - n; i++) {

                int current_serve[N];
                int current_num_serve = this->num_serve;

                //reset temp to best
                for (auto &truck: trucks){
                    truck.temp_Route = truck.S_R_Route;
                }
                

                for(int j = 0; j < N; j++){
                    current_serve[j] = this->serve[j];
                }


                for(int j = i; j < i + n; j++) {

                    vector<Node> temp_Route;
                    
                    for(int k = 0; k < trucks[j].temp_Route.size(); k++) {
                        if (k <2){
                            temp_Route.pb(trucks[j].temp_Route[k]);
                        }
                        else{

                            if(trucks[j].temp_Route[k].is_Sen == true) {

                                if(rng() % 4 == 0) {

                                    if( current_serve[trucks[j].temp_Route[k].pID-1] == 0){
                                        cout << "????????";
                                        exit(0);
                                    }
                                    current_serve[trucks[j].temp_Route[k].pID-1] = 0;
                                    current_num_serve -=1;
                                } 
                                else {
                                    temp_Route.pb(trucks[j].temp_Route[k]);
                                }
                            } 

                            else {
                                if(current_serve[trucks[j].temp_Route[k].pID-1] == 1) {
                                    temp_Route.pb(trucks[j].temp_Route[k]);
                                }
                            }
                        }

                    }

                    trucks[j].temp_Route = temp_Route;

                }

                bool canInsert[N];

                for(int j = i; j < i + n; j++) {

                    for(int k = 0; k < N; k++) {
                        canInsert[k] = true;
                    }


                    while(true) {
                        int best_pos = -1, best = INF;
                        for(int k = 0; k < N; k++) {

                            if(current_serve[packages[k].pID-1] == 1 || canInsert[packages[k].pID-1] == false){
                                continue;
                            }

                            int current = trucks[j].Insert(packages[k], true, false);
                            if(current == INF) {
                                canInsert[packages[k].pID-1] = false;
                            }
                            if(current < best){
                                best = current;
                                best_pos = k;
                            }
                        }

                        if(best_pos == -1) {
                            break;
                        }

                        

                        if (trucks[j].Insert(packages[best_pos], true, true) != INF){
                            if (current_serve[packages[best_pos].pID-1] == 1){
                                cout << "??";
                                exit(0);
                            }
                            current_serve[packages[best_pos].pID-1] = 1;
                            current_num_serve ++;
                        }
                        else{
                            cout <<"???";
                            exit(0);
                        }

                        
                    }

                }


                if(this->num_serve < current_num_serve) {

                    for(auto &truck: trucks){
                        truck.S_R_Route = truck.temp_Route;
                    }

                    for(int j = 0; j < N; j++){
                        this->serve[j] = current_serve[j];
                    }

                    this->num_serve = current_num_serve;
                    // cout << this->num_serve << endl;
                }

            }

        }
    }

    //main solve
    void solve(){
        for (auto &package: packages){
            for (auto &truck: trucks){
                if(truck.Insert(package, false, true) != INF){
                    this->serve[package.pID-1] = 1;
                    this->num_serve ++;
                    break;
                }
                else{
                    package.sender.timeDone = 0;
                    package.receiver.timeDone = 0;
                }
            }
        }



        this->local_search(60);

        this->local_search_n_opt(5, 297);


        for (auto &truck: trucks){
            this->solutions[truck.ID-1] = truck.Route_to_Sol();

            if (this->solutions[truck.ID-1].size() == 1){
                this->used_truck -= 1;
            }
        }

        this->read_sol();

        cout << this->num_serve;
		
    }

};

int main(){

    import_data();

    sort(trucks.begin(), trucks.end(), [&](Truck x, Truck y) {
            return x.vel < y.vel;
        });
    
    sort(packages.begin(), packages.end(), [&](Package x, Package y) {
            if (x.mid < y.mid){
                return true;
            }
            else if (x.mid == y.mid){
                if (x.lp < y.lp){
                    return true;
                }
                else if(x.lp == y.lp){
                    if(x.ld < y.ld){
                        return true;
                    }
                    else if (x.ld == y.ld){
                        if (x.ep < y.ep){
                            return true;
                        }
                        else{
                            return false;
                        }
                    }
                    else{
                        return false;
                    }
                }
                else{
                    return false;
                }
            }
            else{
                return false;
            }
        });


    Solver sol;
    sol.solve();

}
