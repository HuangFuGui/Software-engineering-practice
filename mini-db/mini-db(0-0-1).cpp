#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dir.h>

void tips();
char command_char,command_line[300],command_word[100][100],command_parameter[20][50];
int command_word_num = 0,command_parameter_num = 0;
void analyze_command_line(char *command_line); 
char *database;
char table[100];
FILE *file;
char field_info[1024] = "\0";

typedef struct node{
	char content[300];
	struct node *son[8];
	int son_num = 0;
	int interface_sign = 0;
}node,*command_tree;
command_tree q,node_array[1000];


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
	
}

command_tree traverse_command_tree(command_tree father,char *cur_command_word){
	
	int i=0;
	for(i=0;i<father->son_num;i++){
		q = father->son[i];
		if(q->content[0]=='{'){
			/*printf("%s\n",cur_command_word);*/
			strcpy(command_parameter[command_parameter_num++],cur_command_word);
			return q;
		}
		if(strcmp(q->content,cur_command_word)==0){
			/*printf("%s\n",q->content);*/
			return q;
		}
	}
	return NULL;
}

void separate_command_by_space(){//command_word[0,i]
	int k=0,i=0,j=0;
	for(k=0;command_line[k]!='\0';k++){
		if(command_line[k]=='('||command_line[k]==')'||command_line[k]=='\n'){
			continue;
		}
		if(command_line[k]!=' '&&command_line[k]!=','){
			command_word[i][j++] = command_line[k];
		}
		else if(command_line[k]==' '||command_line[k]==','){
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
		if(one_line[i]!='\t'&&one_line[i]!=' '&&one_line[i]!='\0'){
			cur_max++;
		}
		if(one_line[i]=='\t'||one_line[i]==' '){
			if(cur_max>max){
				max = cur_max;
			}
			cur_max = 0;
		}
		if(one_line[i] =='\0'){
			if(cur_max>max){
				max = cur_max;
			}
			break;
		}
		i++;
	}
	return max;
}
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


void create_database(){//interface_sign:1

	mkdir(command_parameter[0]);
	
	int i=0;
	char fade_database[100] = "\0";
	for(i=0;command_parameter[0][i]!='\0';i++){
		fade_database[i] = command_parameter[0][i];
	}
	fade_database[i] = '\0';
	database = new char[i];
	strcpy(database,fade_database);
	
	printf("<INFO>:Create database successfully!\n\n");
}

void desc_table(char *tb_name){//interface_sign:3
	int i = 0;
	char table_road[1024] = "\0";
	
	for(i=0;database[i]!='\0';i++){
		table_road[i] = database[i];
	}
	table_road[i] = '/';
	
	int j = i + 1;
	for(i=0;tb_name[i]!='\0';i++){
		table_road[j++] = tb_name[i];
	}
	table_road[j] = '.';
	table_road[j+1] = 't';
	table_road[j+2] = 'x';
	table_road[j+3] = 't';
	table_road[j+4] = '\0';
	
	FILE *file = fopen(table_road,"a+");
	char buffer[1024] = "\0";
	if(file!=NULL){
		
		fgets(buffer,1024,file);
		
		int cur_max_width = max_width(buffer);
		int cur_printf_length = 0;
		for(i=0;buffer[i]!='\0';i++){
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

void create_table(){//interface_sign:2

	strcpy(table,command_parameter[0]);
	
	int i = 0;
	char table_road[1024] = "\0";
	
	for(i=0;database[i]!='\0';i++){
		table_road[i] = database[i];
	}
	table_road[i] = '/';
	
	int j = i + 1;
	for(i=0;table[i]!='\0';i++){
		table_road[j++] = table[i];
	}
	table_road[j] = '.';
	table_road[j+1] = 't';
	table_road[j+2] = 'x';
	table_road[j+3] = 't';
	table_road[j+4] = '\0';
	
	FILE *file = fopen(table_road,"a+");
	if(file==NULL){
		printf("<INFO>:Create table failed!\n\n");
	}
	else{
		printf("<INFO>:Create table successfully!\n"); 
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
		field_info[k] = '\0';
		fputs(field_info,file);
		fflush(file);//synchronizes an output stream with the actual file    or fclose(file);
		
		desc_table(table);
	}
}

void use_database(){//interface_sign:4
	int i=0;
	char fade_database[100] = "\0";
	for(i=0;command_parameter[0][i]!='\0';i++){
		fade_database[i] = command_parameter[0][i];
	}
	fade_database[i] = '\0';
	
	database = new char[i];
	strcpy(database,fade_database);
	printf("\n");
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
				desc_table(command_parameter[0]);
			}
			if(analyze_q->interface_sign==4){
				use_database();
			}
			break;
		}
	}
}


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
			memset(command_parameter,'\0',sizeof(command_parameter));
			command_word_num = 0,command_parameter_num = 0;
		}
	}
}
