#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dir.h>

char command_char,command_line[300],command_word[100][100];
int command_word_num = 0;
char *database;
char table[300] = "\0";
FILE *file;

char multi_query[20][500];
int multi_query_num = 0; 
int and_or = 0;//AND:1  OR:2

char not_in_value[10][300];
int not_in_num = 0;

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
void delete_all();//I:6
void delete_by_col();//I:7
void select_all_columns_all_rows();//I:8
void select_all_columns_by_column();//I:9
void select_several_columns(char *col_name,char *value);//I:8_9
void update(char *col_name,char *value);//I:10,11
void select_not_in();//I:9_not_in
void analyze_command_line(char *command_line,command_tree root);

int main(){
	
	printf("\n\t\t\tWelcome to mini-db!\n\n");
	printf("You can manage your database easily,remember following tips at first:\n\n");
	tips();//show tips when clients launch this application 
	printf("You can input 'SHOW TIPS;' to show these tips again,hope to have a good experience!\n\n");
	printf("<COMMAND>:");
	
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
	printf("\tCREATE TABLE tb_name (\n");
	printf("\tcol_name1 DATATYPE,\n");
	printf("\tcol_name2 DATATYPE,\n");
	printf("\t...\n");
	printf("\tcol_nameN DATATYPE\n");
	printf("\t);\n\n");
	printf("Select data from table:\n");
	printf("\tSELECT * FROM tb_name [WHERE col_name1 = ... AND col_name2 = ...] [GROUP BY col_name] [ORDER BY col_name [DESC]] [LIMIT...];\n");
	printf("\tSELECT col_name1 [,col_name2] [,col_name3] FROM tb_name [WHERE col_name1 = ... AND col_name2 = ...] [GROUP BY col_name] [ORDER BY col_name [DESC]] [LIMIT...];\n\n");
	printf("Insert data into table:\n");
	printf("\tINSERT INTO tb_name VALUES (value1,value2,...,valueN);\n");
	printf("\tINSERT INTO tb_name (col_name1,col_name2,[col_name...]) VALUES (value1,value2,[value...]);\n\n");
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
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"DELETE");
	node_array[i++] = q;//16
	node_array[0]->son[4] = q;
	node_array[0]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"FROM");
	node_array[i++] = q;//17
	node_array[16]->son[0] = q;
	node_array[16]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{tb_name}");
	q->interface_sign = 6;
	node_array[i++] = q;//18
	node_array[17]->son[0] = q;
	node_array[17]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"WHERE");
	node_array[i++]  =q;//19
	node_array[18]->son[0] = q;
	node_array[18]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{col_name}");
	node_array[i++] = q;//20
	node_array[19]->son[0] = q;
	node_array[19]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"=");
	node_array[i++] = q;//21
	node_array[20]->son[0] = q;
	node_array[20]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{value}");
	q->interface_sign = 7;
	node_array[i++] = q;//22
	node_array[21]->son[0] = q;
	node_array[21]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"SELECT");
	node_array[i++] = q;//23
	node_array[0]->son[5] = q;
	node_array[0]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"*");
	node_array[i++] = q;//24
	node_array[23]->son[0] = q;
	node_array[23]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{several_col_name}");
	node_array[i++] = q;//25
	node_array[23]->son[1] = q;
	node_array[23]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"FROM");
	node_array[i++] = q;//26
	node_array[24]->son[0] = q;
	node_array[24]->son_num++;
	node_array[25]->son[0] = q;
	node_array[25]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{tb_name}");
	q->interface_sign = 8;
	node_array[i++] = q;//27
	node_array[26]->son[0] = q;
	node_array[26]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"WHERE");
	node_array[i++] = q;//28
	node_array[27]->son[0] = q;
	node_array[27]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{col_name}");
	node_array[i++] = q;//29
	node_array[28]->son[0] = q;
	node_array[28]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"=");
	node_array[i++] = q;//30
	node_array[29]->son[0] = q;
	node_array[29]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{value}");
	q->interface_sign = 9;
	node_array[i++] = q;//31;
	node_array[30]->son[0] = q;
	node_array[30]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"UPDATE");
	node_array[i++] = q;//32
	node_array[0]->son[6] = q;
	node_array[0]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{tb_name}");
	node_array[i++] = q;//33
	node_array[32]->son[0] = q;
	node_array[32]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"SET");
	node_array[i++] = q;//34
	node_array[33]->son[0] = q;
	node_array[33]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{col_name}");
	node_array[i++] = q;//35
	node_array[34]->son[0] = q;
	node_array[34]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"=");
	node_array[i++] = q;//36
	node_array[35]->son[0] = q;
	node_array[35]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{value}");
	q->interface_sign = 10;
	node_array[i++] = q;//37
	node_array[36]->son[0] = q;
	node_array[36]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"WHERE");
	node_array[i++] = q;//38
	node_array[37]->son[0] = q;
	node_array[37]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{col_name}");
	node_array[i++] = q;//39
	node_array[38]->son[0] = q;
	node_array[38]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"=");
	node_array[i++] = q;//40
	node_array[39]->son[0] = q;
	node_array[39]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"{value}");
	q->interface_sign = 11;
	node_array[i++] = q;//41
	node_array[40]->son[0] = q;
	node_array[40]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"NOT");
	node_array[i++] = q;//42
	node_array[29]->son[1] = q;
	node_array[29]->son_num++;
	
	q = (command_tree)malloc(sizeof(node));
	strcpy(q->content,"IN");
	node_array[i++] = q;//43
	node_array[42]->son[0] = q;
	node_array[42]->son_num++;
	node_array[43]->son[0] = node_array[31];
	node_array[43]->son_num++;
	
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
	
	if(strcmp(command_word[0],"SELECT")==0&&strcmp(command_word[1],"*")!=0){//Select several columns...
		j=0;
		int cut_command_word_num = 0;
		for(j=0;command_word[1][j]!='\0';j++);
		command_word[1][j++] = '\t';
		for(i=2;strcmp(command_word[i],"FROM")!=0;i++){
			for(k=0;command_word[i][k]!='\0';k++){
				command_word[1][j++] = command_word[i][k];
			}
			command_word[1][j++] = '\t';
			cut_command_word_num++;
		}
		command_word[1][j-1] = '\0';
		
		k = 2;
		for(j=i;j<command_word_num;j++){
			strcpy(command_word[k++],command_word[j]);
		}
		command_word_num = command_word_num - cut_command_word_num;
	}
	
	if(strcmp(command_word[0],"UPDATE")==0&&command_word_num>6&&strcmp(command_word[6],"WHERE")!=0){//for UPDATE several columns 
		
		int update_cols_helper = 0,cut_cols_num = 0;
		i=6,j=0,k=0;
		while(1){
			if(strcmp(command_word[i],"WHERE")==0||i==command_word_num){
				break;
			}
			if(strcmp(command_word[i],"=")==0){
				update_cols_helper++;
				i++; 
				cut_cols_num++;
				continue;
			}
			if(update_cols_helper%2==0){//column
				for(j=0;command_word[3][j]!='\0';j++);
				command_word[3][j++] = '\t';
				for(k=0;command_word[i][k]!='\0';k++){
					command_word[3][j++] = command_word[i][k];
				}
				command_word[3][j++] = '\0';
			}else{//value
				for(j=0;command_word[5][j]!='\0';j++);
				command_word[5][j++] = '\t';
				for(k=0;command_word[i][k]!='\0';k++){
					command_word[5][j++] = command_word[i][k];
				}
				command_word[5][j++] = '\0';
				
				update_cols_helper++;
			}
			i++;
		}
		
		if(strcmp(command_word[command_word_num-4],"WHERE")==0){
			k = 6;
			for(i = command_word_num-4;i<command_word_num;i++){
				strcpy(command_word[k++],command_word[i]);
				
			}
		}
		
		command_word_num = command_word_num - cut_cols_num * 3;
	}
	
	if((strcmp(command_word[0],"SELECT")==0&&strcmp(command_word[command_word_num-4],"AND")==0)||(strcmp(command_word[0],"SELECT")==0&&strcmp(command_word[command_word_num-4],"OR")==0)){
		
		if(strcmp(command_word[command_word_num-4],"AND")==0){
			and_or = 1;//AND
		}
		else{
			and_or = 2;//OR
		}
		
		for(i=9;i<command_word_num;i++){
			if(strcmp(command_word[i],"=")==0||strcmp(command_word[i],"AND")==0||strcmp(command_word[i],"OR")==0){
				continue;
			}
			strcpy(multi_query[multi_query_num++],command_word[i]);
		}
	} 
	
	if(strcmp(command_word[0],"SELECT")==0&&command_word_num>6&&strcmp(command_word[6],"NOT")==0){//NOT IN (...)
		int k=0;
		for(i=8;i<command_word_num;i++){
			strcpy(not_in_value[not_in_num++],command_word[i]);
			k++;
		}
		command_word_num = command_word_num-k+1;
	}
	
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

void now_database(char *src){//for CREATE and USE database
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
	printf("<INFO>:Create %s successfully!\n\n<COMMAND>:",database);
}

void create_table(){//interface_sign:2
	int i = 0,j = 0;
	now_table(command_word[2]);
	FILE *file = fopen(table,"a+");
	char field_info[1024] = "\0";
	if(file==NULL){
		printf("<INFO>:Create %s table failed!\n\n<COMMAND>:",command_word[2]);
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
		fclose(file);
		
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
		fclose(file);
	}
	printf("\n\n<COMMAND>:");
}

void use_database(){//interface_sign:4
	now_database(command_word[1]);
	printf("<INFO>:Database %s selected!\n\n<COMMAND>:",database);
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
		printf("<ERROR>:Insert value failed!\n\n<COMMAND>:");
	}
	else{
		fputs(value,file);
		fflush(file);
		fclose(file);
		printf("<INFO>:Insert value successfully!\n\n<COMMAND>:");
	}
}

void delete_all(){//interface_sign:6

	now_table(command_word[2]);
	char true_table_name[300] = "\0";
	strcpy(true_table_name,table);
	
	file = fopen(table,"a+");
	char buffer[1024] = "\0";
	if(file==NULL){
		printf("<ERROR>:Delete failed!\n\n<COMMAND>:");
	}
	else{
		now_table("temp");
		FILE *temp = fopen(table,"a+");
		if(temp==NULL){
			printf("<ERROR>:Delete failed!\n\n<COMMAND>:");
		}
		else{
			fgets(buffer,1024,file);
			fputs(buffer,temp);
			fflush(temp);
			
			fclose(file);
			fclose(temp);
			
			remove(true_table_name);
			rename(table,true_table_name);
			
			printf("<INFO>:Delete all rows successfully!\n\n<COMMAND>:");
		}
	}
}

void delete_by_col(){//interface_sign:7

	now_table(command_word[2]);
	char true_table_name[300] = "\0";
	strcpy(true_table_name,table);
	file = fopen(table,"a+");
	char buffer[1024] = "\0";
	
	char field_word[12][50];
	int k = 0,i = 0,j = 0,field_separate_flag = 0;
	char delete_col_value[100] = "\0";
	int delete_num = 0;
	
	now_table("temp");
	FILE *temp = fopen(table,"a+");
	if(file==NULL||temp==NULL){
		printf("<ERROR>:Delete failed!\n\n<COMMAND>:");
	}
	else{
		fgets(buffer,1024,file);
		fputs(buffer,temp);

		for(k = 0;buffer[k]!='\n';k++){//get columns from database  -->  field_word[0,i]
			if(buffer[k]==' '){
				field_word[i][j++] = '\0';
				field_separate_flag++;
				continue;
			}
			if(buffer[k]=='\t'){
				field_separate_flag++;
				i++;
				j = 0;
				continue;
			}
			if(field_separate_flag%2==0){
				field_word[i][j++] = buffer[k];
			}
		}
		
		for(k = 0;k<=i;k++){
			if(strcmp(field_word[k],command_word[4])==0){
				break;
			}
		}
		
		while(fgets(buffer,1024,file)!=NULL){//start from the second row
			field_separate_flag = 0;
			j = 0;
			memset(delete_col_value,'\0',sizeof(delete_col_value));
			for(i = 0;buffer[i]!='\n';i++){
				if(buffer[i]=='\t'){
					field_separate_flag++;
					continue;
				}
				if(field_separate_flag == k){
					delete_col_value[j++] = buffer[i];
				}
			}
			delete_col_value[j++] = '\0';
			
			if(strcmp(delete_col_value,command_word[6])==0){
				delete_num++;
			}
			else{
				fputs(buffer,temp);
			}
		}
		fclose(file);
		fclose(temp);
		
		remove(true_table_name);
		rename(table,true_table_name);	
		
		if(delete_num>1){
			printf("<INFO>:Delete successfully!%d rows deleted!\n\n<COMMAND>:",delete_num);	
		}
		else{
			printf("<INFO>:Delete successfully!%d row deleted!\n\n<COMMAND>:",delete_num);	
		}
	}
}

void select_all_columns_all_rows(){//interface_sign:8
	now_table(command_word[3]);
	file = fopen(table,"a+");
	char buffer[1024] = "\0";
	
	char field_word[12][50];
	int k = 0,i = 0,j = 0,field_separate_flag = 0,select_num = 0;
	if(file==NULL){
		printf("<ERROR>:Query failed!\n\n<COMMAND>:");
	}
	else{
		fgets(buffer,1024,file);
		for(k = 0;buffer[k]!='\n';k++){//get columns from database  -->  field_word[0,i]
			if(buffer[k]==' '){
				field_word[i][j++] = '\0';
				field_separate_flag++;
				continue;
			}
			if(buffer[k]=='\t'){
				field_separate_flag++;
				i++;
				j = 0;
				continue;
			}
			if(field_separate_flag%2==0){
				field_word[i][j++] = buffer[k];
			}
		}
		for(k=0;k<=i;k++){
			printf("%s",field_word[k]);
			if(k<i){
				printf("\t");
			}
			else{
				printf("\n");
			} 
		}
		while(fgets(buffer,1024,file)!=NULL){
			select_num++;
			printf("%s",buffer);
		}
	}
	if(select_num>1){
		printf("<INFO>:Query ok!%d rows returned.\n\n<COMMAND>:",select_num);
	}
	else{
		printf("<INFO>:Query ok!%d row returned.\n\n<COMMAND>:",select_num);
	}
	fclose(file);
}

void select_all_columns_by_column(){//interface_sign:9
	now_table(command_word[3]);
	file = fopen(table,"a+");
	
	char buffer[1024] = "\0";
	char field_word[12][50];
	int k = 0,i = 0,j = 0,field_separate_flag = 0,select_num = 0;
	char table_value[12][300];
	int cols_num = 0;
	
	int multi_query_cols_number[12] = {0};
	int multi_query_cols_num = 0;
	
	if(file==NULL){
		printf("<ERROR>:Query failed!\n\n<COMMAND>:");
	}
	else{
		fgets(buffer,1024,file);
		for(k = 0;buffer[k]!='\n';k++){//get columns from database  -->  field_word[0,i]
			if(buffer[k]==' '){
				field_word[i][j++] = '\0';
				field_separate_flag++;
				cols_num++;
				continue;
			}
			if(buffer[k]=='\t'){
				field_separate_flag++;
				i++;
				j = 0;
				continue;
			}
			if(field_separate_flag%2==0){
				field_word[i][j++] = buffer[k];
			}
		}
		
		for(k=0;k<=i;k++){
			printf("%s",field_word[k]);
			if(k<i){
				printf("\t");
			}
			else{
				printf("\n");
			} 
		}
		
		for(k = 0;k<=i;k++){
			if(strcmp(field_word[k],command_word[5])==0){
				break;
			}
		}
		multi_query_cols_number[multi_query_cols_num++] = k;
		for(k=0;k<multi_query_num;k++){
			if(k%2==0){
				for(j=0;j<=i;j++){
					if(strcmp(multi_query[k],field_word[j])==0){
						multi_query_cols_number[multi_query_cols_num++] = j;
					}
				}
			}
		}
		
		while(fgets(buffer,1024,file)!=NULL){//start from the second row
			field_separate_flag = 0;
			j = 0;
			i=0;

			for(k = 0;buffer[k]!='\n';k++){
				if(buffer[k]=='\t'){
					table_value[i][j++] = '\0';
					i++;
					j=0;
					continue;
				}
				table_value[i][j++] = buffer[k];
			}
			table_value[i][j++] = '\0';
			
			int able_printf;
			if(and_or==1){//AND
				able_printf = 1;//able
				k = 0;
				for(i=0;i<multi_query_cols_num;i++){
					if(i==0){
						if(strcmp(table_value[multi_query_cols_number[i]],command_word[7])!=0){
							able_printf = 0;//unable
							break;
						}
					}
					else{
						k++;
						if(strcmp(table_value[multi_query_cols_number[i]],multi_query[k])!=0){
							able_printf = 0;//unable
							break;
						}
						else{
							k++;
						}
					}
				}
			}
			else{//OR
				able_printf = 0;//unable
				k=0;
				for(i=0;i<multi_query_cols_num;i++){
					if(i==0){
						if(strcmp(table_value[multi_query_cols_number[i]],command_word[7])==0){
							able_printf = 1;//able
							break;
						}
					}
					else{
						k++;
						if(strcmp(table_value[multi_query_cols_number[i]],multi_query[k])==0){
							able_printf = 1;//able
							break;
						}
						else{
							k++;
						}
					}
				}
			}
			
			if(able_printf==1){
				select_num++;
				printf("%s",buffer);
			}
		}
		if(select_num>1){
			printf("<INFO>:Query ok!%d rows returned.\n\n<COMMAND>:",select_num);
		}
		else{
			printf("<INFO>:Query ok!%d row returned.\n\n<COMMAND>:",select_num);
		}
		
		memset(multi_query,'\0',sizeof(multi_query));
		multi_query_num = 0;
		
		fclose(file);
	}
}

void select_several_columns(char *col_name,char *value){//interface_sign:8_9
	
	now_table(command_word[3]);
	file = fopen(table,"a+");
	char buffer[1024] = "\0";
	char field_word[12][50];
	int k = 0,i = 0,j = 0,field_separate_flag = 0;
	int require_query_column[12]={0};
	char cur_query_column[50] = "\0";
	int columns_num = 0,require_query_column_num = 0;
	char table_column_value[12][300];
	int select_column_order[12];
	select_column_order[0] = -1;
	int select_column_order_num = 0;
	int value_i = 0;
	int return_num = 0;
	
	if(file==NULL){
		printf("<ERROR>:Query failed!\n\n<COMMAND>:");
	} 
	else{
		fgets(buffer,1024,file);
		for(k = 0;buffer[k]!='\n';k++){//get columns from database  -->  field_word[0,i]
			if(buffer[k]==' '){
				field_word[i][j++] = '\0';
				field_separate_flag++;
				continue;
			}
			if(buffer[k]=='\t'){
				field_separate_flag++;
				i++;
				j = 0;
				columns_num++;
				continue;
			}
			if(field_separate_flag%2==0){
				field_word[i][j++] = buffer[k];
			}
		}
		
		printf("%s\n",command_word[1]);
		
		j=0,i=0,k=-1;
		while(1){
			k++;
			if(command_word[1][k]=='\t'||command_word[1][k]=='\0'){
				cur_query_column[j++] = '\0';
				for(i=0;i<columns_num+1;i++){
					if(strcmp(cur_query_column,field_word[i])==0){
						require_query_column[require_query_column_num++] = i;
					}
				}
				j=0;
				memset(cur_query_column,'\0',sizeof(cur_query_column));
				if(command_word[1][k]=='\0'){
					break;
				}
				continue;
			}
			cur_query_column[j++] = command_word[1][k];
		}
		
		if(strcmp(col_name,"NULL")!=0&&strcmp(value,"NULL")!=0){
			for(i=0;i<columns_num+1;i++){
				if(strcmp(field_word[i],col_name)==0){
					select_column_order[select_column_order_num++] = i;
				}
			}
			for(i=0;i<multi_query_num;i++){
				if(i%2==0){
					for(j=0;j<columns_num+1;j++){
						if(strcmp(multi_query[i],field_word[j])==0){
							select_column_order[select_column_order_num++] = j;
						}
					}
				}
			}
		}
		
		while(fgets(buffer,1024,file)!=NULL){//start from the second row

			field_separate_flag = 0;
			j = 0,k=0,i=0;
			value_i = 0;
			
			if(select_column_order[0]!=-1){
				for(k = 0;buffer[k]!='\n';k++){
					if(buffer[k]=='\t'){
						table_column_value[i][j++] = '\0';
						i++;
						j=0;
						continue;
					}
					table_column_value[i][j++] = buffer[k];
				}
				table_column_value[i][j++] = '\0';
			}
			
			int able_printf;
			if(and_or==1){//AND
				able_printf = 1;//able
				k = 0;
				for(i=0;i<select_column_order_num;i++){
					if(i==0){
						if(strcmp(table_column_value[select_column_order[i]],command_word[7])!=0){
							able_printf = 0;//unable
							break;
						}
					}
					else{
						k++;
						if(strcmp(table_column_value[select_column_order[i]],multi_query[k])!=0){
							able_printf = 0;//unable
							break;
						}
						else{
							k++;
						}
					}
				}
			}
			else{//OR
				able_printf = 0;//unable
				k=0;
				for(i=0;i<select_column_order_num;i++){
					if(i==0){
						if(strcmp(table_column_value[select_column_order[i]],command_word[7])==0){
							able_printf = 1;//able
							break;
						}
					}
					else{
						k++;
						if(strcmp(table_column_value[select_column_order[i]],multi_query[k])==0){
							able_printf = 1;//able
							break;
						}
						else{
							k++;
						}
					}
				}
			}
			
			field_separate_flag = 0,i=-1;
			while(1){
				
				i++;
				if(buffer[i]=='\t'){
					field_separate_flag++;
					continue;
				}
				
				if(buffer[i]=='\n'){
					if(select_column_order[0]!=-1&&able_printf==1||select_column_order[0]==-1){
						printf("\n");
						return_num++;
					}
					break;
				}

				for(j=0;j<require_query_column_num;j++){
					if(field_separate_flag==require_query_column[j]){
						if((select_column_order[0]!=-1&&able_printf==1)||select_column_order[0]==-1){
							printf("%c",buffer[i]);
							if(buffer[i+1]=='\t'){
								printf("\t");
							}
						}	
					}
				}
				
			}		
		}
		
		if(return_num>1){
			printf("<INFO>:Query ok!%d rows returned.\n\n<COMMAND>:",return_num);
		}
		else{
			printf("<INFO>:Query ok!%d row returned.\n\n<COMMAND>:",return_num);
		}
		
		memset(multi_query,'\0',sizeof(multi_query));
		multi_query_num = 0;
		fclose(file);
	} 
}

void update(char *col_name,char *value){//interface_sign:10,11

	now_table(command_word[1]);
	char origin_table[300] = "\0";
	strcpy(origin_table,table);
	file = fopen(table,"a+");
	char buffer[1024] = "\0";
	char field_line[1024] = "\0";
	
	int k = 0,i = 0,j = 0,field_separate_flag = 0;
	char field_word[12][50];
	
	int require_update_column[12]={0};
	char cur_update_column[50] = "\0";
	int columns_num = 0,require_update_column_num = 0;
	
	char origin_columns_value[12][300];
	char new_columns_value[12][300];
	char new_content[1024] = "\0";
	
	int where_column = -1;
	int need_update_of_where = 0;
	int update_rows_num = 0;
	now_table("temp");
	FILE *temp = fopen(table,"a+");
	if(file==NULL||temp==NULL){
		printf("<ERROR>:Update failed!\n\n<COMMAND>:");
	}
	else{
		fgets(buffer,1024,file);
		strcpy(field_line,buffer);
		fputs(field_line,temp);
		
		for(k = 0;buffer[k]!='\n';k++){//get columns from database  -->  field_word[0,i]
			if(buffer[k]==' '){
				field_word[i][j++] = '\0';
				if(strcmp(field_word[i],col_name)==0){
					where_column = i;
				}
				field_separate_flag++;
				continue;
			}
			if(buffer[k]=='\t'){
				field_separate_flag++;
				i++;
				j = 0;
				columns_num++;
				continue;
			}
			if(field_separate_flag%2==0){
				field_word[i][j++] = buffer[k];
			}
		}
		
		j=0,i=0,k=-1;
		while(1){
			k++;
			if(command_word[3][k]=='\t'||command_word[3][k]=='\0'){
				cur_update_column[j++] = '\0';
				for(i=0;i<columns_num+1;i++){
					if(strcmp(cur_update_column,field_word[i])==0){
						require_update_column[require_update_column_num++] = i;
					}
				}
				j=0;
				memset(cur_update_column,'\0',sizeof(cur_update_column));
				if(command_word[3][k]=='\0'){
					break;
				}
				continue;
			}
			cur_update_column[j++] = command_word[3][k];
		}	
		
		
		i=0,j=0;
		for(k=0;command_word[5][k]!='\0';k++){
			if(command_word[5][k]=='\t'){
				new_columns_value[i][j++] = '\0';
				i++;
				j=0;
				continue;
			}
			new_columns_value[i][j++] = command_word[5][k];
		}
		new_columns_value[i][j++] = '\0';
		
		while(fgets(buffer,1024,file)!=NULL){
			field_separate_flag = 0;
			k=0,j=0,i=0;
			memset(new_content,'\0',sizeof(new_content));
			memset(origin_columns_value,'\0',sizeof(origin_columns_value));
			need_update_of_where = 0;
			
			for(k=0;buffer[k]!='\n';k++){
				if(buffer[k]=='\t'){
					origin_columns_value[i][j++] = '\0';
					i++;
					j=0;
					continue;
				}
				origin_columns_value[i][j++] = buffer[k];
			}
			origin_columns_value[i][j++] = '\0';
			
			if(where_column!=-1&&strcmp(origin_columns_value[where_column],value)==0){
				need_update_of_where = 1;
			}
			
			j=0;
			for(i=0;i<require_update_column_num;i++){
				strcpy(origin_columns_value[require_update_column[i]],new_columns_value[j++]);
			}
			
			k=0,j=0;
			for(i = 0;i<columns_num+1;i++){
				for(j=0;origin_columns_value[i][j]!='\0';j++){
					new_content[k++] = origin_columns_value[i][j];
				}
				if(i<columns_num){
					new_content[k++] = '\t';
				}
				else{
					new_content[k++] = '\n';
				}
			}

			if(where_column==-1||need_update_of_where == 1){//no where
				update_rows_num++;
				fputs(new_content,temp);
			}
			else{
				fputs(buffer,temp);
			}
			
		}
		fclose(file);
		fclose(temp);
		
		remove(origin_table);
		rename(table,origin_table);
		
		if(update_rows_num>1){
			printf("<INFO>:Update ok!%d rows affected.\n\n<COMMAND>:",update_rows_num);
		}
		else{
			printf("<INFO>:Update ok!%d row affected.\n\n<COMMAND>:",update_rows_num);
		}
	}
}

void select_not_in(){//interface_sign:9_not_in
	
	now_table(command_word[3]);
	file = fopen(table,"a+");
	char buffer[1024] = "\0";
	char field_word[12][50];
	int k = 0,i = 0,j = 0,field_separate_flag = 0,cols_num = 0;
	char table_value[12][300];
	int return_num = 0;
	
	char cur_query_column[1024] = "\0";
	char require_query_column[12] = {0};
	int require_query_column_num = 0;
	
	if(file==NULL){
		printf("<ERROR>:Query failed!\n\n<COMMAND>:");
	}
	else{
		fgets(buffer,1024,file);
		for(k = 0;buffer[k]!='\n';k++){//get columns from database  -->  field_word[0,i]
			if(buffer[k]==' '){
				field_word[i][j++] = '\0';
				field_separate_flag++;
				cols_num++;
				continue;
			}
			if(buffer[k]=='\t'){
				field_separate_flag++;
				i++;
				j = 0;
				continue;
			}
			if(field_separate_flag%2==0){
				field_word[i][j++] = buffer[k];
			}
		}
		
		for(k=0;k<cols_num;k++){
			if(strcmp(field_word[k],command_word[5])==0){
				break;
			}
		}
		int judge_column = k;
		
		if(strcmp(command_word[1],"*")!=0){
			j=0,i=0,k=-1;
			while(1){
				k++;
				if(command_word[1][k]=='\t'||command_word[1][k]=='\0'){
					cur_query_column[j++] = '\0';
					for(i=0;i<cols_num;i++){
						if(strcmp(cur_query_column,field_word[i])==0){
							require_query_column[require_query_column_num++] = i;
						}
					}
					j=0;
					memset(cur_query_column,'\0',sizeof(cur_query_column));
					if(command_word[1][k]=='\0'){
						break;
					}
					continue;
				}
				cur_query_column[j++] = command_word[1][k];
			}
			for(k=0;k<require_query_column_num;k++){
				printf("%s",field_word[require_query_column[k]]);
				if(k<require_query_column_num-1){
					printf("\t");
				}
				else{
					printf("\n");
				} 
			}
		}
		else{
			for(k=0;k<cols_num;k++){
				printf("%s",field_word[k]);
				if(k<cols_num-1){
					printf("\t");
				}
				else{
					printf("\n");
				} 
			}
		}
		
		int able_printf;
		while(fgets(buffer,1024,file)!=NULL){
			
			i=0,j=0;
			for(k=0;buffer[k]!='\n';k++){
				if(buffer[k]=='\t'){
					table_value[i][j++] = '\0';
					i++;
					j=0;
					continue;
				}
				table_value[i][j++] = buffer[k];
			}
			table_value[i][j++] = '\0';
				
			able_printf = 1;//able
			for(k=0;k<not_in_num;k++){
				if(strcmp(table_value[judge_column],not_in_value[k])==0){
					able_printf = 0;
					break;
				}
			}
				
			if(able_printf==1){
				if(strcmp(command_word[1],"*")==0){
					return_num++;
					printf("%s",buffer);
				}
				else{
					return_num++;
					field_separate_flag = 0;
					for(k=0;buffer[k]!='\n';k++){
						if(buffer[k]=='\t'){
							field_separate_flag++;
							continue;
						}
						for(i = 0;i<require_query_column_num;i++){
							if(field_separate_flag==require_query_column[i]){
								printf("%c",buffer[k]);
								if(buffer[k+1]=='\t'){
									printf("\t");
								}
							}
						}
					}
					printf("\n");	
				}	
			}		
		}
		
		if(return_num>1){
			printf("<INFO>:Query ok!%d rows returned.\n\n<COMMAND>:",return_num);
		}
		else{
			printf("<INFO>:Query ok!%d row returned.\n\n<COMMAND>:",return_num);
		}
			
		memset(not_in_value,'\0',sizeof(not_in_value));
		not_in_num = 0;
		fclose(file);
	}
}

void analyze_command_line(char *command_line,command_tree root){//command line analyze module
	
	if(strcmp(command_line,"SHOW TIPS")==0){
		tips();
		printf("<COMMAND>:");
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
			if(analyze_q->interface_sign==6&&i==command_word_num-1){
				delete_all();
			}
			if(analyze_q->interface_sign==6&&i!=command_word_num-1){
				continue;
			}
			if(analyze_q->interface_sign==7){
				delete_by_col();
			}
			if(analyze_q->interface_sign==8&&strcmp(command_word[command_word_num-2],"FROM")==0){
				if(strcmp(command_word[1],"*")==0){
					select_all_columns_all_rows();
				}
				else{
					select_several_columns("NULL","NULL");
				}
			}
			if(analyze_q->interface_sign==8&&strcmp(command_word[command_word_num-2],"FROM")!=0){
				continue;
			}
			if(analyze_q->interface_sign==9&&strcmp(command_word[1],"*")==0){
				if(strcmp(command_word[command_word_num-2],"IN")!=0){
					select_all_columns_by_column();
				}
				else{
					select_not_in();
				}
			}
			if(analyze_q->interface_sign==9&&strcmp(command_word[1],"*")!=0){
				if(strcmp(command_word[command_word_num-2],"IN")!=0){
					select_several_columns(command_word[5],command_word[7]);
				}
				else{
					select_not_in();
				}
			}
			if(analyze_q->interface_sign==10&&strcmp(command_word[i+1],"WHERE")!=0){
				update("NULL","NULL");
			}
			if(analyze_q->interface_sign==10&&strcmp(command_word[i+1],"WHERE")==0){
				continue;
			}
			if(analyze_q->interface_sign==11){
				update(command_word[command_word_num-3],command_word[command_word_num-1]);
			}
			break;
		}
	}
}
