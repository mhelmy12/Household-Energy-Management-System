#include <iostream>
#include <iomanip>
using namespace std;
const int Avg_person = 200 , Avg_space = 9 ,Avg_device =200 , Avg_heater = 550 ;
    double price;
const int column_width = 35;


//=====================================
enum Use {
    once,
    daily,
    mo_fr,
    sa_su,
    weekly,
};
//=====================================

void input_use(string &use, Use & u) {

    cin >> use;
    if (use == "once" || use == "o") {
        u = once;
    } else if (use == "daily" || use == "d") {
        u = daily;
    } else if (use == "mo_fr" || use == "m") {
        u = mo_fr;
    } else if (use == "sa_su" || use == "s") {
        u = sa_su;
    } else if (use == "weekly" || use == "w") {
        u = weekly;
    } else {
        u = once;
    }
    
}

//=====================================
struct consumer
{
    string description;
    double watt,watt_standby,hours;
    Use use;
    consumer * next;

};

//=====================================

struct household
{
    string name;
    int person , space , device;
    bool isHeat;
    consumer* consumers;
    int numConsumers;
    int numhousehold;
    int N;
};
household h;
//=====================================
void add_consumer_to_household(household* h, consumer* c) {
    if (h == nullptr) {
        h->consumers = c;
        c->next = nullptr;
    } else {
        c->next = h->consumers;
        h->consumers = c;
    }

    h->numConsumers++;

    

}

//=====================================
consumer* move_up(household* h, int k){
    if(k>h->numConsumers)
    {
        return nullptr;
    }
    consumer* newNode= new consumer;
    consumer* current = h->consumers;
    consumer* prev = h->consumers;
    int count = 1;

    //TARGETING THE NODE
    while(count<k){
        prev = current;
        current = current->next;
        count++;
    }

    //COPYING THE DATA
    newNode->description = current->description;
    newNode->watt = current->watt;
    newNode->watt_standby = current->watt_standby;
    newNode->hours = current->hours;
    newNode->use = current->use;

    if(k==2){
        newNode->next = h->consumers;
        h->consumers= newNode;
    }
    else{
    consumer* temp = h->consumers;
    int count = 1;
    while(count<k-2){
        temp = temp->next;
        count++;
    
    }
    newNode->next = temp->next;
    temp->next = newNode;
    }

    prev->next = current->next;
    return current;

}
//=====================================
double  annual_hours_of_use(consumer* c){
    double operatingHours = c->hours;
    if (c->use == daily) {
        return operatingHours * 365;
    } else if (c->use == weekly) {
        return operatingHours * 52;
    } else if (c->use == sa_su) {
        return operatingHours * 104;
    } else if (c->use == mo_fr) {
        return operatingHours * 260;
    } else if (c->use == once) {
        return operatingHours;
    } else {

        return 0.0;
    }

}
//====================================

double annual_hours_of_standby(consumer* c) {
    return 8760 - annual_hours_of_use(c);
}

//=====================================
double annual_kWh(consumer* c) {
    return (annual_hours_of_use(c) * c->watt + annual_hours_of_standby(c) * c->watt_standby) / 1000;
}

//=====================================



void print_consumer(consumer* c, int powerConsumption) {
    if (c == NULL)
    {
        return;
    }
    cout << right << setw(column_width)<< powerConsumption << ": " << c->description <<" (at address: "<<&(c->description)<<")"<< endl;
    cout << right << setw(column_width)<< "Power Consumption: "<<fixed<<setprecision(2)<<c->watt << " W" << endl;
    cout << right << setw(column_width)<< "Power Consumption Standby: " << fixed<<setprecision(2)<<c->watt_standby << " W" << endl;
    cout << right << setw(column_width)<< "annual hours of use: "<<fixed<<setprecision(2)<<annual_hours_of_use(c) << " h" << endl;
    cout << right << setw(column_width)<< "annual hours of standby: "<<fixed<<setprecision(2)<<annual_hours_of_standby(c) << " h" << endl;
    cout << right << setw(column_width)<< "annual consumption: "<<fixed<<setprecision(1)<<annual_kWh(c) << " W" << endl;
    cout << right << setw(column_width)<< "annual costs: "<<fixed<<setprecision(2)<<annual_kWh(c)*price << " W" << endl;
}


//=====================================
double Power_consumption(household h)
{

    if(h.isHeat) 
        return (Avg_heater * h.person)+(Avg_space * h.space);
    else 
        return (Avg_person * h.person)+(Avg_space * h.space);
}
//====================================
double Power_costs(household *h, double price )
{
    double consumption = 0;
    consumer* current = h->consumers;
    while (current != nullptr) {
        consumption += annual_kWh(current);
        current = current->next;
    }
    return (Power_consumption(*h) +consumption)*price;
}
//=================================
void print_household(household* h, double price) {
    if (h->numhousehold == 0)
    {
        return;   
    }
    

    cout << "H O U S E H O L D N O "<<h->N<<" P O W E R   C O N S U M P T I O N" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    cout << right << setw(column_width)<< "city :" << h->name <<" (at address: "<<&(h->consumers)<<")" <<endl;
    cout << right << setw(column_width)<< "price for one kWh: "<<fixed<<setprecision(2)<< price*100 << " ct/kWh" << endl;
    cout << right << setw(column_width)<< "square metres: " << h->space <<" qm"<< endl;
    cout << right << setw(column_width)<< "persons: " << h->person << endl;
    cout << right << setw(column_width)<< "water heated using electricity: " << (h->isHeat ? "yes" : "no") << endl;

    cout<<endl;
    cout << right << setw(column_width)<< "LIST OF CUSTOMERS" << endl;
    cout << right << setw(column_width)<< "----------------------------------------------------------------------" << endl;
    consumer* current = h->consumers;
    int powerConsumption = 1;
    while (current != NULL) {
        print_consumer(current, powerConsumption);
        current = current->next;
        powerConsumption++;
    }
    cout << endl;
    
    
    cout << right << setw(column_width)<< "----------------------------------------------------------------------" << endl;
    cout << right << setw(column_width)<< "power consumption square meters: " <<fixed<<setprecision(1)<< h->space * Avg_space << " kWh" << endl;
    cout << right << setw(column_width)<< "power consumption all persons: ";
    if(h->isHeat) 
        cout<<fixed<<setprecision(1)<< (Avg_heater * h->person);
    else 
        cout<<fixed<<setprecision(1)<< (Avg_person * h->person);
    cout << " kWh" << endl;
    cout << right << setw(column_width)<< "total annual power consumption: " <<fixed<<setprecision(1)<< Power_costs(h, price)/price << " kWh" << endl;
    cout << right << setw(column_width)<< "annual power costs: " <<fixed<<setprecision(1)<< Power_costs(h, price) << " EUR" << endl;
    cout << endl;
}
//===========================================

void input_city(string& city) {
    cout << "In which city is the household located? ";
    cin >> city;
}
//===========================================

void input_integer(const string &prompt, int& num) {
    cout << "how many "<<prompt<< " does the household have? ";
    cin >> num;

}
//===========================================


void input_electric_water_heating(bool& isHeat) {
    char c;
    cout << "is hot water heated using electricity? (y(es) or n(o)) ";
    cin >> c;
    if (c == 'y') {
        isHeat = true;
    } else {
        isHeat = false;
    }
}
//===========================================

void input_household(household* h, int n) {
    cout << "how many square metres does the household have? ";
    cin >> h->space;

    cout << "how many persons live in this household? ";
    cin >> h->person;

    char c;
    cout << "is hot water heated using electricity? (y(es) or n(o)) ";
    cin >> c;
    h->isHeat = (c=='y');
    h->numhousehold++;
    h->N = n;
    }


//===========================================

void insert_consumer(household* h, consumer* c , int k) {
    consumer* current = h->consumers;
    int count = 1;
    while (count < k-1) {
        current = current->next;
        count++;
    }
    c->next = current->next;
    current->next = c;
    
    h->numConsumers++;
}

void  copy_consumers(household* source, household* dist) {
    consumer* current = source->consumers;
    int k=1;
    while (current != nullptr) {
        consumer* c = new consumer;
        c->description = current->description;
        c->watt = current->watt;
        c->watt_standby = current->watt_standby;
        c->hours = current->hours;
        c->use = current->use;
        if(k==1) add_consumer_to_household(dist, c);
        else {
            consumer* temp = dist->consumers;
            int count = 1;
            while (count < k-1) {
                temp = temp->next;
                count++;
            }
            c->next = temp->next;
            temp->next = c;
            dist->numConsumers++;           
            }
        k++;
        current = current->next;
    }
}



//===========================================
int main() {
    cout<<"CALCULATION OF AVERAGE POWER COSTS FOR A HOUSE\n";
    cout<<"how many households does the house have? ";
    int n; cin>>n;    
    household* h[n];


    //City
    string city;
    cout<<"in which city the house is located? ";
    cin>>city;

    for(int i=0;i<n;i++)
    {
        h[i]=new household;
        h[i]->consumers = nullptr;
        h[i]->numConsumers = 0;
        h[i]->numhousehold = 0;
        h[i]->name = city;
    }
    //Price
    cout<<"what is the price for one kWh in EUR? ";
    cin>>price;

    //Queries
        while (true)
        {
        cout<<"q quit\ni input power consumer\nu move up power consumer\np print household\na print all households\nn new household\nc copy all consumers (added to already existing ones)\n";
        char c; cin>>c;
        switch (c)
        {
        case 'p':
        {
            cout<<"number of household? ";
            int n; cin>>n;
            h[n-1]->N = n;
            print_household(h[n-1],price);
            break;
        }


        case 'q':
        {return 0;
            break;}

        case 'i':
        {
            cout<<"number of household? ";
            int n; cin>>n;
            consumer* c = new consumer;
            cout << "what is the description of the power consumer? ";
            cin >> c->description;
            cout << "how many watt it will have? ";
            cin >> c->watt;
            cout << "how many watt standby it will have? ";
            cin >> c->watt_standby;
            cout << "how often it will be used?\n";
            cout << "daily(d)\nmo_fr(m)\nonce(o)\nsa_su(s)\nweekly(w)\n";
            string s;
            input_use(s, c->use);
            cout << "how many hours it will be operating then? ";
            cin >> c->hours;
            add_consumer_to_household(h[n-1], c);
            break;

        }
        case 'u':
        {
            cout<<"number of household? ";
            int n; cin>>n;
            int k;
            cout << "which one? ";
            cin >> k;
            consumer* c =  move_up(h[n-1], k);
            if(c!=nullptr) delete c;
            break;
        }
        case 'a':
        {
            for(int i=0;i<n;i++)
            {
                print_household(h[i],price);
            }
            
            break;
        }
        case 'n':
        {
            cout<<"number of household? ";
            int n; cin>>n;
            input_household(h[n-1],n);
            break;
        }
        case 'c':
        {
           cout<< "number of household from which to copy consumers? ";
            int source; cin>>source;

            cout<<"number of household to copy to? ";
            int dist; cin>>dist;
            copy_consumers(h[source-1], h[dist-1]);


            break;
        }
        default:
           { cout<<"sorry wrong choice\n";
            break;
           }


            
        }
        cout<<endl;

        }
        
    

    return 0;
}




