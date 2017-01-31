#include <iostream>
#include <string.h>
#include <getopt.h>
#include <sstream>
#include <vector>
#include <random>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>

using namespace std;

struct client{
  long long ID;
  string name;
  bool buy;
  long long price;
  long long quantity;
};

struct transfer{
  long long bought;
  long long sold;
  long long net;
};

struct sell_comp{
  bool operator()(const client* x, 
			  const client* y) const{
	if(x->price > y->price) return 1;
	else if(x->price < y->price) return 0;
	else return (x->ID > y->ID);
  }
};

struct buy_comp{
  bool operator()(const client* x, 
			  const client* y) const{
	if(x->price < y->price) return 1;
	else if(x->price > y->price) return 0;
	else return (x->ID > y->ID);
  }
};

struct order{
  priority_queue<client*, vector<client*>, sell_comp> sq;
  priority_queue<client*, vector<client*>, buy_comp> bq;
};

struct buy_price{
  long long price;
  long long time;
};

struct t_comp{
  bool operator()(const buy_price* x, 
			  const buy_price* y) const{
	return(x->price > y->price);
  }
};

struct travel{
  long long profit;
  long long sell_time;
  long long buy_time;
  long long buy_price;
  long long w_buy_time;
  long long w_buy_price;
};

struct Median{
  priority_queue<long long, vector<long long>, std::less<long long>> low;
  priority_queue<long long, vector<long long>, std::greater<long long>> high;
  long long median;
};

int main(int argc, char* argv[]){
  map<string, Median> m_map;
  unordered_map<string, order> o_map;
  map<string, transfer> t_map;
  unordered_map<string, travel> ttt_map;
  vector<string> ttt_vec;
  int s_flag = 0, v_flag = 0;
  int t_flag = 0, m_flag = 0;
  int g_flag = 0;
  extern char *optarg;
  int arg;
  const char* const short_options = "svmti:g:h";
  const struct option long_options[] = {
	{"summary",		0,	0,	's'},
	{"verbose",		0,	0,	'v'},
	{"median",		0,	0,	'm'},
	{"transfers",	0,	0,	't'},
	{"insider",		1,	0,	'i'},
	{"ttt",			1,	0,	'g'},
	{"help",		0,	0,	'h'},
	{0,				0,	0,	 0 }
  };

  do{
	arg = getopt_long(argc, argv, short_options, long_options, NULL);
	switch (arg){
	case 's':{
	  s_flag = 1;
	  break;
	}
	case 'v':{
	  v_flag = 1;
	  break;
	}
	case 'm':{
	  m_flag = 1;
	  break;
	}
	case 'h':
	  exit(0);
	  break;
	case 't':
	  t_flag = 1;
	  break;
	case 'i':{
	  string temp = optarg;
	  transfer newtrans;
	  newtrans.bought = 0;
	  newtrans.sold = 0;
	  newtrans.net = 0;
	  t_map["INSIDER_" + temp] = newtrans;
	  break;
	}
	case 'g':{
	  g_flag = 1;
	  ttt_vec.push_back(optarg);
	  travel newtravel;
	  newtravel.buy_time = -1;
	  newtravel.sell_time = -1;
	  newtravel.buy_price = -1;
	  newtravel.w_buy_time = -1;
	  newtravel.w_buy_price = -1;
	  newtravel.profit = 0;
	  ttt_map[ttt_vec.back()] = newtravel;
	  break;
	}
	case '?':
	  exit(1);
	  break;
	case -1:
	  break;
	default:
	  abort();
	}
  }while(arg != -1);
  string line;
  getline(cin, line);
  string input_type = line;
  if(input_type != "TL" && input_type != "PR"){
	fprintf(stderr, "Wrong input type!\n");
	exit(1);
  }
  string temp;
  istringstream iss;
  long long random_seed = 0, num_of_orders = 0;
  char last_client, last_equity;
  double arri_rate;
  if(input_type == "PR"){
	getline(cin, line);
	iss.str(line);
	iss >> temp >> random_seed;
	iss.clear();
	getline(cin, line);
	iss.str(line);
	iss >> temp >> num_of_orders;
	iss.clear();
	getline(cin, line);
	iss.str(line);
	iss >> temp >> last_client;
	iss.clear();
	getline(cin, line);
	iss.str(line);
	iss >> temp >> last_equity;
	iss.clear();
	getline(cin, line);
	iss.str(line);
	iss >> temp >> arri_rate;
	iss.clear();
  }
  long long timestamp = 0;
  long long GENERATOR_TIMESTAMP = 0;
  long long count = 0;
  long long cur_timestamp = 0;
  long long NOST = 0;
  long long NOCT = 0;
  long long MT = 0;
  long long CE = 0;
  long long cur_median = 0;
  long long old_timestamp = 0;
  //read input
  if(input_type == "PR" || input_type == "TL"){
	std::mt19937 gen(random_seed);
    std::uniform_int_distribution<char> clients('a', last_client);
    std::uniform_int_distribution<char> equities('A', last_equity);
	std::exponential_distribution<> arrivals(arri_rate);
	std::bernoulli_distribution buy_or_sell;
	std::uniform_int_distribution<> price(2, 11);
	std::uniform_int_distribution<> quantity(1, 30);
	while(count < num_of_orders || getline(cin, line)){
	  string equity;
	  client* newclient = new client;
	  newclient->ID = count;
	  count++;
	  if(input_type == "PR"){
	    timestamp = GENERATOR_TIMESTAMP;
	    GENERATOR_TIMESTAMP = GENERATOR_TIMESTAMP + floor(arrivals(gen)+.5);
	    newclient->name = string("C_") + clients(gen);
	    newclient->buy = (buy_or_sell(gen)? 1 : 0);
	    equity = string("E_") + equities(gen);
	    newclient->price = 5 * price(gen);
	    newclient->quantity = quantity(gen);
	  }
	  else{
	    count++;
	    char symbol_d, symbol_n;
	    string ifbuy;
	    iss.str(line);
	    iss >> timestamp >> newclient->name >> ifbuy >> 
	    equity >> symbol_d >> newclient->price >> 
	    symbol_n >> newclient->quantity;
	    if(timestamp < 0) {
		  fprintf(stderr, "Negative timestamp!\n");
		  exit(1);
	    }
	    if(timestamp < old_timestamp) {
		  fprintf(stderr, "Wrong timestamp!\n");
		  exit(1);
	    }
	    if(newclient->price <= 0) {
		  fprintf(stderr, "Negative price!\n");
		  exit(1);
	    }
	    if(symbol_d != '$') {
		  fprintf(stderr, "Wrong $ symbol price!\n");
		  exit(1);
	    }
	    if(symbol_n != '#') {
		  fprintf(stderr, "Wrong # symbol price!\n");
		  exit(1);
	    }
	    if(equity.size() < 1 
		  || equity.size() > 5) {
		  fprintf(stderr, "Wrong equity size!\n");
		  exit(1);
	    }
	    if(newclient->name.empty()) {
		  fprintf(stderr, "Empty client name!\n");
		  exit(1);
	    }
        for(unsigned long long i = 0; i < newclient->name.size(); i++){
		  if(!isalnum(newclient->name[i]) && newclient->name[i] != '_'){
		    fprintf(stderr, "Wrong client name!\n");
            exit(1);
		  }
	    }
	    if(ifbuy == "BUY") newclient->buy = 1;
	    else if(ifbuy == "SELL") newclient->buy = 0;
	    else{
		  fprintf(stderr, "Neither BUY nor SELL!\n");
		  exit(1);
	    }
	    iss.clear();
	    old_timestamp = timestamp;
      }
	  //trading
	  if(t_flag && t_map.find(newclient->name) == t_map.end()){
		transfer newtran;
	    newtran.net = 0;
		newtran.bought = 0;
		newtran.sold = 0;
		t_map[newclient->name] = newtran;
	  }
	  if(timestamp != cur_timestamp){
		if(m_flag){
		  map<string, Median>::iterator it;
		  it = m_map.begin();
		  while(it != m_map.end() && !m_map.empty()){
			printf("Median match price of %s at time %lld is $%lld\n", 
			it->first.c_str(), cur_timestamp, m_map[it->first].median);
			it++;
		  }
		}
		cur_timestamp = timestamp;
	  }
	  long long usedprice;
	  long long usedquantity;
	  if(newclient->buy){
	    while(newclient->quantity > 0 && !o_map[equity].sq.empty() 
		  	  && newclient->price 
			  >= o_map[equity].sq.top()->price){
		  client* B;
		  client* S;
		  B = newclient;
		  string Sname = o_map[equity].sq.top()->name;
		  S = o_map[equity].sq.top();
		  usedprice = S->price;
		  if(B->quantity > S->quantity){
		    usedquantity = S->quantity;
		    B->quantity -= S->quantity;
		    S->quantity -= S->quantity;
		    delete o_map[equity].sq.top();
		    o_map[equity].sq.pop();
		  }
		  else if(B->quantity < S->quantity){
		    usedquantity = B->quantity;
		    S->quantity -= B->quantity;
		    B->quantity -= B->quantity;
		  }
		  else{
		    usedquantity = S->quantity;
			B->quantity -= S->quantity;
		    S->quantity -= S->quantity;
		    delete o_map[equity].sq.top();
		    o_map[equity].sq.pop();
		  }
		  if(v_flag)
		    printf("%s purchased %lld shares of %s from %s for $%lld/share\n", 
		    B->name.c_str(), usedquantity, equity.c_str(), Sname.c_str(), usedprice);
		  //median
		  if(m_map.find(equity) == m_map.end()){
			Median newmedian;
			m_map[equity] = newmedian;
			m_map[equity].low.push(usedprice);
			m_map[equity].median = usedprice;
		  }
		  else{
			if(usedprice > m_map[equity].low.top())
			  m_map[equity].high.push(usedprice);
			else
			  m_map[equity].low.push(usedprice);
			if(m_map[equity].high.size() - m_map[equity].low.size() == 2){
			  m_map[equity].low.push(m_map[equity].high.top());
			  m_map[equity].high.pop();
			}
			if(m_map[equity].low.size() - m_map[equity].high.size() == 2){
			  m_map[equity].high.push(m_map[equity].low.top());
			  m_map[equity].low.pop();
			}
			if(m_map[equity].high.size() - m_map[equity].low.size() == 1)
			  m_map[equity].median = m_map[equity].high.top();
			if(m_map[equity].low.size() - m_map[equity].high.size() == 1)
			  m_map[equity].median = m_map[equity].low.top();
			if(m_map[equity].high.size() == m_map[equity].low.size())
			  m_map[equity].median = (m_map[equity].high.top() + m_map[equity].low.top()) / 2;
		  }
		  //summary
		  if(s_flag){
		    long long increment = 0;
		    increment = (usedquantity * usedprice) / 100;
		    CE += 2 * increment;
		    MT += usedquantity * usedprice;
		    NOCT += 1;
		    NOST += usedquantity;
		  }
		  //transfer
		  if(t_flag){
		    t_map[B->name].bought += usedquantity;
		    t_map[B->name].net -= usedquantity * usedprice;
		    t_map[Sname].sold += usedquantity;
		    t_map[Sname].net += usedquantity * usedprice;
		  }
	    }
	  }
	  else{
	    while(newclient->quantity > 0 && !o_map[equity].bq.empty() 
		  	  && o_map[equity].bq.top()->price 
			  >= newclient->price){
		  client* B;
		  client* S;
		  S = newclient;
		  string Bname = o_map[equity].bq.top()->name;
		  B = o_map[equity].bq.top();
		  usedprice = B->price;
		  if(B->quantity > S->quantity){
		    usedquantity = S->quantity;
		    B->quantity -= S->quantity;
		    S->quantity -= S->quantity;
		  }
		  else if(B->quantity < S->quantity){
		    usedquantity = B->quantity;
		    S->quantity -= B->quantity;
		    B->quantity -= B->quantity;
		    delete o_map[equity].bq.top();
		    o_map[equity].bq.pop();
		  }
		  else{
		    usedquantity = S->quantity;
		    S->quantity -= B->quantity;
		    B->quantity -= B->quantity;
		    delete o_map[equity].bq.top();
		    o_map[equity].bq.pop();
		  }
		  if(v_flag)
		    printf("%s purchased %lld shares of %s from %s for $%lld/share\n", 
		    Bname.c_str(), usedquantity, equity.c_str(), S->name.c_str(), usedprice);
		  //median
		  if(m_map.find(equity) == m_map.end()){
			Median newmedian;
			m_map[equity] = newmedian;
			m_map[equity].low.push(usedprice);
			m_map[equity].median = usedprice;
		  }
		  else{
			if(usedprice > m_map[equity].low.top())
			  m_map[equity].high.push(usedprice);
			else
			  m_map[equity].low.push(usedprice);
			if(m_map[equity].high.size() - m_map[equity].low.size() == 2){
			  m_map[equity].low.push(m_map[equity].high.top());
			  m_map[equity].high.pop();
			}
			if(m_map[equity].low.size() - m_map[equity].high.size() == 2){
			  m_map[equity].high.push(m_map[equity].low.top());
			  m_map[equity].low.pop();
			}
			if(m_map[equity].high.size() - m_map[equity].low.size() == 1)
			  m_map[equity].median = m_map[equity].high.top();
			if(m_map[equity].low.size() - m_map[equity].high.size() == 1)
			  m_map[equity].median = m_map[equity].low.top();
			if(m_map[equity].high.size() == m_map[equity].low.size())
			  m_map[equity].median = (m_map[equity].high.top() + m_map[equity].low.top()) / 2;
		  }
		  //summary
		  if(s_flag){
		    long long increment = 0;
		    increment = (usedquantity * usedprice) / 100;
		    CE += 2 * increment;
		    MT += usedquantity * usedprice;
		    NOCT += 1;
		    NOST += usedquantity;
		  }
		  //transfer
		  if(t_flag){
		    t_map[Bname].bought += usedquantity;
		    t_map[Bname].net -= usedquantity * usedprice;
		    t_map[S->name].sold += usedquantity;
		    t_map[S->name].net += usedquantity * usedprice;
		  }
	    }
	  }
	  //insider trading
	  map<string, transfer>::iterator it;
	  it = t_map.find("INSIDER_" + equity);
      map<string, Median>::iterator it2;
	  it2 = m_map.find(equity);
	  bool trade_flag = 0;
	  long long profit = 0;
	  string buyer;
	  string seller;
	  if(it != t_map.end() && it2 != m_map.end() && newclient->quantity > 0){
		cur_median = m_map[equity].median;
		if(newclient->buy){
		  profit = newclient->price - cur_median;
		  if(profit > cur_median / 10){
			trade_flag = 1;
			usedprice = newclient->price;
			usedquantity = newclient->quantity;
			newclient->quantity -= newclient->quantity;
			buyer = newclient->name;
			seller = "INSIDER_" + equity;
		  }
		}
		else{
		  profit = cur_median - newclient->price;
		  if(profit > cur_median / 10){
			trade_flag = 1;
			usedprice = newclient->price;
			usedquantity = newclient->quantity;
			newclient->quantity -= newclient->quantity;
			seller = newclient->name;
			buyer = "INSIDER_" + equity;
		  }
		}
		if(v_flag && trade_flag)
		  printf("%s purchased %lld shares of %s from %s for $%lld/share\n", 
		  buyer.c_str(), usedquantity, equity.c_str(), seller.c_str(), usedprice);
		if(trade_flag){
		  //median
		  if(usedprice > m_map[equity].low.top())
			m_map[equity].high.push(usedprice);
		  else
			m_map[equity].low.push(usedprice);
		  if(m_map[equity].high.size() - m_map[equity].low.size() == 2){
			m_map[equity].low.push(m_map[equity].high.top());
			m_map[equity].high.pop();
		  }
		  if(m_map[equity].low.size() - m_map[equity].high.size() == 2){
			m_map[equity].high.push(m_map[equity].low.top());
			m_map[equity].low.pop();
		  }
		  if(m_map[equity].high.size() - m_map[equity].low.size() == 1)
			m_map[equity].median = m_map[equity].high.top();
		  if(m_map[equity].low.size() - m_map[equity].high.size() == 1)
			m_map[equity].median = m_map[equity].low.top();
		  if(m_map[equity].high.size() == m_map[equity].low.size())
			m_map[equity].median = (m_map[equity].high.top() + m_map[equity].low.top()) / 2;
		  //summary
		  if(s_flag){
		    long long increment = 0;
		    increment = (usedquantity * usedprice) / 100;
		    CE += 2 * increment;
		    MT += usedquantity * usedprice;
		    NOCT += 1;
		    NOST += usedquantity;
		  }
		  //transfer
		  if(t_flag){
		    t_map[buyer].bought += usedquantity;
		    t_map[buyer].net -= usedquantity * usedprice;
		    t_map[seller].sold += usedquantity;
		    t_map[seller].net += usedquantity * usedprice;
		  }
		}
	  }
	  //push newclient to prior queue or not
	  if(newclient->quantity > 0){
	    if(newclient->buy)
		  o_map[equity].bq.push(newclient);
	    else
		  o_map[equity].sq.push(newclient);
	  }
	  //new time travel
	  if(g_flag && ttt_map.find(equity) != ttt_map.end()){
		//traveller buy
		if(!newclient->buy){
		  if(ttt_map[equity].w_buy_time == -1 
			|| (ttt_map[equity].w_buy_time != -1 
			&& (newclient->price < ttt_map[equity].w_buy_price))){
			ttt_map[equity].w_buy_price = newclient->price;
			ttt_map[equity].w_buy_time = timestamp;
		  }
		}
		//traveller sell
		if(newclient->buy || (!newclient->buy && trade_flag)){
		  if(ttt_map[equity].buy_time != -1
			&& (ttt_map[equity].sell_time == -1 
			|| (ttt_map[equity].sell_time != -1 
			&& (newclient->price - ttt_map[equity].buy_price > ttt_map[equity].profit)))){
			ttt_map[equity].sell_time = timestamp;
			ttt_map[equity].buy_time = ttt_map[equity].buy_time;
			ttt_map[equity].buy_price = ttt_map[equity].buy_price;
			ttt_map[equity].profit = newclient->price - ttt_map[equity].buy_price;
		  }
		  if(ttt_map[equity].w_buy_time != -1
			&& (ttt_map[equity].sell_time == -1 
			|| (ttt_map[equity].sell_time != -1 
			&& (newclient->price - ttt_map[equity].w_buy_price > ttt_map[equity].profit)))){
			ttt_map[equity].sell_time = timestamp;
			ttt_map[equity].buy_time = ttt_map[equity].w_buy_time;
			ttt_map[equity].buy_price = ttt_map[equity].w_buy_price;
			ttt_map[equity].profit = newclient->price - ttt_map[equity].w_buy_price;
		  }
		}
		//traveller buy
		if(newclient->buy && trade_flag){
		  if(ttt_map[equity].w_buy_time == -1 
			|| (ttt_map[equity].w_buy_time != -1 
			&& (newclient->price < ttt_map[equity].w_buy_price))){
			ttt_map[equity].w_buy_price = newclient->price;
			ttt_map[equity].w_buy_time = timestamp;
		  }
		}
	  }
	}
  }
  //end of day
  if(m_flag){
	map<string, Median>::iterator it;
	it = m_map.begin();
	while(it != m_map.end() && !m_map.empty()){
	  printf("Median match price of %s at time %lld is $%lld\n", 
	  it->first.c_str(), cur_timestamp, m_map[it->first].median);
	  it++;
	}
  }
  printf("---End of Day---\n");
  if(s_flag){
	printf("Commission Earnings: $%lld\n", CE);
	printf("Total Amount of Money Transferred: $%lld\n", MT);
	printf("Number of Completed Trades: %lld\n", NOCT);
	printf("Number of Shares Traded: %lld\n", NOST);
  }
  if(t_flag){
	map<string,transfer>::iterator it;
	it = t_map.begin();
	while(it != t_map.end()){
	  printf("%s bought %lld and sold %lld for a net transfer of $%lld\n", 
	  it->first.c_str(), it->second.bought, it->second.sold, it->second.net);
	  it++;
	}
  }
  if(g_flag){
	for(unsigned long long i = 0; i < ttt_vec.size(); i++){
	  printf("Time travelers would buy %s at time: %lld and sell it at time: %lld\n", 
	  ttt_vec[i].c_str(), ttt_map[ttt_vec[i]].buy_time, ttt_map[ttt_vec[i]].sell_time);
	}
  }
  return 0;
}
