#include "processData.h"
// Trước khi return trả về delete đi con trỏ p
enum CodeValue
{
	sdCode,
	cdCode,
	slCode,
	insCode,
	delCode,
	updCode,
	obCode,
	cbCode,
	osCode,
	csCode
};

static map<string, CodeValue> s_mapCodeValues = {
	{"SD", sdCode},
	{"CD", cdCode},
	{"SL", slCode},
	{"INS", insCode},
	{"DEL", delCode},
	{"UPD", updCode},
	{"OB", obCode},
	{"CB", cbCode},
	{"OS", osCode},
	{"CS", csCode}};

ProcessData::ProcessData()
{
	pList = new List();
	pID = new listID();
}
ProcessData::~ProcessData()
{
}
// Check etablish deposit
int flagDeposit = 0;

// Split
int ProcessData::split(string line, string *&sp)
{
	sp = new string[MAXSIZECODE];
	const string SEP = " ";
	unsigned int pos=0, lastpos = 0, idx = 0;
	while (pos<= line.size() )
	{
		if (idx==0)
		{
			pos = line.find(SEP, lastpos);
			sp[idx] = line.substr(lastpos, pos - lastpos);
			++idx;
			lastpos = pos;	
		}
		else
		{
			pos = line.find(SEP, lastpos+1);
			sp[idx] = line.substr(lastpos+1, pos - lastpos-1);
			++idx;
			lastpos = pos;
		}
	}
	return idx;
}

int ProcessData::process(string line)
{

	string *p = new string[MAXSIZECODE];
	int n = ProcessData::split(line, p);
	if (n <= 0)
	{
		delete[] p;
		return -1;
	}
	int res = -1;
	try
	{
		switch (s_mapCodeValues[p[0]])
		{
		case sdCode:
		{
			res = this->SD(p,n);
			break;
		}
		case cdCode:
		{
			res = this->CD(p,n);
			break;
		}
		case slCode:
		{
			res = this->SL(p,n);
			break;
		}
		case insCode:
		{
			res = this->INS(p, n);
			break;
		}
		case delCode:
		{
			res = this->DEL(p,n);
			break;
		}
		case updCode:
		{
			res = this->UPD(p,n);
			break;
		}
		case obCode:
		{
			res = this->OB(p,n);
			break;
		}
		case osCode:
		{
			res = this->OS(p,n);
			break;
		}
		case cbCode:
		{
			res = this->CB(p,n);
			break;
		}
		case csCode:
		{
			res = this->CS(p,n);
			break;
		}
		default:
			res = -1;
		}
	}
	catch (invalid_argument iax)
	{
		delete[] p;
		return res;
	}
	delete[] p;
	return res;
}

// Instruction SD
int ProcessData::SD(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == ""){
				return -1;
			}
		}
		if (n>2|| n<2){	
			return -1;
		}
		// Data input
		int MN =  stoi(sp[1]);
		// Configure 
		if (pList->deposit == 0){
			pList->deposit = MN;
			flagDeposit = 1;
			return 1;
		}
		// Thay đổi giảm
		if (MN< pList->deposit){
			pList->deposit = MN;
			return 1;
		}
		// Thay đổi tăng
		if (MN > pList->deposit){
			int flag =0;
			// Kiểm tra có các giao dịch mở lệnh hay không
			for (int i = 0; i < pList->size; i++)
			{
				if (pList->arrID[i] != -27){
					flag =1;
					break;
				}
			}
			// Success
			if (flag==1){
				pList->deposit = MN;
				return 1;
			}
			// Fail
			if (flag==0){
				return -1;
			}
		}
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}
	
}

// Instruction CD
int ProcessData::CD(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	return -1;
		}
		if (n>1)	return -1;
		// Xem lại các phần trả về
		return ceil(pList->deposit);
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}
}

// Instruction SL
int ProcessData::SL(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	return -1;
		}
		if (n>2|| n<2)	return -1;
		if (flagDeposit ==0){
			return -1;
		}
		float LV = stof(sp[1]);
		pList->leverage = LV;
		return ceil(pList->leverage * pList->deposit);
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}

}
/*---------------------------------Coding By Hung -----------------------------*/

// Instruction Insert
int ProcessData::INS(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	return -1;
		}
		if (n>6|| n<6)	return -1;
		// Data In
		string BC = sp[1];
		string QC = sp[2];
		int time =  stoi(sp[3]);
		float BP = stof(sp[4]);
		float AP = stof(sp[5]);
		// Init object
		dataFair dataCF = dataFair(BC,QC);
		currFair *newFair = new currFair(dataCF);
		dataEx dataER = dataEx(time,BP,AP);
		exRate *newRate = new exRate(dataER);
		// No have currFair
		if (pList->head == nullptr){
			pList->head = newFair;
			newFair->root = newRate;
			return pList->head->root->data.time;
		}
		currFair *preFair = nullptr;
		currFair *nowFair =pList->head;
		while (nowFair != nullptr){
			if (nowFair->data.BC == BC && nowFair->data.QC ==QC){
				bool taller = true;
				nowFair->root = insertRec(nowFair->root,dataER,taller);
				return nowFair->root->data.time;
			}
			preFair = nowFair;
			nowFair = nowFair->next;
		}
		// No have currFair satisfy BC and QC 
		if (nowFair == nullptr){
			preFair->next = newFair;
			newFair->root = newRate;
			return newFair->root->data.time;
		}
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}
}

// Instruction Delete
int ProcessData::DEL(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	return -1;
		}
		if (n>5|| n<3)	return -1;
		if (n==5){
			string BC = sp[1];
			string QC = sp[2];
			int timeA = stoi(sp[3]);
			int timeB = stoi(sp[4]);
			if (timeA > timeB)	return -1;
			if (pList->head == nullptr)	return -1;
			currFair *nowFair = pList->head;
			while (nowFair != nullptr)
			{
				if (nowFair->data.BC == BC && nowFair->data.QC == QC){
					if (nowFair->root == nullptr)	return -1;
					int* sp=new int[1000];
					int count=0;
					remove(sp,count,nowFair->root,timeA,timeB);
					for (int i = 0; i < count; i++)
					{
						bool shorter,success;
						nowFair->removeRec(nowFair->root,sp[i],shorter,success);
					}
					
					return nowFair->root->data.time;
				}
				nowFair = nowFair->next;
			}
		}
		else if (n==4){
			string BC = sp[1];
			string QC = sp[2];
			int time = stoi(sp[3]);
			if (pList->head == nullptr)	return -1;
			currFair *nowFair = pList->head;
			while (nowFair != nullptr)
			{
				if (nowFair->data.BC == BC && nowFair->data.QC == QC){
					bool shorter,success;
					nowFair->root = this->removeRec(nowFair->root,time,shorter,success);
					return 	nowFair->root->data.time;
				}
				nowFair = nowFair->next;
			}
		}
		else if (n==3){
			string BC = sp[1];
			string QC = sp[2];
			if (pList->head == nullptr)	return -1;
			currFair *nowFair = pList->head;
			while (nowFair != nullptr)
			{
				if (nowFair->data.BC == BC && nowFair->data.QC == QC){
					clearRec(nowFair->root);
					return -1;
				}
				nowFair = nowFair->next;
			}
		}
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}

}

// Instruction Upload
int ProcessData::UPD(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	return -1;
		}
		 if (n>6||n<6)	return -1;
		// Data in
		string BC = sp[1];
		string QC = sp[2];
		int time = stoi(sp[3]);
		float BP = stof(sp[4]);
		float AP = stof(sp[5]);
		if (BP >AP)	return -1;
		if (pList->head == nullptr)	return -1;
		currFair *nowFair = pList->head;
		while (nowFair != nullptr)
		{
			if (nowFair->data.BC == BC && nowFair->data.QC == QC){
				int result =0;
				upLoad(nowFair->root,time,BP,AP,result);
				if (result ==1)	return nowFair->root->data.time;
				if (result==0)	return -1;	
			}
			nowFair = nowFair->next;
		}
		return  -1;

	}
	catch(const std::exception& e)
	{
		return -1;
	}

}

// Instruction OB
int ProcessData::OB(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	return -1;
		}
		 if (n>6|| n<6)	return -1;
		string BC = sp[1];
		string QC = sp[2];
		if (BC != "USD" && QC !="USD")	return -1;
		int time = stoi(sp[3]);
		float LOT = stof(sp[4]);
		int ID = stoi(sp[5]);
		// Kiểm tra trùng nhau
		if (pList->size ==0){
			pList->arrID[pList->size]=ID;
			pList->size=1;
		}
		else
		{
			for (int i = 0; i < pList->size; i++)
			{
				if (ID == pList->arrID[i])	return -1;
			}
			pList->size+=1;
			pList->arrID[pList->size-1] = ID;
		}
		if (pList->head == nullptr)	return -1;
		currFair *nowFair = pList->head;
		while (nowFair != nullptr)
		{
			if 	(nowFair->data.BC == BC && nowFair->data.QC == QC){
				exRate *save = new exRate();
				findNode(nowFair->root,time,save);
				float buyCost =save->data.AP;
				float sellCost=save->data.BP;
				int BorS =0;
				nodeID *newID = new nodeID(ID,buyCost,LOT,BorS,BC,QC);
				if (pID->phead == nullptr){
					pID->phead = newID;
					pID->count++;
				}
				else{
					nodeID *nowNode = pID->phead;
					while (nowNode->next != nullptr)
					{
						nowNode = nowNode->next;
					}
					nowNode->next = newID;
					pID->count++;
				}
				if (QC == "USD"){
					float result = buyCost*LOT*100000;
					// Khối lượng giao dịch lớn hơn số tiền có thể giao dịch
					if (result > pList->deposit*pList->leverage){
						pList->arrID[pList->size-1] = 0;
						pList->size -=1;
						return -1;
					}
					return ceil(result);
				}
				else if (BC == "USD"){
					float result = (buyCost*LOT*100000)/sellCost;
					if (result > pList->deposit*pList->leverage){	
						pList->arrID[pList->size-1] = 0;
						pList->size -=1;
						return -1;
					}
					return ceil(result);
				}		
			}
			nowFair = nowFair->next;
		}
		return -1;

	}
	catch(const std::exception& e)
	{
		return -1;
	}
}

// Instruction OS
int ProcessData::OS(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	return -1;
		}
		if (n>6||n<6)	return -1;
		string BC = sp[1];
		string QC = sp[2];
		if (BC != "USD" && QC !="USD")	return -1;
		int time = stoi(sp[3]);
		float LOT = stof(sp[4]);
		int ID = stoi(sp[5]);
		// Kiểm tra các ID trùng nhau
		if (pList->size ==0){
			pList->arrID[pList->size]=ID;
			pList->size=1;
		}
		else
		{
			for (int i = 0; i < pList->size; i++)
			{
				if (ID == pList->arrID[i])	return -1;
			}
			pList->size+=1;
			pList->arrID[pList->size-1] = ID;
		}
		if (pList->head == nullptr)	return -1;
		currFair *nowFair = pList->head;
		while (nowFair != nullptr)
		{
			if 	(nowFair->data.BC == BC && nowFair->data.QC == QC){
				exRate *save = new exRate();
				findNode(nowFair->root,time,save);
				float sellCost =save->data.BP;
				float buyCost =save->data.AP;
				int BorS=1;
				nodeID *newID = new nodeID(ID,sellCost,LOT,BorS,BC,QC);
					if (pID->phead == nullptr){
						pID->phead = newID;
						pID->count++;
					}
					else{
						nodeID *nowNode = pID->phead;
						while (nowNode->next != nullptr)
						{
							nowNode = nowNode->next;
						}
						nowNode->next = newID;
						pID->count++;
					}
					if (QC == "USD"){
						float result = sellCost*LOT*100000;
						// Khối lượng giao dịch lớn hơn số tiền có thể giao dịch
						if (result >pList->deposit*pList->leverage){	
							pList->arrID[pList->size-1] = 0;
							pList->size -=1;
							return -1;
						}
						return ceil(result);
					}
					else if (BC == "USD"){
						float result = (sellCost*LOT*100000)/buyCost;
						if (result > pList->deposit*pList->leverage){	
							pList->arrID[pList->size-1] = 0;
							pList->size -=1;
							return -1;
						}
						return ceil(result);
					}
				
			}
			nowFair = nowFair->next;
		}
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}
}
// Instruction CB
int ProcessData::CB(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	return -1;
		}
		if (n>3||n<3)	return -1;
		// Data In
		int time = stoi(sp[1]);
		int ID = stoi(sp[2]);
		int flag =0;
		// Kiểm tra ID đã có hay chưa
		for (int i=0;i<pList->size;i++){
			if (pList->arrID[i]== ID){
				flag =1;
				pList->arrID[i] = -27;
				break;
			}
		}
		if (flag ==0)	return -1;
		if (pID->phead == nullptr)	return -1;
		nodeID *nowID = pID->phead;
		while (nowID !=nullptr)
		{
			if (nowID->ID == ID && nowID->BorS == 1 ){
				string BC = nowID->BC;
				string QC = nowID->QC;
				currFair *nowFair = pList->head;
				while (nowFair != nullptr)
				{
					if (nowFair->data.BC == BC && nowFair->data.QC == QC){
						exRate *save = new exRate();
						findNode(nowFair->root,time,save);
						if (QC == "USD"){
							float moneyBuy = save->data.AP * nowID->scale* 100000;
							float moneySell = nowID->cost * nowID->scale*100000;
							float profit = moneySell - moneyBuy;
							pList->deposit += profit;
							return ceil(profit);
						}
						else if (BC == "USD"){
							float moneyBuy = nowID->scale* 100000;
							float moneySell =  (nowID->cost * nowID->scale*100000)/ save->data.AP;
							float profit =moneySell-moneyBuy;
							pList->deposit += profit;
							return ceil(profit);
						}
					}
					nowFair = nowFair->next;
				}
				
			}
			nowID = nowID->next;
		}
		return -1;
		
	}
	catch(const std::exception& e)
	{
		return -1;
	}
}

// Instruction CS
int ProcessData::CS(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	return -1;
		}
		 if (n>3||n<3)	return -1;
		// Data In
		int time = stoi(sp[1]);
		int ID = stoi(sp[2]);
		int flag1 =0;
		// Kiểm tra ID đã có hay chưa
		for (int i=0;i<pList->size;i++){
			if (ID== pList->arrID[i]){
				flag1 =1;
				pList->arrID[i] = -27;
				break;
			}
		}
		if (flag1 ==0)	return -1;
		if (pID->phead == nullptr)	return -1;
		nodeID *nowID = pID->phead;
		while (nowID !=nullptr)
		{
			if (nowID->ID == ID && nowID->BorS == 0 ){
				string BC = nowID->BC;
				string QC = nowID->QC;
				currFair *nowFair = pList->head;
				while (nowFair != nullptr)
				{
					if (nowFair->data.BC == BC && nowFair->data.QC == QC){
						exRate *save = new exRate();
						findNode(nowFair->root,time,save);
						if (QC == "USD"){
							float moneySell = save->data.BP *nowID->scale*100000;
							float moneyBuy = nowID->cost * nowID->scale * 100000;
							float profit = moneySell-moneyBuy;
							pList->deposit += profit;
							return ceil(profit);
						}
						else if (BC == "USD"){
							float moneySell = (save->data.BP * nowID->scale*100000)/save->data.AP;
							float moneyBuy = nowID->scale*100000;
							float profit = moneySell - moneyBuy;
							pList->deposit += profit;
							return ceil(profit);
						}
					}
					nowFair = nowFair->next;
				}
				
			}
			nowID = nowID->next;
		}
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}
}