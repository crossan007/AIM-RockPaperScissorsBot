#include<iostream>
#include "ctocaim.h"
#include "strutils.h"
#include <vector>
#include <fstream>
using namespace std;


string StripTags(string HTML)


    {
    	bool InTag=false;
    	int Counter=0;
    	while(Counter<HTML.length())


        	{
        		
        		if("<"==HTML.substr(Counter,1))


            		{
            			InTag=true;
            		}
            		else if(">"==HTML.substr(Counter,1))


                		{
                			HTML.replace(Counter,1,"");
                			Counter--;
                			InTag=false;
                		}
                		if(true==InTag)
                		{	
                			HTML.replace(Counter,1,"");
                			Counter--;
                		}
                		
                		Counter++;
                	}
                	return HTML;
            }


struct Challenge
{
	string From;
	string To;
	string Value;
	int Stats;
};

bool Win(Challenge,Challenge);
bool Win(Challenge Player1,Challenge Player2)
{
	if(Player1.Value == "r" && Player2.Value=="p") return false;
	else if(Player1.Value == "p" && Player2.Value=="r") return true;
	else if(Player1.Value == "s" && Player2.Value=="r") return false;
	else if(Player1.Value == "r" && Player2.Value=="s") return true;
	else if(Player1.Value == "p" && Player2.Value=="s") return false;
	else if(Player1.Value == "s" && Player2.Value=="p") return true;

}

struct UserStats
{
	string UserName;
	int Wins,Losses,Ties;
};

void LoadDatabase(vector<UserStats>&);
void LoadDatabase(vector<UserStats> & Stats)
{
	ifstream Load;
	UserStats Temp;
	Load.open("stats.txt");
	while(!Load.eof())
	{
		Load>>Temp.UserName>>Temp.Wins>>Temp.Losses>>Temp.Ties;
		Stats.push_back(Temp);
	}
	Load.close();
}
void SaveDatabase(vector<UserStats>);
void SaveDatabase(vector<UserStats> Stats)
{
	ofstream Save;
	Save.open("stats.txt");
	for(int x=0;x<Stats.size();x++)
	{
		Save<<Stats[x].UserName<<" "<<Stats[x].Wins<<" "<<Stats[x].Losses<<" "<<Stats[x].Ties<<endl;
	}
	Save.close();
}

int main()
{
	CTocAim::AIM Aol("userbot3894","12345");
	Aol.connect();
	CTocAim::AIMEvent Event;
	string Message;
	vector<Challenge> Users;
	Challenge Temp;
	Challenge Swap;
	string ToWinner, ToLoser;
	bool FoundMatch=false;
	int FoundIndex=-1;
	UserStats TempStat;
	vector<UserStats> Stats;
	LoadDatabase(Stats);
	while(Aol.connected())
	{
		Event = Aol.do_event();
		if(Event.type() == "IM_IN")
		{
			FoundMatch=false;
			FoundIndex=-1;
			for(int x=0; x<Stats.size();x++)
			{
				if(Stats[x].UserName==Event.from())
				{
					FoundIndex=x;
				}
			}
			if(FoundIndex==-1)
			{
				TempStat.UserName=Event.from();
				TempStat.Wins=0;
				TempStat.Losses=0;
				TempStat.Ties=0;
				Stats.push_back(TempStat);
				FoundIndex=Stats.size()-1;
			}
			Temp.Stats=FoundIndex;

			
			Message=StripTags(Event.arg(1));
			if (Message=="SaveDB" && Event.from()=="crossan007")
			{
				SaveDatabase(Stats);
				Aol.send_im("crossan007","db saved");
			}
			else if(Message=="Stats")
			{
				Aol.send_im(Event.from(),"Wins: "+tostring(Stats[FoundIndex].Wins)+" Ties: "+tostring(Stats[FoundIndex].Ties)+" Losses: "+tostring(Stats[FoundIndex].Losses));

			}
			else if(Message[0] == 'r' || Message[0]=='p'||Message[0]=='s')
			{
				Temp.From=LowerString(Event.from());
				Temp.To=LowerString(Message.substr(2,Message.length()-2));
				Temp.Value=Message[0];
				cout<<"Incoming: "<<Temp.From<<" "<<Temp.To<<" "<<Temp.Value<<endl<<endl<<endl;


				for(int x=0;x<Users.size();x++)
				{
					cout<<x<<": "<<Users[x].From<<" "<<Users[x].To<<" "<<Users[x].Value<<endl;

					if(Users[x].To==Temp.From)
					{
						if(Users[x].Value==Temp.Value)
						{
							Aol.send_im(Users[x].From,"You Tied with "+Users[x].To+" "+Users[x].Value);
							Aol.send_im(Temp.From,"You Tied with "+Temp.To+" "+Users[x].Value);
							Stats[Temp.Stats].Ties++;
							Stats[Users[x].Stats].Ties++;
						}
						else if(Win(Users[x],Temp))
						{
							Aol.send_im(Users[x].From,"You "+Users[x].Value+" won against "+Temp.From+" "+Temp.Value);
							Aol.send_im(Temp.From,"You "+Temp.Value+" Lost against "+Temp.To+" "+Users[x].Value);
							Stats[Temp.Stats].Losses++;
							Stats[Users[x].Stats].Wins++;

						}
						else
						{
							Aol.send_im(Temp.From,"You "+Temp.Value+" Won against "+Temp.To+" "+Users[x].Value);
							Aol.send_im(Users[x].From,"You "+Users[x].Value+" Lost against "+Temp.From+" "+Temp.Value);
							Stats[Temp.Stats].Wins++;
							Stats[Users[x].Stats].Losses++;
						}
						FoundMatch=true;
						cout<<"Found Match. Removing Entry"<<endl;
						Swap=Users[x];
						Users[x]=Users[Users.size()-1];
						Users[Users.size()-1]=Swap;
						Users.pop_back();
						cout<<"entry removed size"<<Users.size()<<endl;
					}
				}
				if(!FoundMatch)
				{
					cout<<"No match found. adding to list"<<endl;
					Users.push_back(Temp);
					cout<<"added to list size:"<<Users.size()<<endl;
				}

			}
			else
			{
				Aol.send_im(Event.from(),"Please use the following format to play rock paper scissors: (r|p|s|) FriendsScreenName");
			}


		}
	}
	return 0;
}