//============================================================================
// Name        : Ck.cpp
// Author      : Christopher Diggs
// Compile     : g++ -std=c++11 Ck.cpp -o CryptKicker.exe
// Description : ReadMe - CryptKicker.txt
// How-To-RUn  : ./CryptKicker.exe < CryptKickerEx.txt
//============================================================================


#include<iostream>
#include<string.h>
#include<vector>
#include<sstream>
#include<cctype>
#include<unordered_map>
#include<map>
using namespace std;



//===============Global Declarations========================
const int MAX_SIZE = 16;

vector<vector<string> > dictionary(MAX_SIZE+1);
vector<vector<string> > organizedSent(MAX_SIZE+1);
int countL[MAX_SIZE+1];
int numbOfWords;
std::unordered_map<std::string, std::string> hashtable;


bool checkDiff();
bool collision(int row1,int col, int row2);
int findSolution(int numb, int row1, int col1, int row2);
void findNext(int * row1, int *col, int *row2);
void mapLetters(int row1, int col, int row2);
void removeMappedLetters(int row1, int col);

//=============================================================

int main()
{ 
  int n;
  string word;
  //read in dicts and sort them by length of words
  cin>>n;
  for(int i=0;i<n;i++)
    {
      cin>>word;
      dictionary[word.length()].push_back(word);
    }
	
  //read in lines of encoded words
  string line;
  getline(cin, line);
  bool success;
  vector<string> sentence;
 
  
  while(getline(cin, line)) 
    {
      memset(&countL,0,sizeof(countL));
	hashtable.clear();

      istringstream iss (line);
      numbOfWords=0;
	//read in the words
      while(iss>>word)  //Use hash table to check for distinct words in a given sentence
	{ 
		//if word is distinct, then add it the organizedSent array
		if (hashtable.count(word)==0 ){
			hashtable.insert ({word, ""});
	 		 organizedSent[word.length()].push_back(word);
	  		numbOfWords++;
		}
		//still add all words in the sentence to the sentence array
		sentence.push_back(word);
		
	} 


	success=false;
	//check difference
	if (checkDiff()){
		//find the column in organizedSent that contains a word
		int biggestCol;
		for(int i=MAX_SIZE; i>0;i--){
			if (organizedSent[i].size()>0){
				biggestCol=i;
				break;
			}
		}	
		hashtable.clear();
		//call the backtracking process
		if (findSolution(0, 0, biggestCol, 0)){
			success=true;
		}
		
	}

	//if there is succes, then decrypt the sentence
	if(success){

		map <string, string> alphaMap;
		for (auto it : hashtable){ 
			alphaMap[it.second]=it.first;
		}


		for(int i=0; i<sentence.size();i++){
			for(int j=0; j<sentence[i].length();j++){
				cout<<alphaMap[sentence[i].substr(j,1)];
			}
			if (i!=sentence.size()-1)
				cout<<" ";
		}
		cout<<endl;
	}
	else{

		for(int i=0; i<sentence.size();i++){
			for(int j=0; j<sentence[i].length();j++){
				cout<<"*";
			}
			if (i!=sentence.size()-1)
				cout<<" ";
		}
		cout<<endl;

	}


	sentence.clear();
	for(int i=1;i<=MAX_SIZE;i++){
		organizedSent[i].clear();
	}
    } //end while 

  return 0;
}

//checks the difference in the dictionary and organizedSent
bool checkDiff(){
	for(int i=1; i<=MAX_SIZE; i++){
		if (dictionary[i].size()<organizedSent[i].size()){
			return false;
		}
	}
	return true;
}

//This function is the backbone of the backtracking process..
int findSolution(int numb, int row1, int col, int row2)
{ 
if (numb>numbOfWords){
		return true;
	}
	else{
		bool mapped = false;

		while(!mapped && col<=MAX_SIZE && row2<organizedSent[col].size()){
			if(collision(row1, col, row2)){
				++row1;
				if(row1>=dictionary[col].size()){
					return false;
				}
			}
			else{
				mapLetters(row1, col, row2);
				int tempRow1=row1;
				int tempRow2=row2;
				int tempCol=col;

				findNext(&tempRow1, &tempCol, &tempRow2);
				
				mapped=findSolution(numb+1, tempRow1, tempCol, tempRow2);//for testing one word
				if (!mapped){
					removeMappedLetters(row1, col);
					++row1;
					if(row1>=dictionary[col].size()){
						return false;

					}
				}
			}
		}
		return mapped;
	}
}

//find the next avaiable word
 void findNext(int *row1, int *col, int *row2){
	if (countL[*col]==organizedSent[*col].size()){
		for(int i=*col-1; i>0; i--){
			if (organizedSent[i].size()>0){
				
				*col=i;
				*row2=0;
				break;
			}
		}
	}
	else{
		*row2=*row2+1;
	}
	*row1=0;
	return;
}

//test for collision
bool collision(int row1,int col, int row2){
	bool flag=false;

	map<string,string> localMap;

	for(int i=0; i<col;i++){
		if (localMap[organizedSent[col][row2].substr(i,1)]!=""){
			if (localMap[organizedSent[col][row2].substr(i,1)]!=dictionary[col][row1].substr(i,1)){
					return true;
			}
		}
		else localMap[organizedSent[col][row2].substr(i,1)]=dictionary[col][row1].substr(i,1);
	}
	
	unordered_map<string,string>::const_iterator test;
	for(int i=0; i<col;i++){
		 test=hashtable.find(dictionary[col][row1].substr(i,1));
		 if (test!=hashtable.end()){
			if (hashtable.find(dictionary[col][row1].substr(i,1))->second!=organizedSent[col][row2].substr(i,1))
			 	return true;
	
		 }
		
		for (auto it : hashtable){ 
			
			if (it.second==organizedSent[col][row2].substr(i,1)&&it.first!=dictionary[col][row1].substr(i,1)){
				return true;
			}
		}
	}
	
	return flag;
}

//map the letters
void mapLetters(int row1, int col, int row2){
	++countL[col];
	unordered_map<string,string>::const_iterator test;
	for(int i=0; i<col;i++){
		test=hashtable.find(dictionary[col][row1].substr(i,1));
		 if (test==hashtable.end())
			hashtable.insert({dictionary[col][row1].substr(i,1), organizedSent[col][row2].substr(i,1)});
	}
}

//un map the letters
void removeMappedLetters(int row1, int col){
	--countL[col];
	for(int i=0; i<col;i++){
		hashtable.erase(dictionary[col][row1].substr(i,1));
	}
}

