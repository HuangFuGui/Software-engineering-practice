#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dir.h>

char command_char,command_line[300],command_word[100][100];
int command_word_num = 0;
char *database;
char table[300] = "\0";
FILE *file;

typedef struct node{
	char content[300];
	struct node *son[10];
	int son_num = 0;
	int interface_sign = 0;
}node,*command_tree;
command_tree q,node_array[1000];

/*Interfaces*/
void tips();
void build_command_tree();
command_tree traverse_command_tree(command_tree father,char *cur_command_word);
void separate_command_by_space();
int max_width(char *one_line);
void now_database(char *src);
void now_table(char *src);
void create_database();//I:1
void create_table();//I:2
void desc_table(char *src);//I:3
void use_database();//I:4
void insert_values();//I:5
void analyze_command_line(char *command_line,command_tree root);

int main(){
	
	printf("\n\t\t\tWelcome to mini-db!\n\n");
	printf("You can manage your database easily,remember following tips at first:\n\n");
	tips();//show tips when clients launch this application 
	printf("You can input 'SHOW TIPS;' to show these tips again,hope to have a good experience!\n\n");
	
	build_command_tree();
	
	int input_i=0;//input module
	while(1){  
		scanf("%c",&command_char);
		if(command_char!=';'){
			command_line[input_i++] = command_char;
		}else{ //';'will not be in the command_line
			getchar(); //for Enter key
			command_line[input_i] = '\0';
			input_i=0;
			analyze_command_line(command_line,node_array[0]);
			
			memset(command_line,'\0',sizeof(command_line));
			memset(command_word,'\0',sizeof(command_word));
			command_word_num = 0;
		}
	}
}

/*Implements*/
void tips(){//tips 
	printf("Create database:\n");
	printf("\tCREATE DATABASE db_name;\n\n");
	printf("Choose database:\n");
	printf("\tUSE db_name;\n\n");
	printf("Show tables of the database:\n");
	printf("\tSHOW TABLES;\n\n");
	printf("Create and define table:\n");
	printf("\tCREATE TABLE tb_name(\n");
	printf("\t\tcol_name1 DATATYPE[(LENGTH)],\n");
	printf("\t\tcol_name2 DATATYPE[(LENGTH)],\n");
	printf("\t\t...\n");
	printf("\t\tcol_nameN DATATYPE[(LENGTH)]\n");
	printf("\t\t);\n\n");
	printf("Select data from table:\n");
	printf("\tSELECT * FROM tb_name [WHERE col_name1 = ... AND col_name2 = ...] [GROUP BY col_name] [ORDER BY col_name [DESC]] [LIMIT...];\n");
	printf("\tSELECT col_name1 [,col_name2] [,col_name3] FROM tb_name [WHERE col_name1 = ... AND col_name2 = ...] [GROUP BY col_name] [ORDER BY col_name [DESC]] [LIMIT...];\n\n");
	printf("Insert data into table:\n");
	printf("\tINSERT [INTO] tb_name VALUES(value1,value2,...,valueN);\n");
	printf("\tINSERT [INTO] tb_name(col_name1,col_name2,[col_name...]) VALUES(value1,value2,[value...]);\n\n");
	printf("Update data:\n");
	printf("\tUPDATE tb_name SET col_name1 = value1 [,col_name2 = value2] [,col_name3 = value3] [WHERE] [col_name = value];\n\n");	
	printf("Delete data:\n");
	printf("\tDELETE FROM tb_name [WHERE col_name = value];\n\n");
	printf("Create index for column:\n");
	printf("\tCREATE INDEX idx_name ON tb_name (col_name);\n\n");
}

void build_command_tree(){
	
	int i =0;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"root");
	node_array[i++] = q;//0 
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"CREATE");
	node_array[i++] = q;//1
	node_array[0]->son[0] = q;
	node_array[0]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"DATABASE");
	node_array[i++] = q;//2
	node_array[1]->son[0] = q;
	node_array[1]->son_num++;

	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{db_name}");
	q->interface_sign = 1;
	node_array[i++] = q;//3
	node_array[2]->son[0] = q;
	node_array[2]->son_num++;

	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"TABLE");
	node_array[i++] = q;//4
	node_array[1]->son[1] = q;
	node_array[1]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{tb_name}");
	node_array[i++] = q;//5
	node_array[4]->son[0] = q;
	node_array[4]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{tb_defination}");
	q->interface_sign = 2;
	node_array[i++] = q;//6;
	node_array[5]->son[0] = q;
	node_array[5]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"DESC");
	node_array[i++] = q;//7
	node_array[0]->son[1] = q;
	node_array[0]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{tb_name}");
	q->interface_sign = 3;
	node_array[i++] = q;//8
	node_array[7]->son[0] = q;
	node_array[7]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"USE");
	node_array[i++] = q;//9
	node_array[0]->son[2] = q;
	node_array[0]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{db_name}");
	q->interface_sign = 4;
	node_array[i++] = q;//10
	node_array[9]->son[0] = q;
	node_array[9]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"INSERT");
	node_array[i++] = q;//11
	node_array[0]->son[3] = q;
	node_array[0]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"INTO");
	node_array[i++] = q;//12
	node_array[11]->son[0] = q;
	node_array[11]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{tb_name}");
	node_array[i++] = q;//13
	node_array[12]->son[0] = q;
	node_array[12]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"VALUES");
	node_array[i++] = q;//14
	node_array[13]->son[0] = q;
	node_array[13]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{insert_values}");
	q->interface_sign = 5;
	node_array[i++] = q;//15
	node_array[14]->son[0] = q;
	node_array[14]->son_num++;
}

command_tree traverse_command_tree(command_tree father,char *cur_command_word){
	
	int i=0;
	for(i=0;i<father->son_num;i++){
		q = father->son[i];
		if(q->content[0]=='{'||strcmp(q->content,cur_command_word)==0){
			return q;
		}
	}
	return NULL;
}

void separate_command_by_space(){//command_word[0,i]
	int k=0,i=0,j=0;
	int comma_flag = 0,space_flag=0; 
	int first_quote_flag = 0; 
	for(k=0;command_line[k]!='\0';k++){
		if(command_line[k]=='\''&&comma_flag==0){//for comma and space in '...'
			comma_flag = 1;
			space_flag = 1;
			continue;
		}
		if(comma_flag==1&&(command_line[k]==','||command_line[k]==' ')){
			command_word[i][j++] = command_line[k];
			continue;
		}
		if(comma_flag==1&&command_line[k]=='\''){
			comma_flag = 0;
			space_flag = 0;
			continue;
		}
		
		if(command_line[k]=='('||command_line[k]==')'||command_line[k]=='\n'){
			continue;
		}
		if(command_line[k]!=' '&&command_line[k]!=','){
			command_word[i][j++] = command_line[k];
		}
		if(command_line[k]==' '||command_line[k]==','){
			command_word[i][j] = '\0';
			i++;
			j=0;
		}
	}
	command_word_num = i+1;
	
	/*for(i=0;i<command_word_num;i++){
		printf("command_word: %s\n",command_word[i]);
	}*/
}

int max_width(char *one_line){
	int i = 0,max = -1,cur_max = 0;
	while(1){
		if(one_line[i]!='\t'&&one_line[i]!=' '&&one_line[i]!='\n'){
			cur_max++;
		}
		if(one_line[i]=='\t'||one_line[i]==' '){
			if(cur_max>max){
				max = cur_max;
			}
			cur_max = 0;
		}
		if(one_line[i] =='\n'){
			if(cur_max>max){
				max = cur_max;
			}
			break;
		}
		i++;
	}
	return max;
}

void now_database(char *src){//for USE and change database
	int i=0;
	char fade_database[100] = "\0";
	for(i=0;src[i]!='\0';i++){
		fade_database[i] = src[i];
	}
	fade_database[i] = '\0';
	database = new char[i];
	strcpy(database,fade_database);
}

void now_table(char *src){//table to use now 
	
	memset(table,'\0',sizeof(table));
	
	int i = 0;
	for(i=0;database[i]!='\0';i++){
		table[i] = database[i];
	}
	table[i] = '/';
	
	int j = i + 1;
	for(i=0;src[i]!='\0';i++){
		table[j++] = src[i];
	}
	table[j] = '.';
	table[j+1] = 't';
	table[j+2] = 'x';
	table[j+3] = 't';
	table[j+4] = '\0';
}

void create_database(){//interface_sign:1
	mkdir(command_word[2]);
	now_database(command_word[2]);
	printf("<INFO>:Create %s successfully!\n\n",database);
}

void create_table(){//interface_sign:2
	int i = 0,j = 0;
	now_table(command_word[2]);
	FILE *file = fopen(table,"a+");
	char field_info[1024] = "\0";
	if(file==NULL){
		printf("<INFO>:Create %s table failed!\n\n",command_word[2]);
	}
	else{
		printf("<INFO>:Create %s table successfully!\n",command_word[2]); 
		int k=0;
		i=3;
		j=0;
		while(1){
			if(command_word[i][j]=='\0'){
				i++;
				if(i==command_word_num){
					break;
				}
				j=0;
				if(i%2==0){
					field_info[k++] = ' ';
				}
				else{
					field_info[k++] = '\t';
				}
				continue;
			}
			field_info[k++] = command_word[i][j++];
		}
		field_info[k] = '\n';
		fputs(field_info,file);
		fflush(file);//synchronizes an output stream with the actual file    or fclose(file);
		
		desc_table(command_word[2]);
	}
}

void desc_table(char *src){//interface_sign:3
	int i = 0,j = 0;
	now_table(src);
	FILE *file = fopen(table,"a+");
	char buffer[1024] = "\0";
	
	if(file!=NULL){
		fgets(buffer,1024,file);
		
		int cur_max_width = max_width(buffer);
		int cur_printf_length = 0;
		for(i=0;buffer[i]!='\n';i++){
			if(buffer[i]==' '){
				if(cur_printf_length<cur_max_width){
					for(j = 1;j<=cur_max_width-cur_printf_length;j++){
						printf(" ");
					}
				}
				printf("  |  ");
				cur_printf_length = 0;
				continue;
			}
			if(buffer[i]=='\t'){
				printf("\n");
				cur_printf_length = 0;
				continue;
			}
			printf("%c",buffer[i]);
			cur_printf_length++;
		}
	}
	printf("\n\n");
}

void use_database(){//interface_sign:4
	now_database(command_word[1]);
	printf("<INFO>:Database %s selected!\n\n",database);
}

void insert_values(){//interface_sign:5
	
	now_table(command_word[2]);
	
	int i = 0,j = 0,k = 0;
	char value[2048] = "\0";
	for(i = 4;i<command_word_num;i++){
		for(j = 0;command_word[i][j]!='\0';j++){
			value[k++] = command_word[i][j];
		}
		if(i<command_word_num-1){
			value[k++] = '\t';
		}
		else{
			value[k++] = '\n';// ***
		}
	}
	file = fopen(table,"a+");
	if(file==NULL){
		printf("<ERROR>:Insert value failed!\n\n");
	}
	else{
		fputs(value,file);
		fflush(file);
		printf("<INFO>:Insert value successfully!\n\n");
	}
}

void analyze_command_line(char *command_line,command_tree root){//command line analyze module
	
	if(strcmp(command_line,"SHOW TIPS")==0){
		tips();
		return;
	}
	
	int i=0;
	command_tree analyze_q = root;
	
	separate_command_by_space();
	
	for(i=0;i<command_word_num;i++){
		analyze_q = traverse_command_tree(analyze_q,command_word[i]);
		if(analyze_q!=NULL&&analyze_q->interface_sign==0){
			continue;
		}
		else if(analyze_q==NULL){
			printf("Error SQL!\n");
			break;
		}
		if(analyze_q->interface_sign>0){
			if(analyze_q->interface_sign==1){
				create_database();
			}
			if(analyze_q->interface_sign==2){
				create_table();
			}
			if(analyze_q->interface_sign==3){
				desc_table(command_word[1]);
			}
			if(analyze_q->interface_sign==4){
				use_database();
			}
			if(analyze_q->interface_sign==5){
				insert_values();
			}
			break;
		}
	}
}
