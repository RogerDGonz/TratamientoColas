#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Estaciones {
	char id_estacion[50]; //Identificacion de la estacion
	int capacidad; //Cantidad de personas que puede atender al mismo tiempo
	char tipo[5]; //El tipo de estacion (FIFO,Prioridad.Round Robin)
	int duracion_turno; //Tiempo de duracion para Round Robin
	int atendiendo;
	int atendidos;//Cantidad de personas atendidas
	int oscio;//Cantidad de tiempo sin hacer nada
	int col;//suma de clientes en cola por segundo
}estacion;
//BREAK POINT */ AVANZAR UNA POR UNA linea de codigo 
typedef struct Clientes {
	char id_cliente[50]; //nombre del cliente
	int prioridad; //valor entero usado en colas
	int tiempo_llegada; //instante en que llega el cliente al sistema
	int  id_estacion[50]; //Identificacion de la estacion
	int duracion[50]; //Tiempo a pasar en una estacion
	int CantEstaciones;//cantidad de estaciones que va el cliente 
	int EstacionActual;//estacion de se encuentra el cliente 
	int RR;
	int total_sis;//Tiempo total que paso en el sistema
	float total_col;//Tiempo total que paso en colas
	int total_est;//Tiempo total que paso en la estacion(siendo atendido)
	int salida;//instante en que salio del sistema
	struct Clientes *Next;//puntero a tipo ciente 
}cliente;


FILE *AuxEst,*AuxCli,*Est,*Cli;

cliente *PonerenCola(char* tipo,cliente *persona,cliente **ColaActual){
	persona->Next=NULL;
	if(!(*ColaActual))
		return persona;
	cliente *sig=*ColaActual;
	if(*tipo!='P'){
		for(sig;sig->Next;sig=sig->Next);
			sig->Next=persona;
	}
	else{
		cliente *ant; 		
		for(sig;sig;sig=sig->Next){
			if(sig->prioridad<persona->prioridad){
				if(!ant){
					persona->Next=sig;
					return persona;
				}
				ant->Next=persona;
				persona->Next=sig;
				return *ColaActual;		
			}	
			ant=sig;		
		}
		if(!sig)
			ant->Next=persona;
	}
	return *ColaActual;
}
cliente *PonerEnAtendidos(cliente *persona,cliente **ColaActual, int * atendiendo){
	persona->Next=NULL;
	if(!(*ColaActual)){
		*atendiendo=*atendiendo+1;
		return persona;
	}
	cliente *sig=*ColaActual;
	cliente *ant; 		
	for(sig;sig->Next;sig=sig->Next);
			sig->Next=persona;
	*atendiendo=*atendiendo+1;
	return *ColaActual;
}	

void ImprimirEstaciones(estacion Esta[],int NEst){
	int x,y,*z;
	for(x=0;x<NEst;x++){
		printf("%s, %d, %s, %d,%d\n",Esta[x].id_estacion,Esta[x].capacidad,Esta[x].tipo,Esta[x].duracion_turno,Esta[x].atendiendo);
	}
}
void ImprimirClientes(int NEst,estacion Esta[],cliente Cli[],int NCli){
	int x,y,z;
	for(x=0;x<NCli;x++){
		printf("\n");
		printf("%s,P%d,TLL%d, ",Cli[x].id_cliente,Cli[x].prioridad,Cli[x].tiempo_llegada);
		for(y=0;y<Cli[x].CantEstaciones;y++){
			printf("%s",Esta[(Cli[x].id_estacion[y])].id_estacion);
			printf(":%d,",Cli[x].duracion[y]);
		}
		printf("cantidadDeEstaciones: %d ",Cli[x].CantEstaciones);
		printf(",Estacion Actual:%d, ",Cli[x].EstacionActual);
		if(!Cli[x].Next)
			printf("Next=NULL;\n");
	}
}

int CompararCadenas(char *c1,char *c2){
	while(*c1!='\0'){
		if(*c1!=*c2)
			return 0;
		c1++;
		c2++;
	}
	if(*c2=='\0')
		return 1;
	return 0;
}
int BuscarEstacion(int NEst,estacion Esta[],char * Estcliente){
	int x;
	for(x=0;x<NEst;x++){
		if(CompararCadenas(Esta[x].id_estacion,Estcliente))
			return x;	
	}
//	printf("\nNo se Encuentra la estacion %s, Proceso Abortado\n",Estcliente);
//	exit(1);
	return -1;

}

void ImprimirCola(int x,estacion EstacionActual[],cliente *(ColaActual[]),cliente * (Atendidos[])){//1
	int i;
	for(i=0;i<x;i++){//2
	printf("\n\t\t\tEstacion: %s  Politica: %s  Capacidad: %d\nCola:", EstacionActual[i].id_estacion,EstacionActual[i].tipo,EstacionActual[i].capacidad);
	cliente *aux;
	for(aux=ColaActual[i];aux;aux=aux->Next){
		if(aux->Next)
			if(EstacionActual[i].tipo[0]=='P'){
				printf("%s P:",(*aux).id_cliente);
				printf("%c[%d;%d;%dm%d",0x1B,1,31,40,(*aux).prioridad);
				printf("%c[%d;%d;%dm ->",0x1B,1,37,40);
			}else
				printf("%s ->",(*aux).id_cliente);
		else
			if(EstacionActual[i].tipo[0]=='P'){
				printf("%s P:",(*aux).id_cliente);
				printf("%c[%d;%d;%dm%d",0x1B,1,31,40,(*aux).prioridad);
				printf("%c[%d;%d;%dm",0x1B,1,37,40);
			}else
				printf("%s",(*aux).id_cliente);	
		EstacionActual[i].col++;
		(*aux).total_col++;
	}
	printf("\nAtendiendo:");
	if(EstacionActual[i].tipo[0]!='R'){//3
		for(aux=Atendidos[i];aux;aux=aux->Next)
			if(aux->Next){
				printf("%s TF:",(*aux).id_cliente);
				printf("%c[%d;%d;%dm%d",0x1B,1,31,40,aux->duracion[aux->EstacionActual]);
				printf("%c[%d;%d;%dm|",0x1B,1,37,40);
			}else{
				printf("%s TF:",(*aux).id_cliente);
				printf("%c[%d;%d;%dm%d",0x1B,1,31,40,aux->duracion[aux->EstacionActual]);
				printf("%c[%d;%d;%dm",0x1B,1,37,40);
			}
	}else{//-3//4
		for(aux=Atendidos[i];aux;aux=aux->Next)
			if(aux->Next){
				printf("%s TF:",(*aux).id_cliente);
				printf("%c[%d;%d;%dm%d",0x1B,1,31,40,aux->duracion[aux->EstacionActual]);
				printf("%c[%d;%d;%dm RR:",0x1B,1,37,40);
				printf("%c[%d;%d;%dm%d",0x1B,1,31,40,(*aux).RR);
				printf("%c[%d;%d;%dm|",0x1B,1,37,40);
			}else{
				printf("%s TF:",(*aux).id_cliente);
				printf("%c[%d;%d;%dm%d",0x1B,1,31,40,aux->duracion[aux->EstacionActual]);
				printf("%c[%d;%d;%dm RR:",0x1B,1,37,40);
				printf("%c[%d;%d;%dm%d",0x1B,1,31,40,(*aux).RR);
				printf("%c[%d;%d;%dm",0x1B,1,37,40);
			}
		}//-4
	printf("\n");
	}
	printf("\n");
}//-1
cliente *PonerEnTiempo(cliente *Llegada,cliente *persona ){
	if(!Llegada)
		return persona;
	cliente *sig=Llegada,*ant=NULL;
	for(sig;sig;sig=sig->Next){
		if(sig->tiempo_llegada>persona->tiempo_llegada){
			if(!ant){
				persona->Next=sig;
				return persona;
			}
			ant->Next=persona;
			persona->Next=sig;
			return Llegada;		
		}	
		ant=sig;		
	}
	ant->Next=persona;
	return Llegada;	
}
cliente * PonerAlFinal(cliente*reubicar, cliente * persona ){
	if(!(reubicar))
		return persona;
	cliente 	*aux=reubicar;
	for(;aux->Next;aux=aux->Next);
		aux->Next=persona;
		return reubicar;	
}
void PasarTiempo(int t,int N, cliente*(atendidos[]),cliente** reubicar,estacion Estaciones[]){
	int x;
	for(x=0;x<N;x++){
		if(atendidos[x]){
			cliente*aux=atendidos[x];
			for(;aux;aux=aux->Next){
				aux->duracion[aux->EstacionActual]--;
				aux->RR--;
			}
		}else{
			Estaciones[x].oscio++;
		}
	}
	for(x=0;x<N;x++){
		if(atendidos[x]){
			cliente *ant=NULL,*aux=atendidos[x],*sig;
			for(;aux;){
				sig=aux->Next;
				(*aux).total_est++;
				if(aux->duracion[aux->EstacionActual]==0){
					if(!ant)
						atendidos[x]=sig;			
					else
						ant->Next=sig;
					aux->EstacionActual++;
					aux->Next=NULL;
					if(aux->EstacionActual!=aux->CantEstaciones){					
						*reubicar=PonerAlFinal(*reubicar,aux);
					}else{
						(*aux).salida=t;
						(*aux).total_sis=t-(*aux).tiempo_llegada;
					}
					Estaciones[x].atendiendo--;
					Estaciones[x].atendidos++;
					aux->RR=-1;
					aux=NULL;
				}
				if(aux && aux->RR==0){
					printf(" N:%s ",aux->id_cliente);
					if(!ant)
						atendidos[x]=sig;			
					else
						ant->Next=sig;
					aux->Next=NULL;				
					*reubicar=PonerAlFinal(*reubicar,aux);
					Estaciones[x].atendiendo--;
					aux->RR=-1;
					aux=NULL;
				}
				ant=aux;
				aux=sig;
			}
		}
	}
}
	
int FinSimulacion(cliente Clientes[],int N){
	int x;
	for(x=0;x<N;x++)
		if(Clientes[x].EstacionActual!=Clientes[x].CantEstaciones)
			return 1;
	return 0;
}

void archivo(FILE *info,int x,int y,int z,float w,estacion esta[],cliente clien[]){
	int i;
	float proms=0.00,promc=0.00;
	info=fopen("Informe.text","w");
	fprintf(info,"Tiempo total de la simulacion: %d\n\n",(x-1));
	fprintf(info,"\tESTACIONES\n\n");
	for(i=0;i<y;i++){
		fprintf(info,"Estacion: %s\n",esta[i].id_estacion);
		fprintf(info,"Tiempo total de uso: %d\n",(x-esta[i].oscio-1));
		fprintf(info,"Tiempo de ocio: %d\n",(esta[i].oscio-1));
		fprintf(info,"Cantidad de clientes atendidos: %d\n",esta[i].atendidos);
		fprintf(info,"promedio de clientes en cola: %.3f\n\n",(esta[i].col/(w-1)));
	}
	fprintf(info,"\n\tCLIENTES\n\n");
	for(i=0;i<z;i++){
		fprintf(info,"Cliente: %s\n",clien[i].id_cliente);
		fprintf(info,"Tiempo total en el sistema: %d\n",clien[i].total_sis);
		fprintf(info,"Tiempo total en colas: %.0f\n",clien[i].total_col);
		fprintf(info,"Tiempo total en estaciones: %d\n",clien[i].total_est);
		fprintf(info,"Tiempo de llegada al sistema: %d\n",clien[i].tiempo_llegada);
		fprintf(info,"Tiempo de salida del sistema: %d\n\n",clien[i].salida);
		proms=proms+clien[i].total_sis;
		promc=promc+clien[i].total_col;
	}
	fprintf(info,"\nTiempo promedio de un cliente en el sistema: %.3f\n",(proms/z));
	fprintf(info,"Tiempo promedio de un cliente en cola: %.3f\n",(promc/z));
	fclose(info);

}


int main (){	
	printf("%c[%d;%d;%dm\n\n\n",0x1B,1,37,40);
	system("clear");
	char e[50],cl[50];
	int tiempo;
	printf("Ingrese el nombre del archivo de estaciones(ejemplo: archivo.conf):\n");
	scanf("%s",e);
	 if((Est=fopen(e,"r")) == NULL){		
		printf("ERROR al abrir el archivo de estaciones.conf\n");
		return 1;
	}
	printf("Ingrese el nombre del archivo de clientes(ejemplo: archivo.conf):\n");
	scanf("%s",cl);
	 if((Cli=fopen(cl,"r")) == NULL){
		printf("ERROR al abrir el archivo de clientes.conf\n");
		return 1;
	}
	printf("Ingrese la velocidad del tiempo en milisegundos(1000milisegundos = 1segundo):\n");
	scanf("%d",&tiempo);
	
	char c,buffer[250];
	int i,f=0,NEst=0,NCli=0;
	AuxEst=fopen("estaciones.conf","r");
	while((c=fgetc(AuxEst))!=EOF){
			if(c=='\n')
				NEst++;
	}
	fclose(AuxEst);
	printf("cantidad de Estaciones: %d",NEst);	
	estacion VarEstaciones[NEst];
	NEst=0;
	//strcpy(reg.enTexto, &buffer[i]);
	printf("\tEstaciones: \n");
	while((c=fgetc(Est))!=EOF){
		if(c!='\n'){
			if(c!=' ')
				buffer[f++]=c; // esta copiando una linea del archivo a buffer (caracter a caracter menos los espacios)
		}
		else{
			buffer[f]='\0';
			for(i=0;buffer[i]!='-';i++);
			buffer[i++]='\0';
			strcpy(VarEstaciones[NEst].id_estacion,buffer);
			f=i;
			for(i;buffer[i]!='-';i++);
			buffer[i++]='\0';
			 VarEstaciones[NEst].capacidad=(int)atof(&buffer[f]);
			f=i;
			for(i;buffer[i]!='-';i++);
			buffer[i++]='\0';
			strcpy(VarEstaciones[NEst].tipo,&buffer[f]);
			if(VarEstaciones[NEst].tipo[0]=='R'){
				VarEstaciones[NEst].duracion_turno=(int)atof(&buffer[i]);
			}
			else{
				VarEstaciones[NEst].duracion_turno=0;
			}
			VarEstaciones[NEst].atendiendo=0;
			VarEstaciones[NEst].atendidos=0;
			VarEstaciones[NEst].oscio=0;
			VarEstaciones[NEst].col=0;
			f=0;	
			NEst++;
		}
	}
	f=0;
	fclose(Est);
	ImprimirEstaciones(VarEstaciones,NEst);//Demostracion que se escribio en la variable de estaciones
	AuxCli=fopen("clientes.conf","r");
	while((c=fgetc(AuxCli))!=EOF){
		if(c=='\n')
			NCli++;
	}	
	fclose(AuxCli);
	printf("\ncantidad de clientes: %d\n",NCli);
	cliente 	VarClientes[NCli];
	NCli=0;
	int EstCliente;
	char AuxClien[50]; 
	while((c=fgetc(Cli))!=EOF){
		if(c!='\n'){
			if(c!=' ')
				buffer[f++]=c; // esta copiando una linea del archivo a buffer (caracter a caracter menos los espacios)
		}else{
			buffer[f]='\0';
			for(i=0;buffer[i]!='-';i++);
			buffer[i++]='\0';
			strcpy(VarClientes[NCli].id_cliente,buffer);
			f=i;
			for(i;buffer[i]!='-';i++);
			buffer[i++]='\0';
			 VarClientes[NCli].prioridad=(int)atof(&buffer[f]);
			f=i;
			for(i;buffer[i]!='-';i++);
			buffer[i++]='\0';
			 VarClientes[NCli].tiempo_llegada=(int)atof(&buffer[f]);		
			EstCliente=0;int x;
			do{
				f=i;
				for(i;buffer[i]!=':';i++);
				buffer[i++]='\0';
				for(x=f;x<i;x++)
					AuxClien[x-f]=buffer[x];
				
				VarClientes[NCli].id_estacion[EstCliente]=BuscarEstacion(NEst,VarEstaciones,AuxClien);
				f=i;
				for(i;buffer[i]!='-'&&buffer[i]!='\n';i++);
				if(buffer[i]=='\n')
					x=1;
				else
					buffer[i++]='\0';	
				VarClientes[NCli].duracion[EstCliente]=(int)atof(&buffer[f]);
				EstCliente++;
			}//fin del do 
			while(x!=1);
			VarClientes[NCli].CantEstaciones=EstCliente;//int EstacionActual;//estacion de se encuentra el cliente |struct Clientes *Next;
			VarClientes[NCli].EstacionActual=0;
			VarClientes[NCli].Next=NULL;
			VarClientes[NCli].RR=-1;
			VarClientes[NCli].total_sis=0;
			VarClientes[NCli].total_col=0;
			VarClientes[NCli].total_est=0;
			VarClientes[NCli].salida=0;
			NCli++;
			f=0;
		}//fin del else
	}//fin del while 
	fclose(Cli);
	ImprimirClientes(NEst,VarEstaciones,VarClientes,NCli);
	int a,b,d;
	cliente *OrdenLLegada=NULL,*sig;// OrdenLLegadad cola de clientes en funcion de su tiempo de llegada 
	for(a=0;a<NCli;a++)
		OrdenLLegada=PonerEnTiempo(OrdenLLegada,&VarClientes[a]);
	for(sig=OrdenLLegada;sig;sig=sig->Next)
		if(sig->Next)
			printf("%s->",sig->id_cliente);
		else{
			a=sig->tiempo_llegada;
			printf("%s\n",sig->id_cliente);
			}
	cliente *(Colas[NEst]);
	cliente *(Atendidos[NEst]);
	cliente *reubicar=NULL; 
	for(i=0;i<NEst;i++){
		Colas[i]=NULL;Atendidos[i]=NULL;
	}
	int TiempoDeSimulacion=0;
	float Tiem=0.00;
	do {
		system("clear");
		printf("\t\t\t\t\t\tTiempo: %d\n",TiempoDeSimulacion);
		PasarTiempo(TiempoDeSimulacion,NEst,Atendidos,&reubicar,VarEstaciones);
		while(OrdenLLegada&&(OrdenLLegada->tiempo_llegada==TiempoDeSimulacion)){
			printf("%s->",OrdenLLegada->id_cliente);
			sig=OrdenLLegada->Next;
			Colas[OrdenLLegada->id_estacion[OrdenLLegada->EstacionActual]]=PonerenCola(VarEstaciones[OrdenLLegada->id_estacion[OrdenLLegada->EstacionActual]].tipo,OrdenLLegada,&Colas[OrdenLLegada->id_estacion[OrdenLLegada->EstacionActual]]);
			OrdenLLegada=sig;
		}
		for(;reubicar;){
			sig=reubicar->Next;
			Colas[reubicar->id_estacion[reubicar->EstacionActual]]=PonerenCola(VarEstaciones[reubicar->id_estacion[reubicar->EstacionActual]].tipo,reubicar,&Colas[reubicar->id_estacion[reubicar->EstacionActual]]);
			reubicar=sig;	
		}
		for(b=0;b<NEst;b++){
			if(Colas[b]){
				while(Colas[b] && ((VarEstaciones[b].capacidad)>(VarEstaciones[b].atendiendo))){
					cliente *persona=Colas[b];
					Colas[b]=Colas[b]->Next;
					if(VarEstaciones[b].tipo[0]=='R')
							persona->RR=VarEstaciones[b].duracion_turno;
					Atendidos[b]=PonerEnAtendidos(persona,&Atendidos[b],&VarEstaciones[b].atendiendo);
				}
			}
		}
		ImprimirCola(NEst,VarEstaciones,Colas,Atendidos);TiempoDeSimulacion++;Tiem++;
		usleep(tiempo*1000);
	}
	while(FinSimulacion(VarClientes,NCli));
	FILE *info;
	archivo(info,TiempoDeSimulacion,NEst,NCli,Tiem,VarEstaciones,VarClientes);
	printf("%c[%dm",0x1B,0);		
	return 0;
}
