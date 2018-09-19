//============================================================================
// Name        : LISA.cpp
// Author      : sid
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <sstream>
#include <bitset>
#include <iterator>
using namespace std;


vector<unsigned char> corruptSync(int , vector<unsigned char>&);

string encodePayload(string );

void populateFile(int , vector<unsigned char> , string , ofstream &op_stream);

string findPayload(int confidence, vector<unsigned char> &buffer );



int main() {

	std::ofstream ofs("mytestdata.txt");
	ofs.clear();
	unsigned char b[]= {0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
			0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF};
	//unsigned char b[]= {0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF};


	cout<<"---------------------------------------------------------"<<endl;
	cout<<b[0];

	vector<unsigned char > sync(b,b+32);

	vector<unsigned char > buffer(b,b+32);


	std::cin.clear();
	string payload;
	cout<<"Enter the Payload"<<endl;
	std::getline(std::cin, payload);
	cout <<endl;

	std::cin.clear();

	int corruptPercentage;
	cout<< "Enter corruption percent"<<endl;
	cin  >>  corruptPercentage;
//
	int confidence_level;
		cout<<"confidence_level= "<<endl;
		cin>> confidence_level;

	//corruptSync(corruptPercentage,sync);   //corrupting the vector bits

	populateFile(0,sync,payload, ofs); //populating the file with junk, sync and payload
	//ofs.close();



	string t1=findPayload(confidence_level,buffer);

	string output= encodePayload(t1);

	cout<<"Retrieved Payload= "<<output<<endl;

	return 0;
}

void populateFile(int junk_size, vector<unsigned char> sync, string payload, ofstream &op_stream){



	while(junk_size--)
	{
		if(junk_size==0){
			cout<<"Inside Break"<<endl;
			break;
		}
		srand(time(NULL));

		char random_integer= random()%255;

		op_stream << std:: bitset<8>(random_integer);


	}
	//placing the sync vector after the file has been populated with junk

	int sync_size=sync.size();

	int i=0;

	while(sync_size--)
	{

		//op_stream << std::bitset<8>(sync.at(i));

		i++;
	}

	//convert the payload into binary form
	int j=0,size=payload.length();
//	cout<<"size= "<<size;

	string payload_binary;

		while(size--)
		{

			std::bitset<8> conv(payload.c_str()[j]);

			payload_binary+=conv.to_string();

			j++;

		}

		op_stream << payload_binary;   //place the payload_binary after the sync bit in the file

		//cout<<endl<<"Payload in Bin= "<<payload_binary<<endl;






}

vector<unsigned char> corruptSync(int percent, vector<unsigned char>& v)
{

	cout<<"v.size= "<<v.size();
	double to_corrupt= ceil((percent*v.size()*8)/100);

	cout<<"to_corrupt= "<<to_corrupt<<endl;
	srand(time(NULL));
    while(to_corrupt--)
	{



		int index= random()%v.size();

		cout<<"Index= "<<index<<endl;



		int bitsetIndex= random()%8;

		cout<<"bitsetIndex= "<<bitsetIndex<<endl;
		cout<<"BEfore corruption"<<std::bitset<8>(v[index])<<endl;

		v[index] ^= (1 << bitsetIndex);
		cout<<"After corruption"<<std::bitset<8>(v[index])<<endl;

	}

    return v;

}

string encodePayload(string payload_binary)
{
	    string output;
		std::stringstream sstream(payload_binary);
			    while(sstream.good())
			    {
			        std::bitset<8> bits;
			        sstream >> bits;
			        char c = char(bits.to_ulong());
			        output += c;
			    }

	return output;
}

string findPayload(int confidence, vector<unsigned char>& buffer )
{
	char a;
	int j=0,i=0, compare_confidence=0,v_index=0,count=0,j_value=0;
	vector<char> temp;
	ifstream in;
	in.open("mytestdata.txt");
	if(!in.is_open()){
		cout<<"Fail"<<endl;
	}

	if(in)
	{
		while(in >> a)
		{

			temp.push_back(a);
		}

	}

	while(v_index<buffer.size())
	{

		std:: bitset<8> kernel(buffer[v_index]);
		std::string v8_str = kernel.to_string();
		std::cout << "string form: 1 " << v8_str << '\n';
		char a[9] = {0};
		strcpy(a, v8_str.c_str());

		cout<< "Kernel "<< a<<endl;

		j=0;

		if(compare_confidence>=confidence)
			break;

		while(j<temp.size() )

		{
			i=0;
			if(compare_confidence >= confidence)
			{
				break;
			}

			while(i<8)
			{
				if(temp.at(j)==a[i])

								{

									i=i+1;
									j=j+1;

									if(i==7) //when there is full match
									{
										compare_confidence++;

										cout<<"Compare Confidence= "<<compare_confidence<<endl;
										cout<<"Value of j= "<<j<<endl;
										if(compare_confidence==confidence){
											break;
										}

										else if(++v_index<buffer.size()){

										i=0;
										j++;
									    std:: bitset<8> kernel(buffer.at(v_index));
									    std::string v8_str = kernel.to_string();
									    	strcpy(a, v8_str.c_str());

										}

									}

								}
				else{

					j+= (8-i);
					break;

				    }

			}

			j_value=j;

		}

		v_index++;
		cout<<"Inside-loop 3"<<endl;
	}

	cout<<"v_index= "<<v_index<<endl;
	cout<<"j_value = "<<j_value<<endl;
	int index= j_value+((buffer.size()-v_index)*8)+1;
	cout<<"Buffer size= "<<buffer.size()<<endl;
	string estimated_payload;
	cout<<"Index= "<<index<<endl;
	while(index<temp.size())

	{

		estimated_payload+=temp[index];
		index++;

	}
	//cout<<"estimated_payload= "<< estimated_payload<<endl;
	in.close();
	return estimated_payload;

}
