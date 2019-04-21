#include<stdio.h>
#include<iostream>
#include<cstdlib>
#include<string>
#include<cstring>
#include<vector>
#include<algorithm>
#include<fstream>
#include<bitset>

using namespace std;

/*
    rrr = reg reg reg
    rri = reg reg imm.
    rrl = reg reg logical
    rrb = reg reg addr.
    ri  = reg imm.
    rr  = reg reg
    j   = jump
    z   = zero
*/

typedef struct Opcode               //This node is used for Hashing using Chaining.
{
    char name[10];
    char code[35];
    char func[35];
    char format[5];
    struct Opcode *next;
}Opcode;

typedef struct Symbol               //Symbol Table is made using Linked List to save space.
{
    char name[50];
    int address;
    struct Symbol *next;
}Symbol;

int PC = -1;
int BaseAddress = 0;
int ErrorCount = 0;

void tokenize(string const &str, const char delim, vector<std::string> &out);
char *addTobin(int input);
char *shamtTobin(int input);
char *intTobin(int input);
const char *getRegisterCode(char *temp);

class HashTable
{
    public:
        Opcode *hash_table[13];

        HashTable(void)
        {
            for(int i = 0; i<13; i++)
                hash_table[i] = NULL;
        }
        
        int HashFunction(char name[])
        {
            int sum = 0, i = 0;

            while(name[i] != '\0')
            {
                sum += name[i++];
            }

            return sum % 13;
        }

        void insertAtIndex(Opcode *Node, int index)
        {
            if(hash_table[index] == NULL)
            {
                hash_table[index] = Node;
                (*Node).next = NULL;
            }
            else
            {
                Opcode* temp = hash_table[index];

                while((*temp).next != NULL)
                {
                    temp = (*temp).next;
                }
                
                (*temp).next = Node;
                (*Node).next = NULL;
            }
        }

        void insertIntoHashMap(Opcode *Node)
        {
            int index = HashFunction((*Node).name);
            insertAtIndex(Node, index);
        }

        void print()
        {
            for(int i = 0; i < 13; i++)
            {
                if(hash_table[i] != NULL)
                {
                    Opcode* temp = hash_table[i];

                    while(temp != NULL)
                    {
                        cout << "NAME:: "<< (*temp).name << " and CODE:: " << (*temp).code << " and func:: " << (*temp).func << " and format:: " << (*temp).format << endl;
                        temp = (*temp).next;
                    }
                    cout << endl;
                }
            }
        }

        Opcode* getOpcodeNode(char *op)
        {
            Opcode* temp = NULL;
            int index = HashFunction(op);
            
            if(hash_table[index] == NULL)
            {
                return NULL;
            }
            else
            {
                temp = hash_table[index];

                while(strcmp((*temp).name, op) != 0 && (*temp).next != NULL)
                {
                    temp = (*temp).next;
                }

                if(strcmp((*temp).name, op) != 0)
                {
                    return NULL;
                }
            }

            return temp;
        }

        char *getOpcodeFormat(Opcode* temp)
        {
            return (*temp).format;
        }
};

class List
{
    public:
        Symbol *head;

        List(void)
        {
            head = NULL;
        }

        void insert(char op[], int l)
        {
            Symbol *new_node = new Symbol;

            int i = 0;
            for(; i < l; i++)
                (*new_node).name[i] = op[i];
            
            (*new_node).name[i] = '\0';
            (*new_node).address = (PC + BaseAddress) * 4;
            (*new_node).next = NULL;

            if(head == NULL)
                head = new_node;
            else
            {
                Symbol *t = head;
                while((*t).next != NULL)
                    t = (*t).next;
                (*t).next = new_node;
            }
        }

        void print(FILE *f)
        {
            Symbol *p = head;

            while(p != NULL)
            {
                cout << (*p).name << " :: ";
                fprintf(f,"%s :: ", (*p).name);
                cout << (*p).address << endl;
                fprintf(f,"%d\n", (*p).address);
                p = (*p).next;
            }
        }

        char *getAddressCode(char *temp, char m)
        {
            Symbol *t = head;
            char *val;
            int num;

            while(t != NULL)
            {
                if(!strcmp(temp, (*t).name))
                {
                    num = (*t).address;
                    break;
                }

                t = (*t).next;
            }

            if(t == NULL)
                val = (char *)"XXX";
            else
            {
                if(m == 'j')
                    val = addTobin(num);
                else if(m == 'b')
                    val = intTobin(num);
            }

            return val;
        }
};

int main(int argc, char *argv[])
{
    HashTable OPTAB;                //Opcode table.
    List SYMTAB;                    //Symbol table.

//creating Hash-table for OPTAB.
    ifstream opcode_table;
    opcode_table.open("opcode_table.txt");
    string line;

    if (opcode_table.is_open())
    {
        while(getline(opcode_table, line))
        {
            char temp[100];
            vector<string> property;
            tokenize(line, ' ', property);
        
            Opcode* Node = new Opcode;
            copy(property.at(0).begin(), property.at(0).end(), temp);
            temp[property.at(0).size()] = '\0';
            strcpy((*Node).name, temp);

            copy(property.at(1).begin(), property.at(1).end(), temp);
            temp[property.at(1).size()] = '\0';
            strcpy((*Node).code, temp);

            copy(property.at(2).begin(), property.at(2).end(), temp);
            temp[property.at(2).size()] = '\0';
            strcpy((*Node).func, temp);

            copy(property.at(3).begin(), property.at(3).end(), temp);
            temp[property.at(3).size()] = '\0';
            strcpy((*Node).format, temp);

            OPTAB.insertIntoHashMap(Node);
        }
    }
    else
    {
        cout << "Unable to open OPTAB file" << endl << "Assembling Terminated" << endl;
        return 0;
    }

    //cout << "Hash-table Created Successfully!" << endl;
    //OPTAB.print();
    opcode_table.close();
//************************************************************************************

// Removing Multi line Comments.
    //cout << "Reading instructions and Removing Comments ..." << endl;
    
    char *filename1 = argv[1];
    ifstream fin1(filename1);
    ofstream fout1;
    fout1.open("output_no_comments1.txt", ofstream::out | ofstream::trunc);

    string line1;
    int flag2 = 0;

    if(fin1.is_open())
    {
    	while (getline(fin1, line1))
        {
    		int flag = 0;
    		string newLine = line1;
    		string::iterator it; 

    		for (it = newLine.begin(); it != newLine.end(); it++)
            {
                if(flag2 == 0)
                {
                    if(*it == '<')
                    {
                        *it = ' ';
                        flag2 = 1;
                    }
                    else
                    {
                        continue;
                    }
                }
                else if(flag2 == 1)
                {
                    if(*it == '>')
                    {	
                        *it = ' ';
                        flag2 = 0;
                    }
                    else
                    {	
                        *it = ' ';
                    }
                }
    		}

    		fout1 << newLine << endl;
    	}

    	fin1.close();
    }
//************************************************************************************

// Removing Single line comments.
    char filename2[100] = "output_no_comments1.txt";
    ifstream fin2(filename2);
    ofstream fout2;
    fout2.open("output_no_final.txt", ofstream::out | ofstream::trunc);

    string line2;
    
    if(fin2.is_open())
    {
    	while (getline(fin2, line2))
        {
    		int flag = 0;
    		string newLine = line2;
    		string::iterator it; 

    		for(it = newLine.begin(); it != newLine.end(); it++)
            {
                if(flag == 0)
                {
                    if(*it == '#')
                    {
                        *it = ' ';
                        flag = 1;
                    }
                    else
                    {
                        continue;
                    }
                }
                else if(flag == 1)
                {
                    *it = ' ';
                }
    		}

            int f = 0;
            string::iterator itt; 
            for (itt = newLine.begin(); itt != newLine.end(); itt++)
            {                 
                if((int)*itt != 13 && (int)*itt != 32)
                {          
                    f = 1;
                }
            }

            if(f == 1)
            {
                //cout << newLine <<endl;
                fout2 << newLine << endl;
            }
    	}

    	fin2.close();
    }
//************************************************************************************

// Pass-1 of Assembler.
    //cout << endl << "Reading instructions and Converting them to machine codes..." << endl;
    
    char filename[100] = "output_no_final.txt";
    ifstream fin(filename);

    ofstream fout;
    if(argc == 2)
        fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
    else if(argc == 3)
        fout.open(argv[2], ofstream::out | ofstream::trunc);

    //cout << "Pass-1:" << endl;
    if (fin.is_open())
    {
        while (getline(fin, line))
        {
            PC++;
            if(strstr(line.c_str(), "START") != NULL)                // go to START section
                break;
        }
        
        char op[1000];
        int flag = 0;
        while (getline(fin, line))
        {
            PC++;
            flag++;
            if (strstr(line.c_str(), ":") != NULL)                  // if the line contains : then it is label
            {
                //cout << "Label Found!" << endl;
                copy(line.begin(), line.end(), op);
                op[line.size() - 1] = '\0';
                SYMTAB.insert(op, line.size() - 1);
            }
        }

        fin.close();

        if(flag == 0)
        {
            cout << "Error occured while assembling, \"START\" section missing!" << endl;
            exit(0);
        }
    }
    else
    {
    	cout << "Unable to open input file. " << endl << "Assembling Terminated" << endl;
    	return 0;
	}
//************************************************************************************

// Pass-2 of Assembler.
    PC = -1;
    fin.open(filename);

    //cout << "Pass-2:" << endl;
    if (fin.is_open())
    {
        while (getline(fin, line))
        {
            PC++;
            if(strstr(line.c_str(), "START") != NULL)                // go to START section
                break;
        }

        char op[100];
        char k;
        char *binary;
        int count;
        while (getline(fin, line))
        {
            PC++;
            vector<string> instruction;
            tokenize(line, ' ', instruction);

            copy(instruction.at(0).begin(), instruction.at(0).end(), op);
            op[instruction.at(0).size()] = '\0';
            //cout << "WORD SCANNED IS " << op << " ";        //check if opcode or label.
            if(strcmp("END", op) == 0)
                break;

            int l = 0;
            while(op[l + 1] != '\0')
            {
                l++;
            }

            if(op[l] == ':')                                //Its a label
            {
                fout << endl;
                continue;
            }
            else
            {
                //cout << "Inside else" << endl;
                char temp[100];
                char temp2[100];
                char temp3[100];
                int temp4;

                Opcode* current_node = OPTAB.getOpcodeNode(op);
                if(current_node == NULL)
                {
                    ErrorCount++;
                    fout.close();
                    if(argc == 2)
                        fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                    else if(argc == 3)
                        fout.open(argv[2], ofstream::out | ofstream::trunc);
                    cout << "Wrong instruction used at line " << PC + 1 << '.' << endl;
                    continue;                    
                }

                fout << "0x" << hex << (PC + BaseAddress) * 4 << " " << (*current_node).code;          //print machine code of the opcode
                
                if (strcmp("z", OPTAB.getOpcodeFormat(current_node)) == 0)
                {
                    fout << " ";
                }
                else if(strcmp("r", OPTAB.getOpcodeFormat(current_node)) == 0)
                {
                    copy(instruction.at(1).begin(), instruction.at(1).end(), temp);
                    temp[instruction.at(1).size()] = '\0';

                    fout << " " << getRegisterCode(temp);
                    if(getRegisterCode(temp) == "111111")
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". Valid register operand expected!" << endl;
                    }
                }
                else if(strcmp("j", OPTAB.getOpcodeFormat(current_node)) == 0)
                {
                    copy(instruction.at(1).begin(), instruction.at(1).end(), temp);
                    temp[instruction.at(1).size()] = '\0';

                    binary = SYMTAB.getAddressCode(temp, 'j');
                    if(binary == "XXX")
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". Undefined identifier (label)!" << endl;
                    }

                    fout << " ";
                    for(count = 0; count < 26; count++)
                    {
                        fout << binary[count];
                    }
                }
                else if(strcmp("rr", OPTAB.getOpcodeFormat(current_node)) == 0)
                {
                    copy(instruction.at(1).begin(), instruction.at(1).end(), temp);
                    temp[instruction.at(1).size()] = '\0';
                    
                    string::iterator it;
                    int i = 0;
                    for(it = instruction.at(2).begin(); it != instruction.at(2).end(); it++)
                    {
                        if(*it == '(')
                            break;
                        temp3[i++] = *it;
                    }
                    temp3[i] = '\0';
                    temp4 = atoi(temp3);                                    // temp3 contains offset value.
                    if(temp4 < -32768 || temp4 > 32767)
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". Immediate operand out of range!" << endl;                        
                    }

                    for(it++, i = 0; it != instruction.at(2).end(); it++)  // temp2 contains register.
                    {
                        if(*it == ')')
                            break;
                        temp2[i++] = *it;
                    }
                    temp2[i] = '\0';
                    
                    fout << " " << getRegisterCode(temp2) << " " << getRegisterCode(temp);
                    if(getRegisterCode(temp) == "111111" || getRegisterCode(temp2) == "111111")
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". Two valid register operands expected!" << endl;
                    }

                    binary = intTobin(temp4);
                    fout << " ";
                    for(count = 0; count < 16; count++)
                    {
                        fout << binary[count];
                    }
                }
                else if(strcmp("ri", OPTAB.getOpcodeFormat(current_node)) == 0) ///this needs formatting changes.
                {
                    copy(instruction.at(1).begin(), instruction.at(1).end(), temp);
                    temp[instruction.at(1).size()] = '\0';
                    
                    temp4 = stoi(instruction.at(2));
                    if(temp4 < -32768 || temp4 > 32767)
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". Immediate operand out of range!" << endl;                        
                    }

                    fout << " " << getRegisterCode(temp);
                    if(getRegisterCode(temp) == "111111")
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". One valid register and immediate operands expected!" << endl;
                    }

                    binary = intTobin(temp4);
                    fout << " ";
                    for(count = 0; count < 16; count++)
                    {
                        fout << binary[count];
                    }
                }
                else if(strcmp("rrr", OPTAB.getOpcodeFormat(current_node)) == 0)
                {
                    copy(instruction.at(1).begin(), instruction.at(1).end(), temp);
                    temp[instruction.at(1).size()] = '\0';
                    
                    copy(instruction.at(2).begin(), instruction.at(2).end(), temp2);
                    temp2[instruction.at(2).size()] = '\0';
                    
                    copy(instruction.at(3).begin(), instruction.at(3).end(), temp3);
                    temp3[instruction.at(3).size()] = '\0';

                    fout << " " << getRegisterCode(temp2)  << " " << getRegisterCode(temp3) << " " << getRegisterCode(temp) << " 00000";
                    if(getRegisterCode(temp) == "111111" || getRegisterCode(temp2) == "111111" || getRegisterCode(temp3) == "111111")
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". Three valid register operands expected!" << endl;
                    }
                }
                else if(strcmp("rri", OPTAB.getOpcodeFormat(current_node)) == 0)
                {
                    copy(instruction.at(1).begin(), instruction.at(1).end(), temp);
                    temp[instruction.at(1).size()] = '\0';
                    
                    copy(instruction.at(2).begin(), instruction.at(2).end(), temp2);
                    temp2[instruction.at(2).size()] = '\0';
                    
                    temp4 = stoi(instruction.at(3));
                    if(temp4 < -32768 || temp4 > 32767)
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". Immediate operand out of range!" << endl;                        
                    }
                    if(temp4 == 0 && (strcmp("001010", (*current_node).code) == 0 || strcmp("001011", (*current_node).code) == 0))
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Logical error at line " << PC + 1 << ". Trying to divide with 0!" << endl;                        
                    }

                    fout << " " << getRegisterCode(temp2)  << " " << getRegisterCode(temp);
                    if(getRegisterCode(temp) == "111111" || getRegisterCode(temp2) == "111111")
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". Two valid register and one immediate operands expected!" << endl;
                    }

                    binary = intTobin(temp4);
                    fout << " ";
                    for(count = 0; count < 16; count++)
                    {
                        fout << binary[count];
                    }
                }
                else if(strcmp("rrl", OPTAB.getOpcodeFormat(current_node)) == 0)
                {
                    copy(instruction.at(1).begin(), instruction.at(1).end(), temp);
                    temp[instruction.at(1).size()] = '\0';
                    
                    copy(instruction.at(2).begin(), instruction.at(2).end(), temp2);
                    temp2[instruction.at(2).size()] = '\0';
                    
                    copy(instruction.at(3).begin(), instruction.at(3).end(), temp3);
                    temp3[instruction.at(3).size()] = '\0';

                    if(getRegisterCode(temp) == "111111" || getRegisterCode(temp2) == "111111")
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". Two valid register and one immediate operands expected!" << endl;
                    }
                    
                    if(getRegisterCode(temp3) == "111111")
                    {
                        temp4 = stoi(instruction.at(3));
                        if(temp4 < -32768 || temp4 > 32767)
                        {
                            ErrorCount++;
                            fout.close();
                            if(argc == 2)
                                fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                            else if(argc == 3)
                                fout.open(argv[2], ofstream::out | ofstream::trunc);
                            cout << "Syntax error at line " << PC + 1 << ". Immediate operand out of range!" << endl;                        
                        }

                        fout << " 00000 " << getRegisterCode(temp2) << " " << getRegisterCode(temp);

                        binary = shamtTobin(temp4);
                        fout << " ";
                        for(count = 0; count < 4; count++)
                        {
                            fout << binary[count];
                        }
                    }
                    else
                    {
                        fout << " " << getRegisterCode(temp3) << " " << getRegisterCode(temp2) << " " << getRegisterCode(temp) << " 00000";
                    }
                }
                else if(strcmp("rrb", OPTAB.getOpcodeFormat(current_node)) == 0)
                {
                    copy(instruction.at(1).begin(), instruction.at(1).end(), temp);
                    temp[instruction.at(1).size()] = '\0';
                    
                    copy(instruction.at(2).begin(), instruction.at(2).end(), temp2);
                    temp2[instruction.at(2).size()] = '\0';

                    copy(instruction.at(3).begin(), instruction.at(3).end(), temp3);
                    temp3[instruction.at(3).size()] = '\0';

                    if(getRegisterCode(temp) == "111111" || getRegisterCode(temp2) == "111111")
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". Two valid register and one immediate operands expected!" << endl;
                    }

                    fout << " " << getRegisterCode(temp) << " " << getRegisterCode(temp2);
                    
                    binary = SYMTAB.getAddressCode(temp3, 'b');
                    if(binary == "XXX")
                    {
                        ErrorCount++;
                        fout.close();
                        if(argc == 2)
                            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
                        else if(argc == 3)
                            fout.open(argv[2], ofstream::out | ofstream::trunc);
                        cout << "Syntax error at line " << PC + 1 << ". Undefined identifier (label)!" << endl;
                    }
                    fout << " ";
                    for(count = 0; count < 16; count++)
                    {
                        fout << binary[count];
                    }
                }
                
                int key = 0;
                for(int i = 0; (*current_node).func[i] != '\0'; i++)
                {
                    if((*current_node).func[i] == 'x')
                        key++;
                }
                if(key == 6)
                {   
                    fout << endl;
                    continue;
                }
                fout << " " << (*current_node).func;
                fout << endl;
            }
        }
    }
    else 
    {
    	cout << "Unable to open input file. " << endl << "Assembling Terminated" << endl;
    	return 0;
	}
    
    if(line != "END")
    {
        ErrorCount++;
        fout.close();
        if(argc == 2)
            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
        else if(argc == 3)
            fout.open(argv[2], ofstream::out | ofstream::trunc);
        cout << "Error occured while assembling at line " << PC + 1 << ". \"END\" lebel missing! Program might not terminate!" << endl;
    }

    if(ErrorCount != 0)
    {
        fout.close();
        if(argc == 2)
            fout.open("output_machine_code.txt", ofstream::out | ofstream::trunc);
        else if(argc == 3)
            fout.open(argv[2], ofstream::out | ofstream::trunc);
    }
    fout.close();

    /*cout << "Symbol Table:" << endl;
    FILE *symbol_table = fopen("symbol_table.txt", "w+");
    SYMTAB.print(symbol_table);
    fclose(symbol_table);*/

    return 0;
}

void tokenize(string const &str, const char delim, vector<std::string> &out)
{
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delim, end)) != string::npos)
	{
		end = str.find(delim, start);
		out.push_back(str.substr(start, end - start));
	}
}
   
char *addTobin(int input)
{
    string str = bitset<26>(input).to_string();
    char *writable = new char[str.size() + 1];
    copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0';
    return writable;
}

char *shamtTobin(int input)
{
    string str = bitset<5>(input).to_string();
    char *writable = new char[str.size() + 1];
    copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0';
    return writable;
}

char *intTobin(int input)
{
    string str = bitset<16>(input).to_string();
    char *writable = new char[str.size() + 1];
    copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0';
    return writable;
}

const char *getRegisterCode(char *temp)
{
    const char *s;

    if (strcmp(temp, "$zero") == 0 || strcmp(temp, "$0") == 0)
        s = "00000";
    else if (strcmp(temp, "$at") == 0 || strcmp(temp, "$1") == 0)
        s = "00001";
    else if (strcmp(temp, "$v0") == 0 || strcmp(temp, "$2") == 0)
        s = "00010";
    else if (strcmp(temp, "$v1") == 0 || strcmp(temp, "$3") == 0)
        s = "00011";
    else if (strcmp(temp, "$a0") == 0 || strcmp(temp, "$4") == 0)
        s = "00100";
    else if (strcmp(temp, "$a1") == 0 || strcmp(temp, "$5") == 0)
        s = "00101";
    else if (strcmp(temp, "$a2") == 0 || strcmp(temp, "$6") == 0)
        s = "00110";
    else if (strcmp(temp, "$a3") == 0 || strcmp(temp, "$7") == 0)
        s = "00111";
    else if (strcmp(temp, "$t0") == 0 || strcmp(temp, "$8") == 0)
        s = "01000";
    else if (strcmp(temp, "$t1") == 0 || strcmp(temp, "$9") == 0)
        s = "01001";
    else if (strcmp(temp, "$t2") == 0 || strcmp(temp, "$10") == 0)
        s = "01010";
    else if (strcmp(temp, "$t3") == 0 || strcmp(temp, "$11") == 0)
        s = "01011";
    else if (strcmp(temp, "$t4") == 0 || strcmp(temp, "$12") == 0)
        s = "01100";
    else if (strcmp(temp, "$t5") == 0 || strcmp(temp, "$13") == 0)
        s = "01101";
    else if (strcmp(temp, "$t6") == 0 || strcmp(temp, "$14") == 0)
        s = "01110";
    else if (strcmp(temp, "$t7") == 0 || strcmp(temp, "$15") == 0)
        s = "01111";
    else if (strcmp(temp, "$s0") == 0 || strcmp(temp, "$16") == 0)
        s = "10000";
    else if (strcmp(temp, "$s1") == 0 || strcmp(temp, "$17") == 0)
        s = "10001";
    else if (strcmp(temp, "$s2") == 0 || strcmp(temp, "$18") == 0)
        s = "10010";
    else if (strcmp(temp, "$s3") == 0 || strcmp(temp, "$19") == 0)
        s = "10011";
    else if (strcmp(temp, "$s4") == 0 || strcmp(temp, "$20") == 0)
        s = "10100";
    else if (strcmp(temp, "$s5") == 0 || strcmp(temp, "$21") == 0)
        s = "10101";
    else if (strcmp(temp, "$s6") == 0 || strcmp(temp, "$22") == 0)
        s = "10110";
    else if (strcmp(temp, "$s7") == 0 || strcmp(temp, "$23") == 0)
        s = "10111";
    else if (strcmp(temp, "$t8") == 0 || strcmp(temp, "$24") == 0)
        s = "11000";
    else if (strcmp(temp, "$t9") == 0 || strcmp(temp, "$25") == 0)
        s = "11001";
    else if (strcmp(temp, "$k0") == 0 || strcmp(temp, "$26") == 0)
        s = "1010";
    else if (strcmp(temp, "$k1") == 0 || strcmp(temp, "$27") == 0)
        s = "11011";
    else if (strcmp(temp, "$gp") == 0 || strcmp(temp, "$28") == 0)
        s = "11100";
    else if (strcmp(temp, "$sp") == 0 || strcmp(temp, "$29") == 0)
        s = "11101";
    else if (strcmp(temp, "$fp") == 0 || strcmp(temp, "$30") == 0)
        s = "11110";
    else if (strcmp(temp, "$ra") == 0 || strcmp(temp, "$31") == 0)
        s = "11111";
    else
        s = "111111";

    return s;
}
