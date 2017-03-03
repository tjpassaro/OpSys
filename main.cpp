#include <iostream>
#include <string>
#include <fstream>  

vector<Process> all;

processInfile(ifstream in);

int main( int argc, char * argv[] )
{
	ifstrean fin;
	ifstream fout;

	if(argc != 3)
	{
		fprintf(stderr,"ERROR: incorrect usage\nUSAGE: ./a.out <input-file> <stats-output-file>\n");
		exit(1);
	}

    fin.open( argv[1], in);
    fout.open( argv[2], out);

    processInfile(fin);
}

processInfile(ifstream in)
{
	char id;
	int arrival;
	int burst;
	int num_bur;
	int io;
	string buf = "";
	while(in.getline(buf, 100)) //until reach eof
	{
		if(buf[0] == '#')
			continue;
		sscanf(buf, "%c|%i|%i|%i|%i", id, arrival, burst, num_bur, io);

		//WILL NEED TO CHANGE ACCORDING TO TJ'S CLASS
		Process temp(id, arrival, burst, num_bur, io); //create process
		all.push_back(temp);


	//check if com
	}



}
