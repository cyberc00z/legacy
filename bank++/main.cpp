#include <iostream>
#include <fstream>
#include <cstdlib>
using std::cin;
using std::cout;
using std::endl;
using std::fstream;
using std::ofstream;
using std::ifstream;
using std::ios;

class bank_query
{
  private:
     char account_number[2];
     char firstName[10];
     char lastName[10];
     float Balance;
  public:
     void read_data();
     void show_data();
     void write_rec();
     void read_rec();
     void search_rec();
     void edit_rec();
     void delete_rec();

};

void bank_query::read_data(){
   cout << "\nEnter Account Number: ";
   cin >> account_number;
   cout << "\n Enter first name : ";
   cin >> firstName;
   cout << "\n Enter lastName : ";
   cin >> lastName;
   cout << "\n Balance : ";
   cin >> Balance;
};

void bank_query::show_data(){
   cout << "Account Number : " << account_number << endl;
   cout << "First Name : " << firstName << endl;
   cout <<  "Last Name : " << lastName << endl;
   cout << "Balance : " <<  Balance  << endl;
   cout << " ----------------------------------- " << endl;
}

void bank_query::write_rec(){
   ofstream outfile;
   outfile.open("record.txt", ios::app);
   read_data();
   outfile.write(reinterpret_cast<char *>(this), sizeof(*this) );
   outfile.close();
}

void bank_query::read_rec(){
    ifstream infile;
    infile.open("record.txt",ios::out);
    if (!infile){
      cout << "Error in Opening! File Not Found!" << endl;
      return;
    }
    cout << "\n*********** DATA FROM FILE ********"<< endl;
    while(!infile.eof())
    {
       if (infile.read(reinterpret_cast<char*>(this), sizeof(*this))){
          show_data();
       }
    }
    infile.close();
}

void bank_query::search_rec()
{
   int n;
   ifstream infile;
   infile.open("record.txt", ios::out);
   if (!infile){
      cout << "\n Error in opening! File Not Found!" << endl;
      return;
   }
   infile.seekg(0, ios::end);
   int counting = infile.tellg()/sizeof(*this);
   cout << "\n There are " << counting << "records in the file";
   cout << "\n Enter Account Number to Search  : ";
   cin >> n;
   infile.seekg((n-1)*sizeof(*this));
   infile.read(reinterpret_cast<char *>(this), sizeof(*this));
   show_data();
}

void bank_query::edit_rec(){
   int n;
   fstream iofile;
   iofile.open("record.txt", ios::in|ios::out);
   if(!iofile){
      cout << "\n Error in opening file " << endl;
      return;
   }
   iofile.seekg(0, ios::end);
   int counting = iofile.tellg()/sizeof(*this);
   cout << "\nThere are " << counting << "record in the file ";
   cout << "\n Enter Record Number to edit : ";
   cin >> n;
   iofile.seekg((n-1)*sizeof(*this));
   iofile.read(reinterpret_cast<char *>(this), sizeof(*this));
   cout << "Record : " << n << "has following data" << endl;
   show_data();
   iofile.close();
   iofile.open("record.bank", ios::out|ios::in);
   iofile.seekg((n-1)* sizeof(*this));
   cout << "\n Enter data to Modify : " << endl;
   read_data();
   iofile.write(reinterpret_cast<char *>(this), sizeof(*this));
}

void bank_query::delete_rec(){
   int n;
   ifstream infile;
   infile.open("record.txt", ios::out);
   
   if (!infile){
      cout << "\n Error in opening! File Not Found!!" << endl;
      return;
   }

   infile.seekg(0, ios::end);
   int counting = infile.tellg()/sizeof(*this);
   cout << "\n There are " << counting << "record in the file " << endl;
   cout << "\n Enter Record Number to Delete :   ";
   cin >> n;
   fstream tmpfile;
   tmpfile.open("tmpfile.txt", ios::out|ios::out);
   infile.seekg(0);
   for (int i=0; i < counting ; i++){
       infile.read(reinterpret_cast<char *>(this), sizeof(*this));
       if (i==(n-1)){
          continue;
       }
       tmpfile.write(reinterpret_cast<char *>(this), sizeof(*this));
   }

   infile.close();
   tmpfile.close();
   remove("record.txt");
   rename("tmpfile.txt", "record.txt");

}

int main(){
   bank_query A;
   int c;
   cout << "**********BANKING SYSTEM**********************" << endl;
   while (true){
       cout << "Select One option : ";
       cout<<"\n\t1-->Add record to file";

        cout<<"\n\t2-->Show record from file";

        cout<<"\n\t3-->Search Record from file";

        cout<<"\n\t4-->Update Record";

        cout<<"\n\t5-->Delete Record";

        cout<<"\n\t6-->Quit";

        cout<<"\nEnter your choice: ";

        cin>>c;
        switch(c){
          case 1:
              A.write_rec();
              break;
          case 2:
              A.read_rec();
              break;
          case 3:
              A.search_rec();
              break;
          case 4:
              A.edit_rec();
              break;
          case 5:
              A.delete_rec();
              break;
          case 6:
              exit(0);
              break;
          default:
              cout <<"\nEnter correct choice ";
              exit(0);
          

        }
   }
   system("pause");
   return 0;
}
