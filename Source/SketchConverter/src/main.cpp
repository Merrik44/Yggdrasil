#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <ctime>

#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include "branch.h"


using namespace std;
using namespace rapidxml;


int main(int argc, char* argv[])
{
    //srand(time(0));
    srand(0);

    const char *fileName = "treefile.xml";

    if (argc > 1)
    	fileName = argv[1];

    //ifstream fin(fileName,ios::binary);

    string line;
    string *data = new string();
    while (getline(cin, line))
        *data += line + '\n';


    char* xmlData = new char[data->size()+1];
    memcpy(xmlData,data->c_str(),data->size()+1);

    delete data;

    //fin.get(xmlData,fileSize);
    //fin.close();

    xml_document<> document;// = new xml_document<>();
    document.parse<0>(xmlData);

    Branch *root = Branch::constructTreeFromXML(&document);
    Branch::convertTo3D(root);

    xml_document<> outputDocument;

    root->writeTreeToXMLDocument(&outputDocument);

    //ofstream fout("treefileout.xml",ios::binary);
    cout << outputDocument << endl;
    //fout.close();

    return 0;
}
