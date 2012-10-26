// Author: Richard Pieterse
// Date: October 2012

#include "objexporter.h"
#include "fstream"
#include "iostream"

using namespace std;

void ExportModel( string filepath, Mesh* model )
{



    // --- update vertex indices ---
    for( int i = 0; i < model->vertices.size(); i++)
        model->vertices[i]->index = i;

    ofstream myfile;
    myfile.open(filepath.c_str());

    cout << filepath << endl;


      myfile << "# Exported from Yggdrasi Procedural Tree Generator " << endl;
    if(model->name == "" )
        model->name = "Tree Mesh";
    myfile << "g " << model->name << endl;

    myfile << endl;

    // ----------------------- Write out vertices ------------------------------
    myfile << "# vertex positions " << endl;
    myfile  << endl;

    vector < Vertex* >& vertices = model->vertices;
    for( unsigned int i = 0; i < vertices.size(); i++)
        myfile << "v " << vertices[i]->position.x <<  " " << vertices[i]->position.y << " " <<  vertices[i]->position.z << endl;

     myfile << endl;

     // ----------------------- Write out Normals ------------------------------

     myfile << "# vertex normals " << endl;
    myfile  << endl;

     for( unsigned int i = 0; i < vertices.size(); i++)
         myfile << "vn " << vertices[i]->normal.x <<  " " << vertices[i]->normal.y << " " <<  vertices[i]->normal.z << endl;

      myfile << endl;

      vector < Face* >& quads = model->quads;
      vector < Face* >& tris = model->triangles;


      // ------------------------ Write out tex coords -------------------------------

      myfile << "# texture coordinates " << endl;
myfile  << endl;

       for( unsigned int i = 0; i < quads.size(); i++)
       {
           Face* face =quads[i];
           for( unsigned int j = 0; j < 4; j++)
           {
               myfile << "vt " << face->texCoords[j].x <<  " " << face->texCoords[j].y << endl;
           //     myfile << "#  " << (face->vertices[j]->index + 1) <<  " "<< endl;

           }
       }

       for( unsigned int i = 0; i < tris.size(); i++)
       {
           Face* face =tris[i];
           for( unsigned int j = 0; j < 3; j++)
           {
               myfile << "vt " << face->texCoords[j].x <<  " " << face->texCoords[j].y << endl;
               // myfile << "#  " << (face->vertices[j]->index + 1) <<  " "<< endl;
           }
       }



     // ------------------------ Write out faces -------------------------------

        myfile << "# faces " << endl;
myfile  << endl;
      for( unsigned int i = 0; i < quads.size(); i++)
      {
          myfile << "f ";
          Face* face =quads[i];
          int vtIndex = i*4 + 1;
          for( unsigned int j = 0; j < 4; j++)
              myfile << (face->vertices[j]->index + 1)   << "/" << (vtIndex + j) << "/" <<  (face->vertices[j]->index +1) <<  " ";

          myfile << endl;
      }


      for( unsigned int i = 0; i < tris.size(); i++)
      {
          myfile << "f ";
          Face* face =tris[i];
          int vtIndex = quads.size()*4 + i*3 + 1;
          for( unsigned int j = 0; j < 3; j++)
              myfile << (face->vertices[j]->index + 1)   << "/" << (vtIndex + j) << "/" <<  (face->vertices[j]->index +1) <<  " ";


          myfile << endl;

        //   myfile << "#  " << (face->vertices[j]->index + 1) <<  " "<< endl;
      }


      myfile.close();
}
